#include "VideoPlayer.h"
#include "VideoFrameImageProvider.h"

#include <QDebug>
#include <QUrl>

VideoFrameImageProvider *VideoPlayer::s_frameProvider = nullptr;

void VideoPlayer::setFrameProvider(VideoFrameImageProvider *provider)
{
    s_frameProvider = provider;
}

QString VideoPlayer::normalizePath(const QString &path)
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

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject(parent)
    , m_decoder(new FFmpegDecoder(this))
    , m_playbackTimer(new QTimer(this))
    , m_filePath()
    , m_isPlaying(false)
    , m_duration(0)
    , m_position(0)
    , m_frameRate(25.0)  // 默认 25fps
    , m_playbackRate(1.0)
    , m_frameSequence(0)
    , m_afterSeek(false)
    , m_lastFramePts(-1)
{
    connect(m_decoder, &FFmpegDecoder::errorOccurred, this, &VideoPlayer::onDecoderError);
    connect(m_playbackTimer, &QTimer::timeout, this, &VideoPlayer::onPlaybackTimer);
}

VideoPlayer::~VideoPlayer()
{
    qDebug() << "VideoPlayer::~VideoPlayer() start";
    m_destroying.store(true);
    m_isPlaying = false;
    stopPlayback();
    if (m_decoder) {
        m_decoder->closeFile();
    }
    qDebug() << "VideoPlayer::~VideoPlayer() done";
}

QString VideoPlayer::filePath() const
{
    return m_filePath;
}

void VideoPlayer::setFilePath(const QString &path)
{
    if (m_filePath == path)
        return;

    m_filePath = path;
    emit filePathChanged();

    stopPlayback();
    m_position = 0;
    m_duration = 0;
    // 保持 m_frameRate 不变（保留默认值 25.0），openFile 成功后会覆盖
    emit positionChanged();
    emit durationChanged();

    if (m_decoder)
        m_decoder->closeFile();

    const QString local = normalizePath(path);
    if (local.isEmpty()) {
        emit videoGeometryChanged();
        return;
    }

    if (m_decoder && m_decoder->openFile(local)) {
        m_duration = static_cast<int>(m_decoder->duration());
        m_frameRate = m_decoder->frameRate();
        emit durationChanged();
        emit videoGeometryChanged();
        qDebug() << "VideoPlayer: opened" << local << "duration" << m_duration << "fps" << m_frameRate;
    } else {
        // 文件打开失败时使用默认帧率
        m_frameRate = 25.0;
        emit errorOccurred(QStringLiteral("无法打开视频文件"));
    }
}

bool VideoPlayer::isPlaying() const
{
    return m_isPlaying;
}

int VideoPlayer::duration() const
{
    return m_duration;
}

int VideoPlayer::position() const
{
    return m_position;
}

void VideoPlayer::setPosition(int position)
{
    if (m_destroying.load() || !m_decoder || !m_decoder->isOpen())
        return;

    const int d = qMax(0, m_duration);
    const int clamped = d > 0 ? qBound(0, position, d) : qMax(0, position);
    if (m_position == clamped && !m_afterSeek)
        return;

    m_position = clamped;
    m_decoder->seek(static_cast<qint64>(m_position));
    m_afterSeek = true;
    m_lastFramePts = -1;
    emit positionChanged();

    // 即使暂停状态也解码一帧并刷新画面，让用户看到 seek 后的位置
    if (!m_isPlaying) {
        const QImage frame = m_decoder->decodeNextFrame();
        if (!frame.isNull()) {
            pushFrame(frame);
            m_afterSeek = false;
        }
    }
}

qreal VideoPlayer::playbackRate() const
{
    return m_playbackRate;
}

void VideoPlayer::setPlaybackRate(qreal rate)
{
    rate = qBound(0.25, rate, 4.0);
    if (qFuzzyCompare(m_playbackRate, rate))
        return;
    m_playbackRate = rate;
    emit playbackRateChanged();
    if (m_isPlaying) {
        stopPlayback();
        startPlayback();
    }
}

int VideoPlayer::videoWidth() const
{
    return m_decoder ? m_decoder->width() : 0;
}

int VideoPlayer::videoHeight() const
{
    return m_decoder ? m_decoder->height() : 0;
}

int VideoPlayer::frameSequence() const
{
    return m_frameSequence;
}

double VideoPlayer::frameRate() const
{
    return m_frameRate;
}

void VideoPlayer::play()
{
    if (m_isPlaying || m_filePath.isEmpty() || !m_decoder || !m_decoder->isOpen())
        return;
    startPlayback();
    m_isPlaying = true;
    emit isPlayingChanged();
}

void VideoPlayer::pause()
{
    if (!m_isPlaying)
        return;
    stopPlayback();
    m_isPlaying = false;
    emit isPlayingChanged();
}

void VideoPlayer::stop()
{
    stopPlayback();
    if (m_decoder && m_decoder->isOpen())
        m_decoder->seek(0);
    m_position = 0;
    if (m_isPlaying) {
        m_isPlaying = false;
        emit isPlayingChanged();
    }
    emit positionChanged();
}

void VideoPlayer::onDecoderError(const QString &error)
{
    qWarning() << "VideoPlayer decoder:" << error;
    emit errorOccurred(error);
    stopPlayback();
    m_isPlaying = false;
    emit isPlayingChanged();
}

void VideoPlayer::pushFrame(const QImage &frame)
{
    if (s_frameProvider)
        s_frameProvider->updateFrame(frame);
    emit frameAvailable(frame);
    ++m_frameSequence;
    emit frameSequenceChanged();
}

void VideoPlayer::onPlaybackTimer()
{
    if (m_destroying.load() || !m_decoder || !m_decoder->isOpen()) {
        pause();
        return;
    }

    const QImage frame = m_decoder->decodeNextFrame();
    if (!frame.isNull()) {
        pushFrame(frame);

        // Seek 后的首帧不递增 position（position 由 setPosition 已设好）
        if (m_afterSeek) {
            m_afterSeek = false;
        } else {
            // 基于帧率的近似位置推算
            const double effectiveFps = qMax(1.0, m_frameRate * qMax(0.25, static_cast<double>(m_playbackRate)));
            const int frameMs = qMax(1, static_cast<int>(1000.0 / effectiveFps));
            m_position = qMin(m_duration, m_position + frameMs);
        }
        emit positionChanged();

        // 播放结束检测
        if (m_duration > 0 && m_position >= m_duration) {
            stopPlayback();
            m_isPlaying = false;
            emit isPlayingChanged();
            // 停在最后一帧
        }
    } else if (m_decoder->isAtEnd()) {
        stopPlayback();
        m_isPlaying = false;
        emit isPlayingChanged();
    }
}

int VideoPlayer::calculateFrameInterval() const
{
    // 有效帧率 = 实际帧率 × 播放倍速，最低 0.5fps，最高 120fps
    const double effectiveFps = qMax(0.5, m_frameRate * qMax(0.25, static_cast<double>(m_playbackRate)));
    return qBound(8, static_cast<int>(1000.0 / effectiveFps), 2000);
}

void VideoPlayer::startPlayback()
{
    const int interval = calculateFrameInterval();
    m_playbackTimer->start(interval);
}

void VideoPlayer::stopPlayback()
{
    m_playbackTimer->stop();
}
