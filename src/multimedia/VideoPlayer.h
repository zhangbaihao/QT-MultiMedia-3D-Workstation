#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QString>
#include <QtGui/QImage>
#include <QMediaPlayer>
#include <QVideoSink>

class VideoPlayer : public QMediaPlayer
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)

public:
    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    QString filePath() const;
    void setFilePath(const QString &path);

    bool isPlaying() const;
    int duration() const;
    int position() const;
    void setPosition(int position);
    QVideoSink* videoSink() const;
    void setVideoSink(QVideoSink* sink);

public slots:
    void play();
    void pause();
    void stop();

signals:
    void filePathChanged();
    void isPlayingChanged();
    void durationChanged();
    void positionChanged();
    void frameAvailable(const QImage &frame);
    void videoSinkChanged();

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onVideoFrameChanged(const QVideoFrame &frame);

private:
    QString m_filePath;
    bool m_isPlaying;
    int m_duration;
    int m_position;
    QVideoSink* m_videoSink;
};

#endif // VIDEOPLAYER_H
