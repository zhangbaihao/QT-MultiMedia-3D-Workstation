#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QThread>
#include <atomic>

// FFmpeg头文件（暂时注释，后续可根据实际情况配置）
/*
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
}
*/

class AudioPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)

public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    QString filePath() const;
    void setFilePath(const QString &path);

    bool isPlaying() const;

public slots:
    void play();
    void pause();
    void stop();

signals:
    void filePathChanged();
    void isPlayingChanged();

private slots:
    void decodeThread();

private:
    void initializeFFmpeg();
    void cleanupFFmpeg();
    bool openFile();
    void decodeFrame();

    QString m_filePath;
    bool m_isPlaying;

    // FFmpeg相关（暂时使用占位符）
    void *m_formatCtx;
    void *m_codecCtx;
    void *m_audioStream;
    void *m_swrCtx;
    int m_audioStreamIndex;

    // 线程相关
    QThread *m_decodeThread;
    std::atomic<bool> m_stopThread;
    QMutex m_mutex;
};

#endif // AUDIOPLAYER_H
