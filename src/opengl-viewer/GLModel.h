#ifndef GLMODEL_H
#define GLMODEL_H

#include <QVector3D>
#include <QVector2D>
#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <assimp/scene.h>

struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector2D texCoord;
};

#include <QOpenGLFunctions_3_3_Core>

class GLModel : protected QOpenGLFunctions_3_3_Core
{
public:
    GLModel();
    ~GLModel();

    bool loadModel(const QString &path);
    void setupBuffers();
    void bind();
    void release();

    int vertexCount() const;
    int faceCount() const;
    QVector3D boundingBoxMin() const;
    QVector3D boundingBoxMax() const;

private:
    void calculateBoundingBox();
    void processNode(const struct aiNode *node, const struct aiScene *scene);
    void processMesh(const struct aiMesh *mesh, const struct aiScene *scene);

    QVector<Vertex> m_vertices;
    QVector<unsigned int> m_indices;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_indexBuffer;
    QOpenGLVertexArrayObject m_vao;
    
    int m_vertexCount;
    int m_faceCount;
    QVector3D m_boundingBoxMin;
    QVector3D m_boundingBoxMax;
};

#endif // GLMODEL_H