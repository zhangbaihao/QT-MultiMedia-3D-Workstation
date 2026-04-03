#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QString>
#include <QtGui/QImage>
#include <QMutex>
#include <QThread>
#include <atomic>

// FFmpeg头文件（暂时注释，后续可根据实际情况配置）
/*
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
*/

class VideoPlayer : public QObject
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
    void decodeThread();

private:
    void initializeFFmpeg();
    void cleanupFFmpeg();
    bool openFile();
    void decodeFrame();

    QString m_filePath;
    bool m_isPlaying;
    int m_duration;
    int m_position;

    // FFmpeg相关（暂时使用占位符）
    void *m_formatCtx;
    void *m_codecCtx;
    void *m_videoStream;
    void *m_swsCtx;
    int m_videoStreamIndex;

    // 线程相关
    QThread *m_decodeThread;
    std::atomic<bool> m_stopThread;
    QMutex m_mutex;
};

#endif // VIDEOPLAYER_H
