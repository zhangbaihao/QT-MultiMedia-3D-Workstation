#include <QTest>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QQuickView>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "../src/3d/ModelLoader.h"
#include "../src/3d/Qml3DRenderer.h"

class IntegrationTest3D : public QObject
{
    Q_OBJECT

private:
    QString m_testModelPath;
    
    void createTestSTLFile(const QString &path);
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // 集成测试
    void testFullWorkflow();
    void testModelSwitching();
    void testRenderOptionsEffect();
};

void IntegrationTest3D::createTestSTLFile(const QString &path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << "solid cube\n";
        // 立方体的12个面
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
        file.close();
    }
}

void IntegrationTest3D::initTestCase()
{
    qDebug() << "=== 3D功能集成测试开始 ===";
    
    // 创建测试模型
    QDir testDir(QDir::current());
    testDir.mkpath("test_models");
    m_testModelPath = QDir::current().filePath("test_models/integration_test_cube.stl");
    createTestSTLFile(m_testModelPath);
    
    qDebug() << "测试模型:" << m_testModelPath;
    qDebug() << "模型存在:" << QFile::exists(m_testModelPath);
}

void IntegrationTest3D::cleanupTestCase()
{
    qDebug() << "=== 3D功能集成测试结束 ===";
}

void IntegrationTest3D::testFullWorkflow()
{
    qDebug() << "\n--- 测试完整工作流程 ---";
    
    // 1. 创建渲染器
    Qml3DRenderer renderer;
    QVERIFY2(renderer.vertexCount() == 0, "初始顶点数应该为0");
    QVERIFY2(renderer.faceCount() == 0, "初始面数应该为0");
    
    // 2. 设置模型路径
    renderer.setModelPath(m_testModelPath);
    QVERIFY2(renderer.modelPath() == m_testModelPath, "模型路径设置失败");
    
    // 3. 验证模型加载
    QVERIFY2(renderer.vertexCount() > 0, "模型加载后顶点数应该大于0");
    QVERIFY2(renderer.faceCount() > 0, "模型加载后面数应该大于0");
    
    qDebug() << "模型加载成功:";
    qDebug() << "  顶点数:" << renderer.vertexCount();
    qDebug() << "  面数:" << renderer.faceCount();
    
    // 4. 测试渲染选项
    renderer.setShowGrid(true);
    renderer.setShowNormals(true);
    renderer.setWireframe(true);
    renderer.setDoubleSided(true);
    
    QVERIFY2(renderer.showGrid() == true, "显示网格设置失败");
    QVERIFY2(renderer.showNormals() == true, "显示法线设置失败");
    QVERIFY2(renderer.wireframe() == true, "线框模式设置失败");
    QVERIFY2(renderer.doubleSided() == true, "双面渲染设置失败");
    
    // 5. 测试视图控制
    renderer.setRotationX(45.0);
    renderer.setRotationY(30.0);
    renderer.setZoom(2.0);
    
    QVERIFY2(renderer.rotationX() == 45.0, "X旋转设置失败");
    QVERIFY2(renderer.rotationY() == 30.0, "Y旋转设置失败");
    QVERIFY2(renderer.zoom() == 2.0, "缩放设置失败");
    
    // 6. 测试颜色设置
    renderer.setModelColor(Qt::red);
    QVERIFY2(renderer.modelColor() == Qt::red, "颜色设置失败");
    
    qDebug() << "完整工作流程测试通过!";
}

void IntegrationTest3D::testModelSwitching()
{
    qDebug() << "\n--- 测试模型切换 ---";
    
    Qml3DRenderer renderer;
    
    // 加载第一个模型
    renderer.setModelPath(m_testModelPath);
    int vertexCount1 = renderer.vertexCount();
    int faceCount1 = renderer.faceCount();
    
    QVERIFY2(vertexCount1 > 0, "第一个模型加载失败");
    
    // 创建第二个不同的模型
    QString model2Path = QDir::current().filePath("test_models/model2.stl");
    QFile file(model2Path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << "solid triangle\n";
        stream << "  facet normal 0 0 1\n";
        stream << "    outer loop\n";
        stream << "      vertex 0 0 0\n";
        stream << "      vertex 1 0 0\n";
        stream << "      vertex 0.5 1 0\n";
        stream << "    endloop\n";
        stream << "  endfacet\n";
        stream << "endsolid triangle\n";
        file.close();
    }
    
    // 切换到第二个模型
    renderer.setModelPath(model2Path);
    int vertexCount2 = renderer.vertexCount();
    int faceCount2 = renderer.faceCount();
    
    QVERIFY2(vertexCount2 > 0, "第二个模型加载失败");
    QVERIFY2(vertexCount2 != vertexCount1, "模型切换后顶点数应该不同");
    
    qDebug() << "模型切换测试通过!";
    qDebug() << "  模型1顶点数:" << vertexCount1;
    qDebug() << "  模型2顶点数:" << vertexCount2;
    
    // 清理
    QFile::remove(model2Path);
}

void IntegrationTest3D::testRenderOptionsEffect()
{
    qDebug() << "\n--- 测试渲染选项效果 ---";
    
    Qml3DRenderer renderer;
    renderer.setModelPath(m_testModelPath);
    
    // 测试线框模式切换
    renderer.setWireframe(false);
    QVERIFY2(renderer.wireframe() == false, "线框模式关闭失败");
    
    renderer.setWireframe(true);
    QVERIFY2(renderer.wireframe() == true, "线框模式开启失败");
    
    // 测试显示网格
    renderer.setShowGrid(false);
    QVERIFY2(renderer.showGrid() == false, "网格显示关闭失败");
    
    renderer.setShowGrid(true);
    QVERIFY2(renderer.showGrid() == true, "网格显示开启失败");
    
    // 测试显示法线
    renderer.setShowNormals(false);
    QVERIFY2(renderer.showNormals() == false, "法线显示关闭失败");
    
    renderer.setShowNormals(true);
    QVERIFY2(renderer.showNormals() == true, "法线显示开启失败");
    
    // 测试双面渲染
    renderer.setDoubleSided(false);
    QVERIFY2(renderer.doubleSided() == false, "双面渲染关闭失败");
    
    renderer.setDoubleSided(true);
    QVERIFY2(renderer.doubleSided() == true, "双面渲染开启失败");
    
    qDebug() << "渲染选项效果测试通过!";
}

QTEST_MAIN(IntegrationTest3D)
#include "integration_test_3d.moc"
