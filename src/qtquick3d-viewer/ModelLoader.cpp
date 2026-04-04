#include "ModelLoader.h"
#include <QDebug>

ModelLoader::ModelLoader(QQuick3DObject *parent)
    : QQuick3DGeometry(parent)
    , m_vertexCount(0)
    , m_faceCount(0)
    , m_loaded(false)
    , m_boundingBoxMin(0, 0, 0)
    , m_boundingBoxMax(0, 0, 0)
{
}

ModelLoader::~ModelLoader()
{
}

QString ModelLoader::source() const
{
    return m_source;
}

void ModelLoader::setSource(const QString &source)
{
    if (m_source != source) {
        m_source = source;
        emit sourceChanged();
        loadModel();
    }
}

int ModelLoader::vertexCount() const
{
    return m_vertexCount;
}

int ModelLoader::faceCount() const
{
    return m_faceCount;
}

bool ModelLoader::loaded() const
{
    return m_loaded;
}

QVector3D ModelLoader::boundingBoxMin() const
{
    return m_boundingBoxMin;
}

QVector3D ModelLoader::boundingBoxMax() const
{
    return m_boundingBoxMax;
}

void ModelLoader::loadModel()
{
    m_loaded = false;
    m_vertices.clear();
    m_indices.clear();
    m_vertexCount = 0;
    m_faceCount = 0;
    m_boundingBoxMin = QVector3D(0, 0, 0);
    m_boundingBoxMax = QVector3D(0, 0, 0);

    if (m_source.isEmpty()) {
        emit loadedChanged();
        emit vertexCountChanged();
        emit faceCountChanged();
        emit boundingBoxChanged();
        return;
    }

    qDebug() << "Loading model from:" << m_source;

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(m_source.toStdString(), 
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        qDebug() << "Assimp error:" << importer.GetErrorString();
        emit loadedChanged();
        emit vertexCountChanged();
        emit faceCountChanged();
        emit boundingBoxChanged();
        return;
    }

    processNode(scene->mRootNode, scene);
    calculateBoundingBox();
    updateGeometry();

    m_loaded = true;
    m_vertexCount = m_vertices.size();
    m_faceCount = m_indices.size() / 3;

    qDebug() << "Model loaded successfully:" << m_vertexCount << "vertices," << m_faceCount << "faces";
    qDebug() << "Bounding box:" << m_boundingBoxMin << "to" << m_boundingBoxMax;

    emit loadedChanged();
    emit vertexCountChanged();
    emit faceCountChanged();
    emit boundingBoxChanged();
}

void ModelLoader::processNode(const aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

void ModelLoader::processMesh(const aiMesh *mesh, const aiScene *scene)
{
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        
        // 位置
        aiVector3D pos = mesh->mVertices[i];
        vertex.position = QVector3D(pos.x, pos.y, pos.z);
        
        // 法线
        if (mesh->mNormals) {
            aiVector3D normal = mesh->mNormals[i];
            vertex.normal = QVector3D(normal.x, normal.y, normal.z);
        } else {
            vertex.normal = QVector3D(0.0f, 0.0f, 1.0f);
        }
        
        // 纹理坐标
        if (mesh->mTextureCoords[0]) {
            aiVector3D texCoord = mesh->mTextureCoords[0][i];
            vertex.texCoord = QVector2D(texCoord.x, texCoord.y);
        } else {
            vertex.texCoord = QVector2D(0.0f, 0.0f);
        }
        
        m_vertices.append(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            m_indices.append(face.mIndices[j]);
        }
    }
}

void ModelLoader::calculateBoundingBox()
{
    if (m_vertices.isEmpty()) {
        return;
    }

    m_boundingBoxMin = m_vertices[0].position;
    m_boundingBoxMax = m_vertices[0].position;

    for (const auto &vertex : m_vertices) {
        m_boundingBoxMin.setX(qMin(m_boundingBoxMin.x(), vertex.position.x()));
        m_boundingBoxMin.setY(qMin(m_boundingBoxMin.y(), vertex.position.y()));
        m_boundingBoxMin.setZ(qMin(m_boundingBoxMin.z(), vertex.position.z()));
        
        m_boundingBoxMax.setX(qMax(m_boundingBoxMax.x(), vertex.position.x()));
        m_boundingBoxMax.setY(qMax(m_boundingBoxMax.y(), vertex.position.y()));
        m_boundingBoxMax.setZ(qMax(m_boundingBoxMax.z(), vertex.position.z()));
    }
}

void ModelLoader::updateGeometry()
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
    setBounds(m_boundingBoxMin, m_boundingBoxMax);

    update();
}