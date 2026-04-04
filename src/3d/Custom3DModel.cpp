#include "Custom3DModel.h"
#include <QFile>
#include <QDebug>

Custom3DModel::Custom3DModel(QQuick3DObject *parent)
    : QQuick3DGeometry(parent)
    , m_color(Qt::blue)
    , m_vertexCount(0)
    , m_faceCount(0)
    , m_loaded(false)
{
}

Custom3DModel::~Custom3DModel()
{
}

QString Custom3DModel::source() const
{
    return m_source;
}

void Custom3DModel::setSource(const QString &source)
{
    qDebug() << "Custom3DModel::setSource called with:" << source;
    qDebug() << "Current m_source:" << m_source;
    
    if (m_source != source) {
        m_source = source;
        emit sourceChanged();
        qDebug() << "Source changed, calling loadModel()";
        loadModel();
    } else {
        qDebug() << "Source not changed, skipping loadModel()";
    }
}

QColor Custom3DModel::color() const
{
    return m_color;
}

void Custom3DModel::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        emit colorChanged();
        updateGeometry();
    }
}

int Custom3DModel::vertexCount() const
{
    return m_vertexCount;
}

int Custom3DModel::faceCount() const
{
    return m_faceCount;
}

bool Custom3DModel::loaded() const
{
    return m_loaded;
}

void Custom3DModel::loadModel()
{
    m_loaded = false;
    m_vertices.clear();
    m_indices.clear();
    m_vertexCount = 0;
    m_faceCount = 0;

    if (m_source.isEmpty()) {
        emit loadedChanged();
        emit vertexCountChanged();
        emit faceCountChanged();
        return;
    }

    // 转换 file:/// 路径为本地路径
    QString localPath = m_source;
    if (localPath.startsWith("file:///")) {
        localPath = localPath.mid(8);
    }

    qDebug() << "Loading model from:" << localPath;

    ModelLoader loader;
    bool success = false;

    if (localPath.endsWith(".stl", Qt::CaseInsensitive)) {
        success = loader.loadSTL(localPath, m_vertices, m_indices);
    } else if (localPath.endsWith(".ply", Qt::CaseInsensitive)) {
        success = loader.loadPLY(localPath, m_vertices, m_indices);
    } else {
        qWarning() << "Unsupported file format:" << localPath;
    }

    if (success) {
        m_loaded = true;
        m_vertexCount = m_vertices.size();
        m_faceCount = m_indices.size() / 3;
        qDebug() << "Model loaded successfully:" << m_vertexCount << "vertices," << m_faceCount << "faces";
        updateGeometry();
    } else {
        qWarning() << "Failed to load model:" << localPath;
    }

    emit loadedChanged();
    emit vertexCountChanged();
    emit faceCountChanged();
}

void Custom3DModel::updateGeometry()
{
    if (m_vertices.isEmpty()) {
        return;
    }

    // 清除旧的几何数据
    clear();

    // 设置顶点属性
    setStride(sizeof(Vertex));
    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);

    // 添加位置属性
    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                 0,
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);

    // 添加法线属性
    addAttribute(QQuick3DGeometry::Attribute::NormalSemantic,
                 sizeof(QVector3D),
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);

    // 添加纹理坐标属性
    addAttribute(QQuick3DGeometry::Attribute::TexCoordSemantic,
                 sizeof(QVector3D) + sizeof(QVector3D),
                 QQuick3DGeometry::Attribute::ComponentType::F32Type);

    // 添加索引属性
    addAttribute(QQuick3DGeometry::Attribute::IndexSemantic,
                 0,
                 QQuick3DGeometry::Attribute::ComponentType::U32Type);

    // 设置顶点和索引数据
    QByteArray vertexData;
    vertexData.resize(m_vertices.size() * sizeof(Vertex));
    memcpy(vertexData.data(), m_vertices.data(), vertexData.size());

    QByteArray indexData;
    indexData.resize(m_indices.size() * sizeof(unsigned int));
    memcpy(indexData.data(), m_indices.data(), indexData.size());

    setVertexData(vertexData);
    setIndexData(indexData);
    setBounds(QVector3D(-100, -100, -100), QVector3D(100, 100, 100));

    update();
}
