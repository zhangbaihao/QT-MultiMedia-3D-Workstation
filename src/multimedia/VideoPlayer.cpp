#include "VideoPlayer.h"
#include <QDebug>
#include <QVideoFrame>
#include <QImage>

VideoPlayer::VideoPlayer(QObject *parent)
    : QMediaPlayer(parent)
    , m_isPlaying(false)
    , m_duration(0)
    , m_position(0)
    , m_videoSink(nullptr)
{
    // 连接信号
    connect(this, &QMediaPlayer::mediaStatusChanged, this, &VideoPlayer::onMediaStatusChanged);
    connect(this, &QMediaPlayer::positionChanged, this, &VideoPlayer::onPositionChanged);
    connect(this, &QMediaPlayer::durationChanged, this, &VideoPlayer::onDurationChanged);
    
    qDebug() << "VideoPlayer initialized";
}

VideoPlayer::~VideoPlayer()
{
    stop();
}

QString VideoPlayer::filePath() const
{
    return m_filePath;
}

void VideoPlayer::setFilePath(const QString &path)
{
    if (m_filePath != path)
    {
        m_filePath = path;
        emit filePathChanged();
        
        // 停止当前播放
        stop();
        
        // 设置媒体源
        setSource(QUrl::fromLocalFile(m_filePath));
        
        qDebug() << "File set:" << m_filePath;
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
    if (m_position != position)
    {
        QMediaPlayer::setPosition(position);
    }
}

QVideoSink* VideoPlayer::videoSink() const
{
    return m_videoSink;
}

void VideoPlayer::setVideoSink(QVideoSink* sink)
{
    if (m_videoSink != sink)
    {
        // 断开旧的连接
        if (m_videoSink)
        {
            disconnect(m_videoSink, &QVideoSink::videoFrameChanged, this, &VideoPlayer::onVideoFrameChanged);
        }
        
        m_videoSink = sink;
        
        // 连接新的视频帧信号
        if (m_videoSink)
        {
            connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &VideoPlayer::onVideoFrameChanged);
        }
        
        emit videoSinkChanged();
        qDebug() << "Video sink set";
    }
}

void VideoPlayer::play()
{
    if (!m_isPlaying && !m_filePath.isEmpty())
    {
        QMediaPlayer::play();
        m_isPlaying = true;
        emit isPlayingChanged();
        qDebug() << "Video playing";
    }
}

void VideoPlayer::pause()
{
    if (m_isPlaying)
    {
        QMediaPlayer::pause();
        m_isPlaying = false;
        emit isPlayingChanged();
        qDebug() << "Video paused";
    }
}

void VideoPlayer::stop()
{
    if (m_isPlaying || playbackState() == QMediaPlayer::PlayingState)
    {
        QMediaPlayer::stop();
        m_isPlaying = false;
        m_position = 0;
        emit isPlayingChanged();
        emit positionChanged();
        qDebug() << "Video stopped";
    }
}

void VideoPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        stop();
    }
}

void VideoPlayer::onPositionChanged(qint64 position)
{
    m_position = static_cast<int>(position);
    emit positionChanged();
}

void VideoPlayer::onDurationChanged(qint64 duration)
{
    m_duration = static_cast<int>(duration);
    emit durationChanged();
    qDebug() << "Video duration:" << m_duration << "ms";
}

void VideoPlayer::onVideoFrameChanged(const QVideoFrame &frame)
{
    if (frame.isValid())
    {
        QVideoFrame cloneFrame(frame);
        if (cloneFrame.map(QVideoFrame::ReadOnly))
        {
            // 获取视频帧的宽度和高度
            int width = cloneFrame.width();
            int height = cloneFrame.height();
            
            // 转换为QImage
            QImage image = cloneFrame.toImage();
            
            // 发射视频帧信号
            emit frameAvailable(image);
            
            cloneFrame.unmap();
        }
    }
}
