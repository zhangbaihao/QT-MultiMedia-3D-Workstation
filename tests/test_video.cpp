#include <QTest>
#include <QCoreApplication>
#include <QDebug>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>

class VideoTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // 视频功能测试
    void testMediaPlayerCreation();
    void testMediaPlayerProperties();
    void testAudioOutput();
};

void VideoTest::initTestCase()
{
    qDebug() << "=== 视频播放功能测试开始 ===";
}

void VideoTest::cleanupTestCase()
{
    qDebug() << "=== 视频播放功能测试结束 ===";
}

void VideoTest::testMediaPlayerCreation()
{
    qDebug() << "\n--- 测试MediaPlayer创建 ---";
    
    QMediaPlayer *player = new QMediaPlayer(this);
    QVERIFY2(player != nullptr, "MediaPlayer创建失败");
    
    delete player;
    qDebug() << "MediaPlayer创建测试通过";
}

void VideoTest::testMediaPlayerProperties()
{
    qDebug() << "\n--- 测试MediaPlayer属性 ---";
    
    QMediaPlayer player;
    
    // 测试初始状态
    QVERIFY2(player.playbackState() == QMediaPlayer::StoppedState, "初始状态应该是Stopped");
    QVERIFY2(player.position() == 0, "初始位置应该是0");
    
    // 测试播放速度
    player.setPlaybackRate(1.5);
    QVERIFY2(player.playbackRate() == 1.5, "播放速度设置失败");
    
    qDebug() << "MediaPlayer属性测试通过";
}

void VideoTest::testAudioOutput()
{
    qDebug() << "\n--- 测试音频输出 ---";
    
    QMediaPlayer player;
    QAudioOutput *audioOutput = new QAudioOutput(this);
    player.setAudioOutput(audioOutput);
    
    QVERIFY2(player.audioOutput() != nullptr, "音频输出设置失败");
    
    // 测试音量
    audioOutput->setVolume(0.5);
    QVERIFY2(audioOutput->volume() == 0.5, "音量设置失败");
    
    // 测试静音
    audioOutput->setMuted(true);
    QVERIFY2(audioOutput->isMuted() == true, "静音设置失败");
    
    qDebug() << "音频输出测试通过";
}

QTEST_MAIN(VideoTest)
#include "test_video.moc"
