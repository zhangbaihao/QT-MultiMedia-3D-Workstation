#include <QTest>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "../src/3d/ModelLoader.h"
#include "../src/3d/Qml3DRenderer.h"

class Test3DFunctionality : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // 3D功能测试
    void testModelLoading();
    void testModelRendering();
    void testModelInfo();
    void testRenderOptions();
    void testViewControls();
};

void Test3DFunctionality::initTestCase()
{
    qDebug() << "=== 3D功能完整测试开始 ===";
    
    // 创建测试模型
    QDir testDir(QDir::current());
    testDir.mkpath("test_3d_models");
    
    QString cubeStlPath = QDir::current().filePath("test_3d_models/cube.stl");
    QFile stlFile(cubeStlPath);
    if (stlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&stlFile);
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
        stlFile.close();
    }
    
    qDebug() << "测试模型已创建";
}

void Test3DFunctionality::cleanupTestCase()
{
    qDebug() << "=== 3D功能完整测试结束 ===";
}

void Test3DFunctionality::testModelLoading()
{
    qDebug() << "\n--- 测试模型加载 ---";
    
    Qml3DRenderer renderer;
    
    // 测试空路径
    renderer.setModelPath("");
    QVERIFY2(renderer.vertexCount() == 0, "空路径应该返回0顶点");
    QVERIFY2(renderer.faceCount() == 0, "空路径应该返回0面");
    
    // 测试不存在的文件
    renderer.setModelPath("nonexistent.stl");
    QVERIFY2(renderer.vertexCount() == 0, "不存在的文件应该返回0顶点");
    QVERIFY2(renderer.faceCount() == 0, "不存在的文件应该返回0面");
    
    // 测试有效STL文件
    QString stlPath = QDir::current().filePath("test_3d_models/cube.stl");
    renderer.setModelPath(stlPath);
    
    QVERIFY2(renderer.vertexCount() > 0, "有效的STL文件应该加载顶点");
    QVERIFY2(renderer.faceCount() > 0, "有效的STL文件应该加载面");
    
    qDebug() << "模型加载测试通过";
    qDebug() << "  顶点数:" << renderer.vertexCount();
    qDebug() << "  面数:" << renderer.faceCount();
}

void Test3DFunctionality::testModelRendering()
{
    qDebug() << "\n--- 测试模型渲染 ---";
    
    Qml3DRenderer renderer;
    
    // 测试默认立方体渲染
    QVERIFY2(renderer.vertexCount() == 0, "初始状态应该有0顶点");
    
    // 加载模型
    QString stlPath = QDir::current().filePath("test_3d_models/cube.stl");
    renderer.setModelPath(stlPath);
    
    // 测试渲染属性
    QVERIFY2(renderer.modelPath() == stlPath, "模型路径设置失败");
    QVERIFY2(renderer.modelColor() == Qt::blue, "初始颜色应该是蓝色");
    
    // 测试颜色设置
    renderer.setModelColor(Qt::red);
    QVERIFY2(renderer.modelColor() == Qt::red, "颜色设置失败");
    
    qDebug() << "模型渲染测试通过";
}

void Test3DFunctionality::testModelInfo()
{
    qDebug() << "\n--- 测试模型信息 ---";
    
    Qml3DRenderer renderer;
    
    // 加载前
    QVERIFY2(renderer.vertexCount() == 0, "加载前顶点数应该为0");
    QVERIFY2(renderer.faceCount() == 0, "加载前面数应该为0");
    
    // 加载后
    QString stlPath = QDir::current().filePath("test_3d_models/cube.stl");
    renderer.setModelPath(stlPath);
    
    QVERIFY2(renderer.vertexCount() > 0, "加载后顶点数应该大于0");
    QVERIFY2(renderer.faceCount() > 0, "加载后面数应该大于0");
    
    qDebug() << "模型信息测试通过";
    qDebug() << "  实际顶点数:" << renderer.vertexCount();
    qDebug() << "  实际面数:" << renderer.faceCount();
}

void Test3DFunctionality::testRenderOptions()
{
    qDebug() << "\n--- 测试渲染选项 ---";
    
    Qml3DRenderer renderer;
    
    // 测试默认值
    QVERIFY2(renderer.showGrid() == false, "初始显示网格应该为false");
    QVERIFY2(renderer.showNormals() == false, "初始显示法线应该为false");
    QVERIFY2(renderer.wireframe() == false, "初始线框模式应该为false");
    QVERIFY2(renderer.doubleSided() == false, "初始双面渲染应该为false");
    
    // 测试设置值
    renderer.setShowGrid(true);
    QVERIFY2(renderer.showGrid() == true, "显示网格设置失败");
    
    renderer.setShowNormals(true);
    QVERIFY2(renderer.showNormals() == true, "显示法线设置失败");
    
    renderer.setWireframe(true);
    QVERIFY2(renderer.wireframe() == true, "线框模式设置失败");
    
    renderer.setDoubleSided(true);
    QVERIFY2(renderer.doubleSided() == true, "双面渲染设置失败");
    
    qDebug() << "渲染选项测试通过";
}

void Test3DFunctionality::testViewControls()
{
    qDebug() << "\n--- 测试视图控制 ---";
    
    Qml3DRenderer renderer;
    
    // 测试默认值
    QVERIFY2(renderer.rotationX() == 0, "初始X旋转应该为0");
    QVERIFY2(renderer.rotationY() == 0, "初始Y旋转应该为0");
    QVERIFY2(renderer.zoom() == 1.0, "初始缩放应该为1.0");
    
    // 测试设置值
    renderer.setRotationX(45.0);
    QVERIFY2(renderer.rotationX() == 45.0, "X旋转设置失败");
    
    renderer.setRotationY(30.0);
    QVERIFY2(renderer.rotationY() == 30.0, "Y旋转设置失败");
    
    renderer.setZoom(2.0);
    QVERIFY2(renderer.zoom() == 2.0, "缩放设置失败");
    
    qDebug() << "视图控制测试通过";
}

QTEST_MAIN(Test3DFunctionality)
#include "test_3d_functionality.moc"
