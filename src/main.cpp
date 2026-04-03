#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "3d/Qml3DRenderer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 注册QML类型
    qmlRegisterType<Qml3DRenderer>("QtMultiMedia3D", 1, 0, "Qml3DRenderer");

    QQmlApplicationEngine engine;
    // 使用QML模块的URI来加载主文件
    engine.load(QUrl(QStringLiteral("qrc:/QtMultiMedia3D/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
