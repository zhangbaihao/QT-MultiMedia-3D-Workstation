#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "3d/Custom3DModel.h"
#include "multimedia/VideoPlayer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 注册QML类型
    qmlRegisterType<Custom3DModel>("QtMultiMedia3D", 1, 0, "Custom3DModel");
    qmlRegisterType<VideoPlayer>("QtMultiMedia3D", 1, 0, "VideoPlayer");

    QQmlApplicationEngine engine;
    // 使用QML模块的URI来加载主文件
    engine.load(QUrl(QStringLiteral("qrc:/QtMultiMedia3D/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
