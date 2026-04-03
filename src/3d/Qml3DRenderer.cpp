#include "Qml3DRenderer.h"
#include <QQuickWindow>
#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGFlatColorMaterial>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Qml3DRenderer::Qml3DRenderer(QQuickItem *parent)
    : QQuickItem(parent)
    , m_modelColor(Qt::blue)
    , m_rotationX(0)
    , m_rotationY(0)
    , m_zoom(1.0)
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
    if (m_modelPath != path) {
        m_modelPath = path;
        emit modelPathChanged();
        update();
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

qreal Qml3DRenderer::rotationX() const
{
    return m_rotationX;
}

void Qml3DRenderer::setRotationX(qreal rotation)
{
    if (m_rotationX != rotation) {
        m_rotationX = rotation;
        emit rotationXChanged();
        update();
    }
}

qreal Qml3DRenderer::rotationY() const
{
    return m_rotationY;
}

void Qml3DRenderer::setRotationY(qreal rotation)
{
    if (m_rotationY != rotation) {
        m_rotationY = rotation;
        emit rotationYChanged();
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

QSGNode *Qml3DRenderer::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    QSGGeometryNode *node = static_cast<QSGGeometryNode *>(oldNode);
    if (!node) {
        node = new QSGGeometryNode();

        // 创建立方体的几何数据
        QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 8);
        geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        // 创建材质
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial();
        material->setColor(m_modelColor);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    }

    // 更新几何数据 - 绘制一个立方体的轮廓
    QSGGeometry *geometry = node->geometry();
    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();

    // 计算旋转后的顶点位置
    qreal cx = width() / 2;
    qreal cy = height() / 2;
    qreal size = 100 * m_zoom;

    // 简化的立方体投影
    qreal cosX = cos(m_rotationX * M_PI / 180);
    qreal sinX = sin(m_rotationX * M_PI / 180);
    qreal cosY = cos(m_rotationY * M_PI / 180);
    qreal sinY = sin(m_rotationY * M_PI / 180);

    // 立方体的8个顶点
    qreal x[8] = {-1, 1, -1, 1, -1, 1, -1, 1};
    qreal y[8] = {-1, -1, 1, 1, -1, -1, 1, 1};
    qreal z[8] = {-1, -1, -1, -1, 1, 1, 1, 1};

    for (int i = 0; i < 8; i++) {
        // 应用Y轴旋转
        qreal rx = x[i] * cosY - z[i] * sinY;
        qreal rz = x[i] * sinY + z[i] * cosY;
        qreal ry = y[i];

        // 应用X轴旋转
        qreal ry2 = ry * cosX - rz * sinX;
        qreal rz2 = ry * sinX + rz * cosX;

        // 投影到2D
        qreal scale = 1.0 / (1.0 + rz2 * 0.3);
        vertices[i].x = cx + rx * size * scale;
        vertices[i].y = cy + ry2 * size * scale;
    }

    geometry->markVertexDataDirty();

    // 更新材质颜色
    QSGFlatColorMaterial *material = static_cast<QSGFlatColorMaterial *>(node->material());
    material->setColor(m_modelColor);

    node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return node;
}

void Qml3DRenderer::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    if (newGeometry.size() != oldGeometry.size()) {
        update();
    }
}
