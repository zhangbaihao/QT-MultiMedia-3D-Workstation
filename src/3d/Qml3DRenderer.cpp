#include "Qml3DRenderer.h"
#include <QQuickWindow>
#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGFlatColorMaterial>
#include <QSGNode>
#include <QVector3D>
#include <QFile>
#include <QFileInfo>
#include <cmath>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Qml3DRenderer::Qml3DRenderer(QQuickItem *parent)
    : QQuickItem(parent)
    , m_modelColor(Qt::blue)
    , m_zoom(1.0)
    , m_cameraX(0)
    , m_cameraY(0)
    , m_cameraZ(3.0f)
    , m_showGrid(false)
    , m_showNormals(false)
    , m_wireframe(false)
    , m_doubleSided(false)
    , m_vertexCount(0)
    , m_faceCount(0)
{
    setFlag(ItemHasContents, true);
}

Qml3DRenderer::~Qml3DRenderer()
{
}

QString Qml3DRenderer::modelPath() const
{
    return m_modelPath;
}

void Qml3DRenderer::setModelPath(const QString &path)
{
    qDebug() << "Qml3DRenderer::setModelPath called with path:" << path;
    if (m_modelPath != path) {
        m_modelPath = path;
        qDebug() << "Model path updated to:" << m_modelPath;
        emit modelPathChanged();
        qDebug() << "Calling loadModelFile()...";
        if (loadModelFile()) {
            qDebug() << "Model loaded successfully!";
            updateModelInfo();
            qDebug() << "Model info updated:" << m_vertexCount << "vertices," << m_faceCount << "faces";
        } else {
            qDebug() << "Model loading failed!";
        }
        update();
        qDebug() << "Update called";
    } else {
        qDebug() << "Model path unchanged:" << path;
    }
}

QColor Qml3DRenderer::modelColor() const
{
    return m_modelColor;
}

void Qml3DRenderer::setModelColor(const QColor &color)
{
    if (m_modelColor != color) {
        m_modelColor = color;
        emit modelColorChanged();
        update();
    }
}

qreal Qml3DRenderer::zoom() const
{
    return m_zoom;
}

void Qml3DRenderer::setZoom(qreal zoom)
{
    if (m_zoom != zoom) {
        m_zoom = zoom;
        emit zoomChanged();
        update();
    }
}

qreal Qml3DRenderer::cameraX() const
{
    return m_cameraX;
}

void Qml3DRenderer::setCameraX(qreal x)
{
    if (!qFuzzyCompare(m_cameraX, x)) {
        m_cameraX = x;
        emit cameraXChanged();
        update();
    }
}

qreal Qml3DRenderer::cameraY() const
{
    return m_cameraY;
}

void Qml3DRenderer::setCameraY(qreal y)
{
    if (!qFuzzyCompare(m_cameraY, y)) {
        m_cameraY = y;
        emit cameraYChanged();
        update();
    }
}

qreal Qml3DRenderer::cameraZ() const
{
    return m_cameraZ;
}

void Qml3DRenderer::setCameraZ(qreal z)
{
    if (!qFuzzyCompare(m_cameraZ, z)) {
        m_cameraZ = z;
        emit cameraZChanged();
        update();
    }
}

void Qml3DRenderer::rotateByMouse(qreal dx, qreal dy)
{
    QQuaternion qY = QQuaternion::fromAxisAndAngle(0, 1, 0, -dx * 0.5);
    QQuaternion qX = QQuaternion::fromAxisAndAngle(1, 0, 0, -dy * 0.5);
    m_rotation = qY * m_rotation * qX;
    update();
}

void Qml3DRenderer::panByMouse(qreal dx, qreal dy)
{
    qreal panSpeed = 0.005 * m_zoom;
    m_cameraX += dx * panSpeed;
    m_cameraY += dy * panSpeed;
    emit cameraXChanged();
    emit cameraYChanged();
    update();
}

void Qml3DRenderer::resetView()
{
    m_rotation = QQuaternion();
    m_cameraX = 0;
    m_cameraY = 0;
    m_cameraZ = 3.0f;
    emit cameraXChanged();
    emit cameraYChanged();
    emit cameraZChanged();
    update();
}

bool Qml3DRenderer::showGrid() const
{
    return m_showGrid;
}

void Qml3DRenderer::setShowGrid(bool showGrid)
{
    if (m_showGrid != showGrid) {
        m_showGrid = showGrid;
        emit showGridChanged();
        update();
    }
}

bool Qml3DRenderer::showNormals() const
{
    return m_showNormals;
}

void Qml3DRenderer::setShowNormals(bool showNormals)
{
    if (m_showNormals != showNormals) {
        m_showNormals = showNormals;
        emit showNormalsChanged();
        update();
    }
}

bool Qml3DRenderer::wireframe() const
{
    return m_wireframe;
}

void Qml3DRenderer::setWireframe(bool wireframe)
{
    if (m_wireframe != wireframe) {
        m_wireframe = wireframe;
        emit wireframeChanged();
        update();
    }
}

bool Qml3DRenderer::doubleSided() const
{
    return m_doubleSided;
}

void Qml3DRenderer::setDoubleSided(bool doubleSided)
{
    if (m_doubleSided != doubleSided) {
        m_doubleSided = doubleSided;
        emit doubleSidedChanged();
        update();
    }
}

int Qml3DRenderer::vertexCount() const
{
    return m_vertexCount;
}

int Qml3DRenderer::faceCount() const
{
    return m_faceCount;
}

bool Qml3DRenderer::loadModelFile()
{
    qDebug() << "Qml3DRenderer::loadModelFile called with path:" << m_modelPath;
    
    if (m_modelPath.isEmpty()) {
        qDebug() << "Model path is empty";
        m_vertices.clear();
        m_indices.clear();
        return false;
    }

    qDebug() << "Model path exists:" << QFile::exists(m_modelPath);
    qDebug() << "File size:" << QFileInfo(m_modelPath).size() << "bytes";

    ModelLoader loader;
    bool result = false;
    
    if (m_modelPath.endsWith(".stl", Qt::CaseInsensitive)) {
        qDebug() << "Loading STL file...";
        result = loader.loadSTL(m_modelPath, m_vertices, m_indices);
        qDebug() << "STL loading result:" << result;
    } else if (m_modelPath.endsWith(".ply", Qt::CaseInsensitive)) {
        qDebug() << "Loading PLY file...";
        result = loader.loadPLY(m_modelPath, m_vertices, m_indices);
        qDebug() << "PLY loading result:" << result;
    } else {
        qDebug() << "Unsupported file format:" << m_modelPath;
    }
    
    qDebug() << "Loaded" << m_vertices.size() << "vertices and" << m_indices.size() << "indices";
    
    return result;
}

void Qml3DRenderer::updateModelInfo()
{
    m_vertexCount = m_vertices.size();
    m_faceCount = m_indices.size() / 3;
    
    emit vertexCountChanged();
    emit faceCountChanged();
}

QVector3D Qml3DRenderer::applyRotation(const QVector3D& v) const
{
    QQuaternion p(0, v.x(), v.y(), v.z());
    QQuaternion result = m_rotation * p * m_rotation.conjugated();
    return QVector3D(result.x(), result.y(), result.z());
}

QSGNode *Qml3DRenderer::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    QSGGeometryNode *node = static_cast<QSGGeometryNode *>(oldNode);
    
    qreal cx = width() / 2;
    qreal cy = height() / 2;
    qreal size = 100 * m_zoom;
    
    // 立方体顶点
    QVector3D cubeVertices[8] = {
        QVector3D(-1, -1, -1),
        QVector3D(1, -1, -1),
        QVector3D(1, 1, -1),
        QVector3D(-1, 1, -1),
        QVector3D(-1, -1, 1),
        QVector3D(1, -1, 1),
        QVector3D(1, 1, 1),
        QVector3D(-1, 1, 1)
    };
    
    // 立方体边
    int cubeEdges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    
    // 创建或更新几何节点
    if (!node) {
        node = new QSGGeometryNode();
        // 12条边，每条边2个顶点
        QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 24);
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial();
        material->setColor(m_modelColor);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    }
    
    QSGGeometry *geometry = node->geometry();
    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
    
    if (vertices) {
        // 绘制立方体
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 2; j++) {
                QVector3D vertex = cubeVertices[cubeEdges[i][j]];
                // 应用旋转
                QVector3D rotated = applyRotation(vertex);
                // 应用透视投影
                float perspective = m_cameraZ / (m_cameraZ - rotated.z() * 0.5f);
                perspective = qBound(0.1f, perspective, 10.0f);
                // 计算屏幕坐标
                vertices[i * 2 + j].x = cx + rotated.x() * size * perspective - m_cameraX * 100;
                vertices[i * 2 + j].y = cy - rotated.y() * size * perspective + m_cameraY * 100;
            }
        }
        
        geometry->markVertexDataDirty();
        QSGFlatColorMaterial *material = static_cast<QSGFlatColorMaterial *>(node->material());
        if (material) {
            material->setColor(m_modelColor);
        }
        node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
    }

    return node;
}

void Qml3DRenderer::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    if (newGeometry.size() != oldGeometry.size()) {
        update();
    }
}
