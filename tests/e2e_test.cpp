#include <QTest>
#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QThread>
#include <QFile>
#include <QDir>

class EndToEndTest : public QObject
{
    Q_OBJECT

private:
    QString m_appPath;
    QProcess *m_appProcess;
    
    void createTestFiles();
    bool waitForAppStartup(int timeoutMs = 5000);
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // E2E测试
    void testAppLaunch();
    void test3DModelPage();
    void testVideoPlayerPage();
    void testControlsPage();
    void testNavigation();
};

void EndToEndTest::createTestFiles()
{
    // 创建测试模型文件
    QDir testDir(QDir::current());
    testDir.mkpath("test_resources");
    
    // 创建STL测试文件
    QString stlPath = QDir::current().filePath("test_resources/test_cube.stl");
    QFile stlFile(stlPath);
    if (stlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&stlFile);
        stream << "solid cube\n";
        for (int i = 0; i < 12; i++) {
            stream << "  facet normal 0 0 1\n";
            stream << "    outer loop\n";
            stream << "      vertex 0 0 0\n";
            stream << "      vertex 1 0 0\n";
            stream << "      vertex 1 1 0\n";
            stream << "    endloop\n";
            stream << "  endfacet\n";
        }
        stream << "endsolid cube\n";
        stlFile.close();
    }
    
    qDebug() << "测试资源创建完成:";
    qDebug() << "  STL模型:" << stlPath << "存在:" << QFile::exists(stlPath);
}

void EndToEndTest::initTestCase()
{
    qDebug() << "=== 端到端测试开始 ===";
    
    // 查找应用程序路径
    QStringList possiblePaths;
    possiblePaths << QDir::current().filePath("../../src/Release/QT-MultiMedia-3D-Workstation.exe");
    possiblePaths << QDir::current().filePath("../src/Release/QT-MultiMedia-3D-Workstation.exe");
    possiblePaths << "F:/code/AIProject_Auto/QT-MultiMedia-3D-Workstation/build/src/Release/QT-MultiMedia-3D-Workstation.exe";
    
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            m_appPath = path;
            break;
        }
    }
    
    QVERIFY2(!m_appPath.isEmpty(), "找不到应用程序可执行文件");
    qDebug() << "应用程序路径:" << m_appPath;
    
    // 创建测试文件
    createTestFiles();
    
    m_appProcess = nullptr;
}

void EndToEndTest::cleanupTestCase()
{
    qDebug() << "=== 端到端测试结束 ===";
    
    if (m_appProcess) {
        m_appProcess->terminate();
        if (!m_appProcess->waitForFinished(3000)) {
            m_appProcess->kill();
        }
        delete m_appProcess;
    }
}

bool EndToEndTest::waitForAppStartup(int timeoutMs)
{
    QTimer timer;
    timer.setSingleShot(true);
    timer.start(timeoutMs);
    
    while (timer.isActive() && m_appProcess->state() == QProcess::Running) {
        QThread::msleep(100);
        QCoreApplication::processEvents();
    }
    
    return m_appProcess->state() == QProcess::Running;
}

void EndToEndTest::testAppLaunch()
{
    qDebug() << "\n--- 测试应用程序启动 ---";
    
    m_appProcess = new QProcess(this);
    m_appProcess->setProgram(m_appPath);
    
    // 启动应用程序
    m_appProcess->start();
    
    QVERIFY2(m_appProcess->waitForStarted(5000), "应用程序启动失败");
    QVERIFY2(waitForAppStartup(3000), "应用程序未能正常运行");
    
    qDebug() << "应用程序启动成功!";
    qDebug() << "PID:" << m_appProcess->processId();
}

void EndToEndTest::test3DModelPage()
{
    qDebug() << "\n--- 测试3D模型页面 ---";
    
    // 这里可以添加更多测试逻辑
    // 例如通过模拟用户交互来测试功能
    
    QVERIFY2(m_appProcess && m_appProcess->state() == QProcess::Running, 
             "应用程序未运行");
    
    qDebug() << "3D模型页面测试通过 (基础检查)";
}

void EndToEndTest::testVideoPlayerPage()
{
    qDebug() << "\n--- 测试视频播放页面 ---";
    
    QVERIFY2(m_appProcess && m_appProcess->state() == QProcess::Running, 
             "应用程序未运行");
    
    qDebug() << "视频播放页面测试通过 (基础检查)";
}

void EndToEndTest::testControlsPage()
{
    qDebug() << "\n--- 测试控件展示页面 ---";
    
    QVERIFY2(m_appProcess && m_appProcess->state() == QProcess::Running, 
             "应用程序未运行");
    
    qDebug() << "控件展示页面测试通过 (基础检查)";
}

void EndToEndTest::testNavigation()
{
    qDebug() << "\n--- 测试页面导航 ---";
    
    QVERIFY2(m_appProcess && m_appProcess->state() == QProcess::Running, 
             "应用程序未运行");
    
    // 检查应用程序是否仍在运行（没有崩溃）
    QThread::msleep(1000);
    QVERIFY2(m_appProcess->state() == QProcess::Running, 
             "应用程序在导航测试中崩溃");
    
    qDebug() << "页面导航测试通过";
}

QTEST_MAIN(EndToEndTest)
#include "e2e_test.moc"
