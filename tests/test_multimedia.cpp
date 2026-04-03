#include <QTest>
#include <QCoreApplication>
#include "../src/multimedia/VideoPlayer.h"
#include "../src/multimedia/AudioPlayer.h"

class TestMultimedia : public QObject
{
    Q_OBJECT

private slots:
    void testVideoPlayer();
    void testAudioPlayer();
};

void TestMultimedia::testVideoPlayer()
{
    VideoPlayer player;
    
    // 测试设置文件路径
    player.setFilePath("test.mp4");
    QCOMPARE(player.filePath(), QString("test.mp4"));
    
    // 测试播放控制
    player.play();
    QVERIFY(player.isPlaying());
    
    player.pause();
    QVERIFY(!player.isPlaying());
    
    player.stop();
    QVERIFY(!player.isPlaying());
}

void TestMultimedia::testAudioPlayer()
{
    AudioPlayer player;
    
    // 测试设置文件路径
    player.setFilePath("test.mp3");
    QCOMPARE(player.filePath(), QString("test.mp3"));
    
    // 测试播放控制
    player.play();
    QVERIFY(player.isPlaying());
    
    player.pause();
    QVERIFY(!player.isPlaying());
    
    player.stop();
    QVERIFY(!player.isPlaying());
}

QTEST_MAIN(TestMultimedia)
#include "test_multimedia.moc"
