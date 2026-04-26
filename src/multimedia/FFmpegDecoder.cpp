#include "FFmpegDecoder.h"
#include <QDebug>

extern "C" {
#include <libavutil/imgutils.h>
}

FFmpegDecoder::FFmpegDecoder(QObject *parent)
    : QObject(parent)
    , m_formatContext(nullptr)
    , m_videoCodecContext(nullptr)
    , m_frame(nullptr)
    , m_frameRGB(nullptr)
    , m_swsContext(nullptr)
    , m_videoStreamIndex(-1)
    , m_frameRate(0.0)
    , m_duration(0)
    , m_isOpen(false)
    , m_eof(false)
{
    initializeFFmpeg();
}

FFmpegDecoder::~FFmpegDecoder()
{
    QMutexLocker locker(&m_mutex);
    qDebug() << "FFmpegDecoder::~FFmpegDecoder() start";
    closeFileUnlocked();
    cleanup();
    qDebug() << "FFmpegDecoder::~FFmpegDecoder() done";
}

void FFmpegDecoder::initializeFFmpeg()
{
    avformat_network_init();
}

void FFmpegDecoder::cleanup()
{
    // 资源由 closeFileUnlocked 释放；此处保留为兼容旧调用路径
}

bool FFmpegDecoder::openFile(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    closeFileUnlocked();

    m_eof = false;

    int ret = avformat_open_input(&m_formatContext, filePath.toUtf8().constData(), nullptr, nullptr);
    if (ret < 0) {
        emit errorOccurred(QStringLiteral("无法打开文件: %1").arg(filePath));
        return false;
    }

    ret = avformat_find_stream_info(m_formatContext, nullptr);
    if (ret < 0) {
        emit errorOccurred(QStringLiteral("无法获取流信息"));
        closeFileUnlocked();
        return false;
    }

    m_videoStreamIndex = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (m_videoStreamIndex < 0) {
        emit errorOccurred(QStringLiteral("未找到视频流"));
        closeFileUnlocked();
        return false;
    }

    if (!setupVideoCodec()) {
        closeFileUnlocked();
        return false;
    }

    AVStream *videoStream = m_formatContext->streams[m_videoStreamIndex];
    m_frameRate = av_q2d(videoStream->avg_frame_rate);
    if (m_frameRate <= 0.0 || m_frameRate > 1000.0)
        m_frameRate = av_q2d(videoStream->r_frame_rate);
    if (m_frameRate <= 0.0 || m_frameRate > 1000.0)
        m_frameRate = 25.0;

    if (m_formatContext->duration != AV_NOPTS_VALUE)
        m_duration = (m_formatContext->duration * 1000) / AV_TIME_BASE;
    else
        m_duration = 0;

    m_isOpen = true;
    qDebug() << "FFmpegDecoder opened" << filePath << width() << "x" << height() << "fps" << m_frameRate << "duration" << m_duration;
    return true;
}

void FFmpegDecoder::closeFileUnlocked()
{
    if (m_swsContext) {
        sws_freeContext(m_swsContext);
        m_swsContext = nullptr;
    }
    if (m_frameRGB) {
        av_frame_free(&m_frameRGB);
        m_frameRGB = nullptr;
    }
    if (m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
    if (m_videoCodecContext) {
        avcodec_free_context(&m_videoCodecContext);
        m_videoCodecContext = nullptr;
    }
    if (m_formatContext) {
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
    }

    m_videoStreamIndex = -1;
    m_frameRate = 0.0;
    m_duration = 0;
    m_isOpen = false;
    m_eof = false;
}

void FFmpegDecoder::closeFile()
{
    QMutexLocker locker(&m_mutex);
    closeFileUnlocked();
}

bool FFmpegDecoder::isOpen() const
{
    return m_isOpen;
}

bool FFmpegDecoder::isAtEnd() const
{
    return m_eof;
}

int FFmpegDecoder::width() const
{
    return m_videoCodecContext ? m_videoCodecContext->width : 0;
}

int FFmpegDecoder::height() const
{
    return m_videoCodecContext ? m_videoCodecContext->height : 0;
}

double FFmpegDecoder::frameRate() const
{
    return m_frameRate;
}

qint64 FFmpegDecoder::duration() const
{
    return m_duration;
}

bool FFmpegDecoder::seek(qint64 positionMs)
{
    QMutexLocker locker(&m_mutex);

    if (!m_isOpen || !m_formatContext || m_videoStreamIndex < 0)
        return false;

    m_eof = false;

    AVStream *st = m_formatContext->streams[m_videoStreamIndex];
    const int64_t ts = av_rescale_q(positionMs, AVRational{1, 1000}, st->time_base);
    int ret = av_seek_frame(m_formatContext, m_videoStreamIndex, ts, AVSEEK_FLAG_BACKWARD);
    if (ret < 0) {
        const int64_t us = positionMs * 1000LL;
        ret = av_seek_frame(m_formatContext, -1, us, AVSEEK_FLAG_BACKWARD);
    }
    if (ret < 0) {
        emit errorOccurred(QStringLiteral("Seek 失败"));
        return false;
    }

    if (m_videoCodecContext)
        avcodec_flush_buffers(m_videoCodecContext);

    return true;
}

QImage FFmpegDecoder::decodeNextFrame()
{
    QMutexLocker locker(&m_mutex);

    if (!m_isOpen || !m_formatContext || !m_videoCodecContext || !m_frame || !m_frameRGB)
        return QImage();

    AVPacket *packet = av_packet_alloc();
    if (!packet)
        return QImage();

    int ret = 0;
    while ((ret = av_read_frame(m_formatContext, packet)) >= 0) {
        if (packet->stream_index != m_videoStreamIndex) {
            av_packet_unref(packet);
            continue;
        }

        ret = avcodec_send_packet(m_videoCodecContext, packet);
        av_packet_unref(packet);
        if (ret < 0)
            continue;

        ret = avcodec_receive_frame(m_videoCodecContext, m_frame);
        if (ret == AVERROR(EAGAIN))
            continue;
        if (ret == AVERROR_EOF) {
            m_eof = true;
            av_packet_free(&packet);
            return QImage();
        }
        if (ret < 0)
            continue;

        const QImage image = convertFrameToImage(m_frame);
        av_packet_free(&packet);
        return image;
    }

    if (ret == AVERROR_EOF || ret < 0)
        m_eof = true;
    av_packet_free(&packet);
    return QImage();
}

bool FFmpegDecoder::hasVideo() const
{
    return m_videoStreamIndex >= 0;
}

bool FFmpegDecoder::setupVideoCodec()
{
    AVStream *videoStream = m_formatContext->streams[m_videoStreamIndex];
    const AVCodec *codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (!codec) {
        emit errorOccurred(QStringLiteral("未找到合适的视频解码器"));
        return false;
    }

    m_videoCodecContext = avcodec_alloc_context3(codec);
    if (!m_videoCodecContext) {
        emit errorOccurred(QStringLiteral("无法分配解码器上下文"));
        return false;
    }

    int ret = avcodec_parameters_to_context(m_videoCodecContext, videoStream->codecpar);
    if (ret < 0) {
        emit errorOccurred(QStringLiteral("无法复制解码器参数"));
        return false;
    }

    ret = avcodec_open2(m_videoCodecContext, codec, nullptr);
    if (ret < 0) {
        emit errorOccurred(QStringLiteral("无法打开解码器"));
        return false;
    }

    m_frame = av_frame_alloc();
    m_frameRGB = av_frame_alloc();
    if (!m_frame || !m_frameRGB) {
        emit errorOccurred(QStringLiteral("无法分配帧"));
        return false;
    }

    const int w = m_videoCodecContext->width;
    const int h = m_videoCodecContext->height;
    // 使用 BGRA 32-bit 格式：字节顺序 B,G,R,A（小端），与 QImage::Format_ARGB32 兼容
    const AVPixelFormat targetFormat = AV_PIX_FMT_BGRA;

    // 使用 av_frame_get_buffer 正确设置 data / linesize / buf，
    // 这样 av_frame_free 能自动管理缓冲区的生命周期，避免手动管理导致的内存问题
    m_frameRGB->format = targetFormat;
    m_frameRGB->width = w;
    m_frameRGB->height = h;
    int retBuf = av_frame_get_buffer(m_frameRGB, 0);
    if (retBuf < 0) {
        emit errorOccurred(QStringLiteral("无法分配 RGB 帧缓冲"));
        return false;
    }

    // 确保像素格式有效
    AVPixelFormat srcPixFmt = m_videoCodecContext->pix_fmt;
    if (srcPixFmt == AV_PIX_FMT_NONE) {
        qWarning() << "FFmpegDecoder: unknown pixel format, assuming AV_PIX_FMT_YUV420P";
        srcPixFmt = AV_PIX_FMT_YUV420P;
    }

    m_swsContext = sws_getContext(w, h, srcPixFmt, w, h, targetFormat, SWS_BILINEAR,
                                  nullptr, nullptr, nullptr);
    if (!m_swsContext) {
        emit errorOccurred(QStringLiteral("无法创建缩放上下文"));
        return false;
    }

    return true;
}

QImage FFmpegDecoder::convertFrameToImage(AVFrame *frame)
{
    if (!frame || !m_frameRGB || !m_swsContext || !m_videoCodecContext)
        return QImage();

    const int w = m_videoCodecContext->width;
    const int h = m_videoCodecContext->height;
    if (w <= 0 || h <= 0)
        return QImage();

    sws_scale(m_swsContext, frame->data, frame->linesize, 0, h, m_frameRGB->data,
               m_frameRGB->linesize);

    if (!m_frameRGB->data[0] || m_frameRGB->linesize[0] <= 0)
        return QImage();

    // 使用 Format_ARGB32 匹配 AV_PIX_FMT_BGRA 字节顺序（小端: B,G,R,A）
    QImage image(m_frameRGB->data[0], w, h, m_frameRGB->linesize[0], QImage::Format_ARGB32);
    if (image.isNull())
        return QImage();
    return image.copy();
}
