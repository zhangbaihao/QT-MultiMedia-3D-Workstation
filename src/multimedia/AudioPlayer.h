#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QAudioFormat>
#include <QByteArray>
#include <QIODevice>
#include <QMutex>
#include <QObject>
#include <QString>

#include <atomic>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

class QAudioDevice;
class QAudioSink;

class AudioPlayer;

class AudioPullDevice final : public QIODevice
{
public:
    explicit AudioPullDevice(AudioPlayer *player);
    void resetPlayer() { m_player = nullptr; }

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

public:
    bool isSequential() const override { return true; }
    qint64 bytesAvailable() const override;

private:
    AudioPlayer *m_player;
};

class AudioPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(QString audioDevice READ audioDevice WRITE setAudioDevice NOTIFY audioDeviceChanged)

public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer() override;

    QString filePath() const;
    void setFilePath(const QString &path);

    bool isPlaying() const;
    int duration() const;
    int position() const;
    void setPosition(int positionMs);

    qreal volume() const;
    void setVolume(qreal v);

    bool muted() const;
    void setMuted(bool m);

    QString audioDevice() const;
    void setAudioDevice(const QString &description);

    // 获取可用音频设备列表
    Q_INVOKABLE QStringList availableAudioDevices() const;

public slots:
    void play();
    void pause();
    void stop();

signals:
    void filePathChanged();
    void isPlayingChanged();
    void durationChanged();
    void positionChanged();
    void volumeChanged();
    void mutedChanged();
    void audioDeviceChanged();
    void errorOccurred(const QString &error);

public:
    qint64 readAudioData(char *data, qint64 maxlen);
    qint64 pcmQueueSize() const;
    bool isAtEnd() const;

private:
    static QString normalizePath(const QString &path);

    bool ensureDecoderOpen();
    void closeDecoder();
    void closeDecoderUnlocked();
    bool setupSwr(const AVCodecParameters *par);
    int fillAudioQueue(int targetBytes);
    QAudioDevice findAudioDevice() const;

    QString m_filePath;
    bool m_isPlaying = false;
    int m_durationMs = 0;
    int m_positionMs = 0;
    qreal m_volume = 0.8;
    bool m_muted = false;

    std::atomic<bool> m_destroying{false};

    AVFormatContext *m_fmt = nullptr;
    AVCodecContext *m_dec = nullptr;
    SwrContext *m_swr = nullptr;
    int m_audioIndex = -1;
    AVStream *m_audioStream = nullptr;

    QAudioSink *m_sink = nullptr;
    AudioPullDevice *m_pull = nullptr;
    QAudioFormat m_outFormat;

    mutable QMutex m_mutex;
    QByteArray m_pcmQueue;
    bool m_eofDecode = false;
    AVFrame *m_audioFrame = nullptr;

    QString m_audioDeviceDesc; // 用户指定的音频设备描述
};

#endif
