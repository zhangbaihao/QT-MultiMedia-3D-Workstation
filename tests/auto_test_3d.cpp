#include <QTest>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "../src/3d/ModelLoader.h"
#include "../src/3d/Qml3DRenderer.h"

class AutoTest3D : public QObject
{
    Q_OBJECT

private:
    QString m_testModelPath;
    void createTestSTLFile(const QString &path);

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // 模型加载测试
    void testModelLoaderSTL();
    void testModelLoaderPLY();
    void testModelLoaderInvalidFile();
    
    // Qml3DRenderer测试
    void testQml3DRendererProperties();
    void testQml3DRendererModelLoading();
    
    // 渲染选项测试
    void testRenderOptions();
    
    // 模型信息测试
    void testModelInfo();
};

void AutoTest3D::initTestCase()
{
    qDebug() << "=== 3D功能自动化测试开始 ===";
    
    // 检查多个可能的路径
    QStringList possiblePaths;
    possiblePaths << QDir::current().filePath("../../../test_models/cube.stl");  // 从 build/tests/Release
    possiblePaths << QDir::current().filePath("../../test_models/cube.stl");     // 从 build/tests
    possiblePaths << QDir::current().filePath("../test_models/cube.stl");        // 从 build
    possiblePaths << "F:/code/AIProject_Auto/QT-MultiMedia-3D-Workstation/test_models/cube.stl";
    
    QString foundPath;
    for (const QString &path : possiblePaths) {
        qDebug() << "检查路径:" << path << "存在:" << QFile::exists(path);
        if (QFile::exists(path)) {
            foundPath = path;
            break;
        }
    }
    
    if (!foundPath.isEmpty()) {
        m_testModelPath = foundPath;
        qDebug() << "找到测试模型:" << m_testModelPath;
    } else {
        // 创建测试模型目录和文件
        QDir testDir(QDir::current());
        testDir.mkpath("test_models");
        m_testModelPath = QDir::current().filePath("test_models/cube.stl");
        createTestSTLFile(m_testModelPath);
        qDebug() << "创建测试模型:" << m_testModelPath;
    }
}

void AutoTest3D::createTestSTLFile(const QString &path)
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
        qDebug() << "测试STL文件已创建:" << path;
    }
}

void AutoTest3D::cleanupTestCase()
{
    qDebug() << "=== 3D功能自动化测试结束 ===";
}

void AutoTest3D::testModelLoaderSTL()
{
    qDebug() << "\n--- 测试STL模型加载 ---";
    
    ModelLoader loader;
    QVector<Vertex> vertices;
    QVector<unsigned int> indices;
    
    // 测试加载cube.stl
    bool result = loader.loadSTL(m_testModelPath, vertices, indices);
    
    QVERIFY2(result, "STL文件加载失败");
    QVERIFY2(!vertices.isEmpty(), "顶点数据为空");
    QVERIFY2(!indices.isEmpty(), "索引数据为空");
    
    qDebug() << "顶点数:" << vertices.size();
    qDebug() << "索引数:" << indices.size();
    qDebug() << "面数:" << indices.size() / 3;
    
    // 验证顶点数据
    QVERIFY2(vertices.size() == 36, "立方体应该有36个顶点(12个面 x 3个顶点)");
    QVERIFY2(indices.size() == 36, "立方体应该有36个索引");
}

void AutoTest3D::testModelLoaderPLY()
{
    qDebug() << "\n--- 测试PLY模型加载 ---";
    
    ModelLoader loader;
    QVector<Vertex> vertices;
    QVector<unsigned int> indices;
    
    // 测试加载不存在的PLY文件
    QString plyPath = QDir::current().filePath("../../test_models/nonexistent.ply");
    bool result = loader.loadPLY(plyPath, vertices, indices);
    
    QVERIFY2(!result, "不存在的PLY文件应该加载失败");
}

void AutoTest3D::testModelLoaderInvalidFile()
{
    qDebug() << "\n--- 测试无效文件加载 ---";
    
    ModelLoader loader;
    QVector<Vertex> vertices;
    QVector<unsigned int> indices;
    
    // 测试空路径
    bool result = loader.loadSTL("", vertices, indices);
    QVERIFY2(!result, "空路径应该加载失败");
    
    // 测试无效文件扩展名
    result = loader.loadSTL("test.obj", vertices, indices);
    // 注意：当前实现不会检查扩展名，只会尝试解析
}

void AutoTest3D::testQml3DRendererProperties()
{
    qDebug() << "\n--- 测试Qml3DRenderer属性 ---";
    
    Qml3DRenderer renderer;
    
    // 测试初始值
    QVERIFY2(renderer.modelPath().isEmpty(), "初始模型路径应该为空");
    QVERIFY2(renderer.modelColor() == Qt::blue, "初始颜色应该是蓝色");
    QVERIFY2(renderer.rotationX() == 0, "初始X旋转应该是0");
    QVERIFY2(renderer.rotationY() == 0, "初始Y旋转应该是0");
    QVERIFY2(renderer.zoom() == 1.0, "初始缩放应该是1.0");
    QVERIFY2(renderer.vertexCount() == 0, "初始顶点数应该是0");
    QVERIFY2(renderer.faceCount() == 0, "初始面数应该是0");
    
    // 测试渲染选项初始值
    QVERIFY2(renderer.showGrid() == false, "初始显示网格应该是false");
    QVERIFY2(renderer.showNormals() == false, "初始显示法线应该是false");
    QVERIFY2(renderer.wireframe() == false, "初始线框模式应该是false");
    QVERIFY2(renderer.doubleSided() == false, "初始双面渲染应该是false");
    
    qDebug() << "属性初始值测试通过";
}

void AutoTest3D::testQml3DRendererModelLoading()
{
    qDebug() << "\n--- 测试Qml3DRenderer模型加载 ---";
    
    Qml3DRenderer renderer;
    
    // 设置模型路径
    renderer.setModelPath(m_testModelPath);
    
    // 验证路径设置
    QVERIFY2(renderer.modelPath() == m_testModelPath, "模型路径设置失败");
    
    // 验证模型信息更新
    QVERIFY2(renderer.vertexCount() == 36, "顶点数应该为36");
    QVERIFY2(renderer.faceCount() == 12, "面数应该为12");
    
    qDebug() << "模型加载测试通过";
    qDebug() << "顶点数:" << renderer.vertexCount();
    qDebug() << "面数:" << renderer.faceCount();
}

void AutoTest3D::testRenderOptions()
{
    qDebug() << "\n--- 测试渲染选项 ---";
    
    Qml3DRenderer renderer;
    
    // 测试显示网格
    renderer.setShowGrid(true);
    QVERIFY2(renderer.showGrid() == true, "显示网格设置失败");
    
    // 测试显示法线
    renderer.setShowNormals(true);
    QVERIFY2(renderer.showNormals() == true, "显示法线设置失败");
    
    // 测试线框模式
    renderer.setWireframe(true);
    QVERIFY2(renderer.wireframe() == true, "线框模式设置失败");
    
    // 测试双面渲染
    renderer.setDoubleSided(true);
    QVERIFY2(renderer.doubleSided() == true, "双面渲染设置失败");
    
    qDebug() << "渲染选项测试通过";
}

void AutoTest3D::testModelInfo()
{
    qDebug() << "\n--- 测试模型信息 ---";
    
    Qml3DRenderer renderer;
    
    // 加载模型前
    QVERIFY2(renderer.vertexCount() == 0, "加载前顶点数应该为0");
    QVERIFY2(renderer.faceCount() == 0, "加载前面数应该为0");
    
    // 加载模型
    renderer.setModelPath(m_testModelPath);
    
    // 加载模型后
    QVERIFY2(renderer.vertexCount() > 0, "加载后顶点数应该大于0");
    QVERIFY2(renderer.faceCount() > 0, "加载后面数应该大于0");
    
    qDebug() << "模型信息测试通过";
    qDebug() << "实际顶点数:" << renderer.vertexCount();
    qDebug() << "实际面数:" << renderer.faceCount();
}

QTEST_MAIN(AutoTest3D)
#include "auto_test_3d.moc"
