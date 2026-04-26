#ifndef FFMPEGDECODER_H
#define FFMPEGDECODER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#include <QObject>
#include <QString>
#include <QImage>
#include <QMutex>

class FFmpegDecoder : public QObject
{
    Q_OBJECT

public:
    explicit FFmpegDecoder(QObject *parent = nullptr);
    ~FFmpegDecoder();

    bool openFile(const QString &filePath);
    void closeFile();
    bool isOpen() const;

    int width() const;
    int height() const;
    double frameRate() const;
    qint64 duration() const;

    bool seek(qint64 positionMs);
    QImage decodeNextFrame();
    bool hasVideo() const;
    bool isAtEnd() const;

signals:
    void errorOccurred(const QString &error);

private:
    void initializeFFmpeg();
    void cleanup();
    void closeFileUnlocked();
    bool setupVideoCodec();
    QImage convertFrameToImage(AVFrame *frame);
    AVFormatContext *m_formatContext;
    AVCodecContext *m_videoCodecContext;
    AVFrame *m_frame;
    AVFrame *m_frameRGB;
    SwsContext *m_swsContext;
    int m_videoStreamIndex;
    double m_frameRate;
    qint64 m_duration;
    bool m_isOpen;
    bool m_eof;

    QMutex m_mutex;
};

#endif
