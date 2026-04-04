#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QString>
#include <QtGui/QImage>
#include <QMediaPlayer>

class VideoPlayer : public QMediaPlayer
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)

public:
    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    QString filePath() const;
    void setFilePath(const QString &path);

    bool isPlaying() const;
    int duration() const;
    int position() const;
    void setPosition(int position);

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

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);

private:
    QString m_filePath;
    bool m_isPlaying;
    int m_duration;
    int m_position;

    // Qt多媒体相关
    QMediaPlayer *m_mediaPlayer;
};

#endif // VIDEOPLAYER_H
