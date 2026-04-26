#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QString>
#include <QtGui/QImage>
#include <QTimer>

#include <atomic>

#include "FFmpegDecoder.h"

class VideoFrameImageProvider;

class VideoPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(int videoWidth READ videoWidth NOTIFY videoGeometryChanged)
    Q_PROPERTY(int videoHeight READ videoHeight NOTIFY videoGeometryChanged)
    Q_PROPERTY(int frameSequence READ frameSequence NOTIFY frameSequenceChanged)
    Q_PROPERTY(double frameRate READ frameRate NOTIFY videoGeometryChanged)

public:
    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    static void setFrameProvider(VideoFrameImageProvider *provider);

    QString filePath() const;
    void setFilePath(const QString &path);

    bool isPlaying() const;
    int duration() const;
    int position() const;
    void setPosition(int position);

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    int videoWidth() const;
    int videoHeight() const;
    int frameSequence() const;
    double frameRate() const;

public slots:
    void play();
    void pause();
    void stop();

signals:
    void filePathChanged();
    void isPlayingChanged();
    void durationChanged();
    void positionChanged();
    void playbackRateChanged();
    void videoGeometryChanged();
    void frameSequenceChanged();
    void frameAvailable(const QImage &frame);
    void errorOccurred(const QString &error);

private slots:
    void onDecoderError(const QString &error);
    void onPlaybackTimer();

private:
    void startPlayback();
    void stopPlayback();
    void pushFrame(const QImage &frame);
    static QString normalizePath(const QString &path);
    int calculateFrameInterval() const;

    FFmpegDecoder *m_decoder;
    QTimer *m_playbackTimer;
    QString m_filePath;
    bool m_isPlaying;
    int m_duration;
    int m_position;
    double m_frameRate;
    qreal m_playbackRate;
    int m_frameSequence;

    bool m_afterSeek;    // seek 后跳过首帧的位置递增
    qint64 m_lastFramePts; // 上一帧 PTS（毫秒）

    std::atomic<bool> m_destroying{false};

    static VideoFrameImageProvider *s_frameProvider;
};

#endif
