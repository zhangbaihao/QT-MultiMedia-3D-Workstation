#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "multimedia/VideoPlayer.h"
#include "qtquick3d-viewer/ModelLoader.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 注册QML类型
    qmlRegisterType<VideoPlayer>("QtMultiMedia3D", 1, 0, "VideoPlayer");
    qmlRegisterType<ModelLoader>("QtMultiMedia3D", 1, 0, "ModelLoader");

    QQmlApplicationEngine engine;
    // 加载主QML文件
    engine.load(QUrl(QStringLiteral("qrc:/QtMultiMedia3D/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
