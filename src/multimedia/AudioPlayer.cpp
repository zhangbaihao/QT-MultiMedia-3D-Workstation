#include "AudioPlayer.h"

#include <QAudio>
#include <QAudioSink>
#include <QDebug>
#include <QMediaDevices>
#include <QUrl>

#include <libavutil/opt.h>

static AVSampleFormat avSampleFormatForQt(const QAudioFormat &fmt)
{
    switch (fmt.sampleFormat()) {
    case QAudioFormat::Float:
        return AV_SAMPLE_FMT_FLT;
    case QAudioFormat::Int16:
        return AV_SAMPLE_FMT_S16;
    case QAudioFormat::Int32:
        return AV_SAMPLE_FMT_S32;
    case QAudioFormat::UInt8:
        return AV_SAMPLE_FMT_U8;
    default:
        return AV_SAMPLE_FMT_FLT;
    }
}

AudioPullDevice::AudioPullDevice(AudioPlayer *player)
    : QIODevice(player)
    , m_player(player)
{
}

qint64 AudioPullDevice::readData(char *data, qint64 maxlen)
{
    qDebug() << "AudioPullDevice::readData: called with maxlen=" << maxlen;
    return m_player ? m_player->readAudioData(data, maxlen) : 0;
}

qint64 AudioPullDevice::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return -1;
}

qint64 AudioPullDevice::bytesAvailable() const
{
    // QAudioSink 在调用 readData 之前会先检查 bytesAvailable。
    //
    // 即使队列暂时为空，只要还在播放中就要返回正值，
    // 否则 QAudioSink 认为没有数据了就进入 IdleState 不再拉取。
    //
    // 返回正值 → sink 调用 readData → readAudioData 调用 fillAudioQueue 补充数据
    qint64 queueSize = m_player ? m_player->pcmQueueSize() : 0;
    if (queueSize > 0)
        return queueSize + QIODevice::bytesAvailable();
    if (m_player && m_player->isPlaying() && !m_player->isAtEnd())
        return 4096; // 播放中但队列空：告诉 sink 还有数据要来，触发 readData
    return QIODevice::bytesAvailable();
}

QString AudioPlayer::normalizePath(const QString &path)
{
    if (path.isEmpty())
        return QString();
    const QUrl url(path);
    if (url.isLocalFile())
        return url.toLocalFile();
    if (path.startsWith(QLatin1String("file:"), Qt::CaseInsensitive))
        return QUrl(path).toLocalFile();
    return QUrl::fromUserInput(path).toLocalFile().isEmpty() ? path : QUrl::fromUserInput(path).toLocalFile();
}

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , m_pull(new AudioPullDevice(this))
{
    // 在构造时枚举可用音频设备
    qDebug() << "AudioPlayer: available audio devices:";
    const auto devices = QMediaDevices::audioOutputs();
    for (int i = 0; i < devices.size(); ++i) {
        const auto &dev = devices[i];
        const auto fmt = dev.preferredFormat();
        qDebug() << "  [" << i << "]" << dev.description()
                 << "(rate:" << fmt.sampleRate()
                 << "ch:" << fmt.channelCount()
                 << "fmt:" << fmt.sampleFormat()
                 << "isDefault:" << (dev == QMediaDevices::defaultAudioOutput())
                 << ")";
    }
}

AudioPlayer::~AudioPlayer()
{
    qDebug() << "AudioPlayer::~AudioPlayer() start";
    m_destroying.store(true);
    m_isPlaying = false;

    // 1. 先停止音频输出设备（同步等待，确保没有正在进行的 readData 调用）
    if (m_sink) {
        qDebug() << "AudioPlayer::~AudioPlayer stopping sink, state:" << m_sink->state();
        if (m_sink->state() != QAudio::StoppedState)
            m_sink->stop();
    }

    // 2. 断开 AudioPullDevice 对 AudioPlayer 的引用（stop 后再无回调）
    if (m_pull)
        m_pull->resetPlayer();

    // 3. 关闭拉取设备
    if (m_pull && m_pull->isOpen()) {
        m_pull->close();
    }

    // 4. 关闭 FFmpeg 解码器（在 mutex 保护下）
    closeDecoder();

    qDebug() << "AudioPlayer::~AudioPlayer() done";
    // 注意：m_sink 和 m_pull 都是 AudioPlayer 的子对象（parent=this），
    // 由 Qt 的 parent-child 机制自动销毁，不要手动 delete！
}

QString AudioPlayer::filePath() const
{
    return m_filePath;
}

void AudioPlayer::setFilePath(const QString &path)
{
    if (m_filePath == path)
        return;
    const bool wasPlaying = m_isPlaying;
    if (m_sink) {
        m_sink->stop();
        m_isPlaying = false;
        emit isPlayingChanged();
    }
    m_pull->close();
    closeDecoder();

    m_filePath = path;
    emit filePathChanged();

    if (!m_filePath.isEmpty()) {
        if (!ensureDecoderOpen()) {
            // 无音频流是正常情况（视频文件可能没有音轨），不发出 error 信号
            qDebug() << "AudioPlayer: no audio stream in" << path;
            return;
        }
    }

    if (wasPlaying && !m_filePath.isEmpty())
        play();
}

bool AudioPlayer::isPlaying() const
{
    return m_isPlaying;
}

int AudioPlayer::duration() const
{
    return m_durationMs;
}

int AudioPlayer::position() const
{
    return m_positionMs;
}

void AudioPlayer::setPosition(int positionMs)
{
    {
        QMutexLocker lock(&m_mutex);
        if (!m_fmt || m_audioIndex < 0 || !m_audioStream || !m_dec)
            return;

        const int d = qMax(0, m_durationMs);
        const int clamped = d > 0 ? qBound(0, positionMs, d) : qMax(0, positionMs);
        m_positionMs = clamped;
        m_pcmQueue.clear();
        m_eofDecode = false;

        const int64_t ts = av_rescale_q(m_positionMs, AVRational{1, 1000}, m_audioStream->time_base);
        int ret = av_seek_frame(m_fmt, m_audioIndex, ts, AVSEEK_FLAG_BACKWARD);
        if (ret < 0) {
            const int64_t us = static_cast<int64_t>(m_positionMs) * 1000LL;
            ret = av_seek_frame(m_fmt, -1, us, AVSEEK_FLAG_BACKWARD);
        }
        if (m_dec)
            avcodec_flush_buffers(m_dec);
        if (m_swr)
            swr_init(m_swr);
    }
    emit positionChanged();
}

qreal AudioPlayer::volume() const
{
    return m_volume;
}

void AudioPlayer::setVolume(qreal v)
{
    v = qBound(0.0, v, 1.0);
    if (qFuzzyCompare(m_volume, v))
        return;
    m_volume = v;
    emit volumeChanged();
    if (m_sink) {
        if (m_muted)
            m_sink->setVolume(0.0f);
        else
            m_sink->setVolume(static_cast<float>(m_volume));
    }
}

bool AudioPlayer::muted() const
{
    return m_muted;
}

QString AudioPlayer::audioDevice() const
{
    return m_audioDeviceDesc;
}

void AudioPlayer::setAudioDevice(const QString &description)
{
    if (m_audioDeviceDesc == description)
        return;
    m_audioDeviceDesc = description;
    emit audioDeviceChanged();
}

QStringList AudioPlayer::availableAudioDevices() const
{
    QStringList list;
    const auto devices = QMediaDevices::audioOutputs();
    for (const auto &dev : devices)
        list.append(dev.description());
    return list;
}

QAudioDevice AudioPlayer::findAudioDevice() const
{
    if (m_audioDeviceDesc.isEmpty()) {
        const auto defaultDev = QMediaDevices::defaultAudioOutput();
        qDebug() << "AudioPlayer: using default device:" << defaultDev.description();
        return defaultDev;
    }
    const auto devices = QMediaDevices::audioOutputs();
    for (const auto &dev : devices) {
        if (dev.description() == m_audioDeviceDesc) {
            qDebug() << "AudioPlayer: using specified device:" << m_audioDeviceDesc;
            return dev;
        }
    }
    // 未找到指定设备时回退到默认设备
    qWarning() << "AudioPlayer: device" << m_audioDeviceDesc << "not found, using default";
    return QMediaDevices::defaultAudioOutput();
}

void AudioPlayer::setMuted(bool m)
{
    if (m_muted == m)
        return;
    m_muted = m;
    emit mutedChanged();
    if (m_sink) {
        if (m_muted)
            m_sink->setVolume(0.0f);
        else
            m_sink->setVolume(static_cast<float>(m_volume));
    }
}

void AudioPlayer::play()
{
    if (m_filePath.isEmpty())
        return;

    // 确保解码器打开；如果打开失败（如无音频流的视频），不报错，仅静默跳过
    if (!ensureDecoderOpen()) {
        qDebug() << "AudioPlayer: no audio stream available, skipping audio playback";
        return;
    }

    // 查找音频设备（用户指定的或系统默认）
    const QAudioDevice dev = findAudioDevice();

    if (m_sink) {
        // 重用已有音频输出
        if (m_sink->state() == QAudio::StoppedState) {
            delete m_sink;
            m_sink = nullptr;
        } else if (m_sink->state() == QAudio::SuspendedState) {
            m_sink->resume();
            m_isPlaying = true;
            emit isPlayingChanged();
            return;
        }
    }

    if (!m_sink) {
        qDebug() << "AudioPlayer: using audio device:" << dev.description()
                 << "with format - rate:" << m_outFormat.sampleRate()
                 << "channels:" << m_outFormat.channelCount()
                 << "format:" << m_outFormat.sampleFormat();

        m_sink = new QAudioSink(dev, m_outFormat, this);
        if (!m_sink || m_sink->error() != QAudio::NoError) {
            emit errorOccurred(QStringLiteral("无法创建音频输出设备"));
            return;
        }

        // 验证 QAudioSink 实际使用的格式
        const QAudioFormat actualFmt = m_sink->format();
        qDebug() << "AudioPlayer: sink actual format - rate:" << actualFmt.sampleRate()
                 << "channels:" << actualFmt.channelCount()
                 << "format:" << actualFmt.sampleFormat();

        // 如果实际格式与我们的输出格式不匹配，重新配置 swr
        if (actualFmt.sampleRate() != m_outFormat.sampleRate()
            || actualFmt.channelCount() != m_outFormat.channelCount()
            || actualFmt.sampleFormat() != m_outFormat.sampleFormat()) {
            qWarning() << "AudioPlayer: format mismatch, reconfiguring swr for actual format";
            m_outFormat = actualFmt;
            // 重新设置 swr
            {
                QMutexLocker lock(&m_mutex);
                if (m_swr) {
                    swr_free(&m_swr);
                    m_swr = nullptr;
                }
                if (m_audioStream && m_audioStream->codecpar) {
                    if (!setupSwr(m_audioStream->codecpar)) {
                        qWarning() << "AudioPlayer: failed to reconfigure swr";
                    }
                }
            }
        }

        m_sink->setVolume(m_muted ? 0.0f : static_cast<float>(m_volume));

        // 监听音频状态变化以调试
        connect(m_sink, &QAudioSink::stateChanged, this, [](QAudio::State state) {
            qDebug() << "AudioPlayer::QAudioSink state:" << state;
        });
    }

    if (!m_pull->isOpen()) {
        if (!m_pull->open(QIODevice::ReadOnly)) {
            emit errorOccurred(QStringLiteral("无法打开音频缓冲"));
            return;
        }
    }

    // 在启动 sink 之前预填音频队列，防止立即 underrun
    bool hasData = false;
    {
        QMutexLocker lock(&m_mutex);
        if (m_pcmQueue.isEmpty()) {
            qDebug() << "AudioPlayer: pre-filling audio queue...";
            const int prefillSize = qMax(m_outFormat.bytesForDuration(50000), 16384); // 50ms
            const int oldSize = m_pcmQueue.size();
            fillAudioQueue(prefillSize);
            const int newSize = m_pcmQueue.size();
            qDebug() << "AudioPlayer: pre-fill done, added" << (newSize - oldSize)
                     << "bytes, queue size =" << newSize;
        }
        hasData = !m_pcmQueue.isEmpty();
    }

    if (!hasData) {
        qWarning() << "AudioPlayer: pre-fill produced no data, aborting playback";
        m_sink->stop();
        return;
    }

    qDebug() << "AudioPlayer: starting playback with" << m_pcmQueue.size() << "bytes pre-filled";
    m_sink->start(m_pull);

    // 验证音频状态
    qDebug() << "AudioPlayer: sink state after start:" << m_sink->state()
             << "error:" << m_sink->error();

    // 检查解码器状态
    qDebug() << "AudioPlayer: decoder valid?" << (m_fmt != nullptr)
             << "swr valid?" << (m_swr != nullptr)
             << "codecpar sample_rate:" << (m_audioStream && m_audioStream->codecpar ? m_audioStream->codecpar->sample_rate : -1);

    m_isPlaying = true;
    emit isPlayingChanged();
}

void AudioPlayer::pause()
{
    if (m_sink && m_isPlaying) {
        m_sink->suspend();
        m_isPlaying = false;
        emit isPlayingChanged();
    }
}

void AudioPlayer::stop()
{
    if (m_sink)
        m_sink->stop();
    if (m_pull && m_pull->isOpen())
        m_pull->close();

    {
        QMutexLocker lock(&m_mutex);
        m_pcmQueue.clear();
        m_eofDecode = false;
        m_positionMs = 0;
        if (m_fmt && m_audioStream && m_dec && m_audioIndex >= 0) {
            av_seek_frame(m_fmt, m_audioIndex, 0, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(m_dec);
            if (m_swr)
                swr_init(m_swr);
        }
    }

    if (m_isPlaying) {
        m_isPlaying = false;
        emit isPlayingChanged();
    }
    emit positionChanged();
}

bool AudioPlayer::ensureDecoderOpen()
{
    {
        QMutexLocker lock(&m_mutex);
        if (m_fmt)
            return true;

        const QString local = normalizePath(m_filePath);
        if (local.isEmpty())
            return false;

        qDebug() << "AudioPlayer: opening" << local;

        int ret = avformat_open_input(&m_fmt, local.toUtf8().constData(), nullptr, nullptr);
        if (ret < 0) {
            qWarning() << "AudioPlayer avformat_open_input" << ret;
            return false;
        }

        ret = avformat_find_stream_info(m_fmt, nullptr);
        if (ret < 0) {
            qWarning() << "AudioPlayer avformat_find_stream_info failed";
            closeDecoderUnlocked();
            return false;
        }

        // 打印所有流信息
        qDebug() << "AudioPlayer: found" << m_fmt->nb_streams << "streams";
        for (unsigned i = 0; i < m_fmt->nb_streams; ++i) {
            if (m_fmt->streams[i] && m_fmt->streams[i]->codecpar) {
                qDebug() << "  Stream" << i << "type:" << m_fmt->streams[i]->codecpar->codec_type;
            }
        }

        m_audioIndex = av_find_best_stream(m_fmt, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
        if (m_audioIndex < 0) {
            qDebug() << "AudioPlayer: no audio stream found";
            closeDecoderUnlocked();
            return false;
        }

        m_audioStream = m_fmt->streams[m_audioIndex];
        const AVCodec *codec = avcodec_find_decoder(m_audioStream->codecpar->codec_id);
        if (!codec) {
            qWarning() << "AudioPlayer: no decoder for codec" << m_audioStream->codecpar->codec_id;
            closeDecoderUnlocked();
            return false;
        }

        m_dec = avcodec_alloc_context3(codec);
        if (!m_dec) {
            closeDecoderUnlocked();
            return false;
        }

        ret = avcodec_parameters_to_context(m_dec, m_audioStream->codecpar);
        if (ret < 0) {
            closeDecoderUnlocked();
            return false;
        }

        ret = avcodec_open2(m_dec, codec, nullptr);
        if (ret < 0) {
            qWarning() << "AudioPlayer: avcodec_open2 failed:" << ret;
            closeDecoderUnlocked();
            return false;
        }

        const QAudioDevice dev = findAudioDevice();
        m_outFormat = dev.preferredFormat();
        if (m_outFormat.sampleRate() <= 0)
            m_outFormat.setSampleRate(48000);
        if (m_outFormat.channelCount() <= 0)
            m_outFormat.setChannelCount(2);
        if (m_outFormat.sampleFormat() == QAudioFormat::Unknown)
            m_outFormat.setSampleFormat(QAudioFormat::Int16);

        // 确保输出格式被音频设备支持
        if (!dev.isFormatSupported(m_outFormat)) {
            qWarning() << "AudioPlayer: format not supported, trying to find compatible format";
            // 尝试常用格式
            QAudioFormat testFmt = m_outFormat;
            testFmt.setSampleRate(44100);
            if (dev.isFormatSupported(testFmt)) {
                m_outFormat = testFmt;
            } else {
                testFmt.setSampleRate(48000);
                testFmt.setSampleFormat(QAudioFormat::Int16);
                if (dev.isFormatSupported(testFmt)) {
                    m_outFormat = testFmt;
                }
                // 否则就用原来的格式碰运气
            }
        }

        qDebug() << "AudioPlayer: output format - rate:" << m_outFormat.sampleRate()
                 << "channels:" << m_outFormat.channelCount()
                 << "format:" << m_outFormat.sampleFormat();

        if (!setupSwr(m_audioStream->codecpar)) {
            qWarning() << "AudioPlayer: swr setup failed";
            closeDecoderUnlocked();
            return false;
        }

        if (!m_audioFrame)
            m_audioFrame = av_frame_alloc();
        if (!m_audioFrame) {
            closeDecoderUnlocked();
            return false;
        }

        if (m_fmt->duration != AV_NOPTS_VALUE)
            m_durationMs = static_cast<int>((m_fmt->duration * 1000) / AV_TIME_BASE);
        else
            m_durationMs = 0;

        m_positionMs = 0;
        m_pcmQueue.clear();
        m_eofDecode = false;

        qDebug() << "AudioPlayer: opened successfully, duration:" << m_durationMs << "ms";
    }

    emit durationChanged();
    emit positionChanged();
    return true;
}

void AudioPlayer::closeDecoderUnlocked()
{
    if (m_swr) {
        swr_free(&m_swr);
        m_swr = nullptr;
    }
    if (m_audioFrame) {
        av_frame_free(&m_audioFrame);
        m_audioFrame = nullptr;
    }
    if (m_dec) {
        avcodec_free_context(&m_dec);
        m_dec = nullptr;
    }
    if (m_fmt) {
        avformat_close_input(&m_fmt);
        m_fmt = nullptr;
    }
    m_audioStream = nullptr;
    m_audioIndex = -1;
    m_pcmQueue.clear();
    m_durationMs = 0;
    m_positionMs = 0;
    m_eofDecode = false;
}

void AudioPlayer::closeDecoder()
{
    QMutexLocker lock(&m_mutex);
    closeDecoderUnlocked();
}

bool AudioPlayer::setupSwr(const AVCodecParameters *par)
{
    const AVSampleFormat inSf = static_cast<AVSampleFormat>(par->format);
    const AVSampleFormat outSf = avSampleFormatForQt(m_outFormat);

    AVChannelLayout inLayout;
    memset(&inLayout, 0, sizeof(inLayout));
    if (par->ch_layout.nb_channels > 0) {
        if (av_channel_layout_copy(&inLayout, &par->ch_layout) < 0)
            av_channel_layout_default(&inLayout, par->ch_layout.nb_channels);
    } else {
        av_channel_layout_default(&inLayout, 2);
    }

    AVChannelLayout outLayout;
    memset(&outLayout, 0, sizeof(outLayout));
    av_channel_layout_default(&outLayout, m_outFormat.channelCount());

    SwrContext *swr = nullptr;
    const int ret = swr_alloc_set_opts2(&swr, &outLayout, outSf, m_outFormat.sampleRate(), &inLayout, inSf,
                                        par->sample_rate, 0, nullptr);
    // swr_alloc_set_opts2 复制了 layout，之后可以安全释放
    av_channel_layout_uninit(&inLayout);
    av_channel_layout_uninit(&outLayout);

    if (ret < 0 || !swr) {
        return false;
    }

    if (swr_init(swr) < 0) {
        swr_free(&swr);
        return false;
    }

    m_swr = swr;
    return true;
}

qint64 AudioPlayer::pcmQueueSize() const
{
    QMutexLocker locker(&m_mutex);
    return m_pcmQueue.size();
}

bool AudioPlayer::isAtEnd() const
{
    QMutexLocker locker(&m_mutex);
    return m_eofDecode;
}

qint64 AudioPlayer::readAudioData(char *data, qint64 maxlen)
{
    qDebug() << "AudioPlayer::readAudioData: ENTERED maxlen=" << maxlen
             << "queueSize=" << m_pcmQueue.size()
             << "destroying=" << m_destroying.load()
             << "data=" << (void*)data;

    if (!data || maxlen <= 0 || m_destroying.load()) {
        qWarning() << "AudioPlayer::readAudioData: early return, data=" << (void*)data
                   << "maxlen=" << maxlen << "destroying=" << m_destroying.load();
        return 0;
    }

    qint64 n = 0;

    {
        QMutexLocker locker(&m_mutex);

        // 如果队列中没有足够数据，尝试解码更多
        if (static_cast<qint64>(m_pcmQueue.size()) < maxlen) {
            if (m_fmt && m_dec && m_swr && m_audioFrame && m_audioStream) {
                fillAudioQueue(static_cast<int>(maxlen));
            }
        }

        n = qMin(maxlen, static_cast<qint64>(m_pcmQueue.size()));
        if (n <= 0) {
            qWarning() << "AudioPlayer::readAudioData: returning 0 bytes, queue="
                       << m_pcmQueue.size() << "maxlen=" << maxlen;
            return 0;
        }

        memcpy(data, m_pcmQueue.constData(), static_cast<size_t>(n));
        m_pcmQueue.remove(0, static_cast<int>(n));

        if (m_outFormat.sampleRate() > 0) {
            const int bpf = m_outFormat.bytesForFrames(1);
            if (bpf > 0) {
                const qint64 totalSamples = n / bpf;
                const int deltaMs = static_cast<int>((totalSamples * 1000LL) / m_outFormat.sampleRate());
                m_positionMs = qMin(m_durationMs, m_positionMs + deltaMs);
            }
        }

        qDebug() << "AudioPlayer::readAudioData: returned" << n << "bytes, queue remaining" << m_pcmQueue.size();
    }

    if (n > 0)
        emit positionChanged();
    return n;
}

int AudioPlayer::fillAudioQueue(int targetBytes)
{
    // 填充音频队列到 targetBytes 字节
    if (!m_fmt || !m_dec || !m_swr || !m_audioFrame || !m_audioStream)
        return m_pcmQueue.size();

    if (!m_audioStream->codecpar || m_audioStream->codecpar->sample_rate <= 0)
        return m_pcmQueue.size();

    const int inRate = m_audioStream->codecpar->sample_rate;
    const int outRate = m_outFormat.sampleRate();
    int bpf = m_outFormat.bytesForFrames(1);
    if (bpf <= 0)
        bpf = m_outFormat.bytesPerSample() * m_outFormat.channelCount();
    if (bpf <= 0)
        bpf = 4;

    if (static_cast<qint64>(m_pcmQueue.size()) >= targetBytes)
        return m_pcmQueue.size();

    AVPacket *pkt = av_packet_alloc();
    if (!pkt)
        return m_pcmQueue.size();

    int guard = 0;
    while (static_cast<qint64>(m_pcmQueue.size()) < targetBytes && !m_eofDecode) {
        if (++guard > 4096) {
            qWarning() << "AudioPlayer fillAudioQueue: guard hit, queue=" << m_pcmQueue.size();
            m_eofDecode = true;
            break;
        }

        // 从解码器取已解码帧
        for (;;) {
            int ret = avcodec_receive_frame(m_dec, m_audioFrame);
            if (ret == AVERROR(EAGAIN))
                break;
            if (ret == AVERROR_EOF) {
                qDebug() << "AudioPlayer fillAudioQueue: decoder EOF";
                m_eofDecode = true;
                break;
            }
            if (ret < 0) {
                qDebug() << "AudioPlayer fillAudioQueue: receive error" << ret;
                break;
            }

            const int nbSamples = m_audioFrame->nb_samples;
            if (nbSamples <= 0) {
                av_frame_unref(m_audioFrame);
                continue;
            }

            const int64_t delay = swr_get_delay(m_swr, outRate);
            int64_t maxOut = av_rescale_rnd(delay + nbSamples, outRate, inRate, AV_ROUND_UP);
            if (maxOut <= 0) maxOut = nbSamples;

            QByteArray buf(static_cast<int>(maxOut) * bpf + 256, Qt::Uninitialized);
            uint8_t *outPtr = reinterpret_cast<uint8_t *>(buf.data());
            const int converted = swr_convert(m_swr, &outPtr, static_cast<int>(maxOut),
                                              const_cast<const uint8_t **>(m_audioFrame->data), nbSamples);
            av_frame_unref(m_audioFrame);

            if (converted > 0) {
                m_pcmQueue.append(buf.constData(), converted * bpf);
            } else if (converted < 0) {
                qWarning() << "AudioPlayer fillAudioQueue: swr_convert failed" << converted;
            }

            if (static_cast<qint64>(m_pcmQueue.size()) >= targetBytes)
                break;
        }

        if (static_cast<qint64>(m_pcmQueue.size()) >= targetBytes || m_eofDecode)
            break;

        // 读取更多数据包
        int ret = av_read_frame(m_fmt, pkt);
        if (ret < 0) {
            if (ret == AVERROR_EOF) {
                qDebug() << "AudioPlayer fillAudioQueue: file EOF";
                m_eofDecode = true;
            }
            break;
        }

        if (pkt->stream_index != m_audioIndex) {
            av_packet_unref(pkt);
            continue;
        }

        ret = avcodec_send_packet(m_dec, pkt);
        av_packet_unref(pkt);
        if (ret < 0 && ret != AVERROR(EAGAIN)) {
            qDebug() << "AudioPlayer fillAudioQueue: send_packet error" << ret;
            continue;
        }
    }

    av_packet_free(&pkt);
    return m_pcmQueue.size();
}
