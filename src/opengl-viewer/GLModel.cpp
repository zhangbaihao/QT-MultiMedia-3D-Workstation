#include "GLModel.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <QDebug>

GLModel::GLModel()
    : m_vertexCount(0)
    , m_faceCount(0)
    , m_boundingBoxMin(0, 0, 0)
    , m_boundingBoxMax(0, 0, 0)
{
}

GLModel::~GLModel()
{
    m_vertexBuffer.destroy();
    m_indexBuffer.destroy();
    m_vao.destroy();
}

bool GLModel::loadModel(const QString &path)
{
    m_vertices.clear();
    m_indices.clear();
    m_vertexCount = 0;
    m_faceCount = 0;
    m_boundingBoxMin = QVector3D(0, 0, 0);
    m_boundingBoxMax = QVector3D(0, 0, 0);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.toStdString(), 
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        qDebug() << "Assimp error:" << importer.GetErrorString();
        return false;
    }

    // 处理场景节点
    processNode(scene->mRootNode, scene);
    calculateBoundingBox();

    m_vertexCount = m_vertices.size();
    m_faceCount = m_indices.size() / 3;

    qDebug() << "Model loaded successfully:" << m_vertexCount << "vertices," << m_faceCount << "faces";
    qDebug() << "Bounding box:" << m_boundingBoxMin << "to" << m_boundingBoxMax;

    return true;
}

void GLModel::processNode(const aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

void GLModel::processMesh(const aiMesh *mesh, const aiScene *scene)
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

void GLModel::calculateBoundingBox()
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

void GLModel::setupBuffers()
{
    if (m_vertices.isEmpty()) {
        return;
    }

    // 初始化 OpenGL 函数
    initializeOpenGLFunctions();

    m_vao.create();
    m_vertexBuffer.create();
    m_indexBuffer.create();

    m_vao.bind();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(m_vertices.constData(), m_vertices.size() * sizeof(Vertex));

    m_indexBuffer.bind();
    m_indexBuffer.allocate(m_indices.constData(), m_indices.size() * sizeof(unsigned int));

    // 设置顶点属性
    // 位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    
    // 法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(QVector3D)));
    
    // 纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(QVector3D) + sizeof(QVector3D)));

    m_vao.release();
    m_vertexBuffer.release();
    m_indexBuffer.release();
}

void GLModel::bind()
{
    m_vao.bind();
}

void GLModel::release()
{
    m_vao.release();
}

int GLModel::vertexCount() const
{
    return m_vertexCount;
}

int GLModel::faceCount() const
{
    return m_faceCount;
}

QVector3D GLModel::boundingBoxMin() const
{
    return m_boundingBoxMin;
}

QVector3D GLModel::boundingBoxMax() const
{
    return m_boundingBoxMax;
}