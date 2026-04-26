#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QDir>
#include <iostream>

#include "multimedia/AudioPlayer.h"
#include "multimedia/VideoFrameImageProvider.h"
#include "multimedia/VideoPlayer.h"
#include "other-viewer/FramebufferObject.h"
#include "other-viewer/ModelLoader.h"
#include "qtquick3d-viewer/CameraController.h"
#include "qtquick3d-viewer/quickModelLoader.h"

// 文件日志输出
static QFile s_logFile;
static QTextStream s_logStream;
static QMutex s_logMutex;

static void logMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    QMutexLocker locker(&s_logMutex);

    // 格式化时间戳
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

    // 格式化消息
    QString formatted;
    switch (type) {
    case QtDebugMsg:    formatted = QStringLiteral("[D]"); break;
    case QtWarningMsg:  formatted = QStringLiteral("[W]"); break;
    case QtCriticalMsg: formatted = QStringLiteral("[C]"); break;
    case QtFatalMsg:    formatted = QStringLiteral("[F]"); break;
    case QtInfoMsg:     formatted = QStringLiteral("[I]"); break;
    }

    QString line = QStringLiteral("%1 %2 %3").arg(timestamp, formatted, msg);

    // 输出到文件
    if (s_logFile.isOpen()) {
        s_logStream << line << "\n";
        s_logStream.flush();
    }

    // 也输出到 stderr（方便调试器查看）
    std::cerr << line.toStdString() << std::endl;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 打开日志文件（在可执行文件同目录）
    QString logPath = QCoreApplication::applicationDirPath() + "/audiolog.txt";
    s_logFile.setFileName(logPath);
    bool logOk = false;
    if (s_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        s_logStream.setDevice(&s_logFile);
        logOk = true;
    } else {
        // 回退到 temp 目录
        logPath = QDir::tempPath() + "/QT-MultiMedia-audiolog.txt";
        s_logFile.setFileName(logPath);
        if (s_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            s_logStream.setDevice(&s_logFile);
            logOk = true;
        }
    }

    // 安装日志处理器（保存旧处理器以便退出时恢复）
    const auto prevLogHandler = qInstallMessageHandler(logMessageHandler);

    qDebug() << "=== Application started ===";
    qDebug() << "Log file:" << logPath << "(opened:" << logOk << ")";

    static VideoFrameImageProvider s_videoFrameProvider;
    VideoPlayer::setFrameProvider(&s_videoFrameProvider);

    qmlRegisterType<VideoPlayer>("QtMultiMedia3D", 1, 0, "VideoPlayer");
    qmlRegisterType<AudioPlayer>("QtMultiMedia3D", 1, 0, "AudioPlayer");
    qmlRegisterType<ModelLoader>("QtMultiMedia3D", 1, 0, "ModelLoader");
    qmlRegisterType<CameraController>("QtMultiMedia3D", 1, 0, "CameraController");
    qmlRegisterType<OtherModelLoader>("QtMultiMedia3D", 1, 0, "OtherModelLoader");
    qmlRegisterType<FramebufferObject>("QtMultiMedia3D", 1, 0, "FramebufferObject");

    QQmlApplicationEngine engine;
    engine.addImageProvider(QStringLiteral("videoframe"), &s_videoFrameProvider);

    OtherModelLoader modelLoader;
    engine.rootContext()->setContextProperty("modelLoader", &modelLoader);

    engine.load(QUrl(QStringLiteral("qrc:/QtMultiMedia3D/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    const int ret = app.exec();

    // 恢复之前的日志处理器（如果之前有的话）。
    // 在静态对象销毁期间，全局 Qt 消息处理器仍然存在，如果它访问已经被销毁的静态对象
    //（如 s_logFile / s_logStream / s_logMutex），会导致未定义行为甚至崩溃。
    qInstallMessageHandler(prevLogHandler);

    return ret;
}
