#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <QQuick3DGeometry>
#include <QVector3D>
#include <QVector2D>
#include <QString>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector2D texCoord;
};

class ModelLoader : public QQuick3DGeometry
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int vertexCount READ vertexCount NOTIFY vertexCountChanged)
    Q_PROPERTY(int faceCount READ faceCount NOTIFY faceCountChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(QVector3D boundingBoxMin READ boundingBoxMin NOTIFY boundingBoxChanged)
    Q_PROPERTY(QVector3D boundingBoxMax READ boundingBoxMax NOTIFY boundingBoxChanged)

public:
    explicit ModelLoader(QQuick3DObject *parent = nullptr);
    ~ModelLoader();

    QString source() const;
    void setSource(const QString &source);

    int vertexCount() const;
    int faceCount() const;
    bool loaded() const;
    QVector3D boundingBoxMin() const;
    QVector3D boundingBoxMax() const;

signals:
    void sourceChanged();
    void vertexCountChanged();
    void faceCountChanged();
    void loadedChanged();
    void boundingBoxChanged();

private:
    void loadModel();
    void processNode(const aiNode *node, const aiScene *scene);
    void processMesh(const aiMesh *mesh, const aiScene *scene);
    void updateGeometry();
    void calculateBoundingBox();

    QString m_source;
    QVector<Vertex> m_vertices;
    QVector<unsigned int> m_indices;
    int m_vertexCount;
    int m_faceCount;
    bool m_loaded;
    QVector3D m_boundingBoxMin;
    QVector3D m_boundingBoxMax;
};

#endif // MODELLOADER_H