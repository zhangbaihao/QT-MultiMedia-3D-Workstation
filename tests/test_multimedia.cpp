#include <QFileInfo>
#include <QTest>
#include <QCoreApplication>

#include "../src/multimedia/AudioPlayer.h"
#include "../src/multimedia/VideoPlayer.h"

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

    const QString path = QStringLiteral("test.mp4");
    if (!QFileInfo::exists(path)) {
        QSKIP("缺少 test.mp4：将短视频放在工作目录后再运行此用例。");
    }

    player.setFilePath(path);
    QCOMPARE(player.filePath(), path);
    QVERIFY(player.duration() > 0);

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

    const QString path = QStringLiteral("test.mp3");
    if (!QFileInfo::exists(path)) {
        QSKIP("缺少 test.mp3：将带音频的媒体放在工作目录后再运行此用例。");
    }

    player.setFilePath(path);
    QCOMPARE(player.filePath(), path);
    QVERIFY(player.duration() > 0);

    player.play();
    QVERIFY(player.isPlaying());

    player.pause();
    QVERIFY(!player.isPlaying());

    player.stop();
    QVERIFY(!player.isPlaying());
}

QTEST_MAIN(TestMultimedia)
#include "test_multimedia.moc"
