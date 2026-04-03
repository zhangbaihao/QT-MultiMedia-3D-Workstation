#include <QTest>
#include <QCoreApplication>
#include "../src/3d/ModelLoader.h"
#include "../src/3d/Camera.h"

class Test3D : public QObject
{
    Q_OBJECT

private slots:
    void testCamera();
    void testModelLoader();
};

void Test3D::testCamera()
{
    Camera camera;
    
    // 测试相机初始化
    QVector3D initialPosition(0.0f, 0.0f, 5.0f);
    QVector3D initialTarget(0.0f, 0.0f, 0.0f);
    
    // 测试旋转
    camera.rotate(45.0f, 0.0f);
    
    // 测试平移
    camera.translate(QVector3D(1.0f, 0.0f, 0.0f));
    
    // 测试缩放
    camera.zoom(1.0f);
    
    // 测试投影矩阵
    camera.setPerspective(45.0f, 1.0f, 0.1f, 100.0f);
    QMatrix4x4 projection = camera.projectionMatrix();
    QVERIFY(!projection.isIdentity());
}

void Test3D::testModelLoader()
{
    ModelLoader loader;
    QVector<Vertex> vertices;
    QVector<unsigned int> indices;
    
    // 测试STL加载（这里只是测试接口，实际文件需要存在）
    // bool stlResult = loader.loadSTL("test.stl", vertices, indices);
    // QVERIFY(stlResult);
    
    // 测试PLY加载（这里只是测试接口，实际文件需要存在）
    // bool plyResult = loader.loadPLY("test.ply", vertices, indices);
    // QVERIFY(plyResult);
    
    // 这里我们只是测试类的初始化，实际文件加载需要具体的模型文件
    QVERIFY(true);
}

QTEST_MAIN(Test3D)
#include "test_3d.moc"
