#ifndef CUSTOM3DMODEL_H
#define CUSTOM3DMODEL_H

#include <QQuick3DGeometry>
#include <QVector3D>
#include <QVector2D>
#include <QColor>
#include "ModelLoader.h"

class Custom3DModel : public QQuick3DGeometry
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(int vertexCount READ vertexCount NOTIFY vertexCountChanged)
    Q_PROPERTY(int faceCount READ faceCount NOTIFY faceCountChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

public:
    explicit Custom3DModel(QQuick3DObject *parent = nullptr);
    ~Custom3DModel();

    QString source() const;
    void setSource(const QString &source);

    QColor color() const;
    void setColor(const QColor &color);

    int vertexCount() const;
    int faceCount() const;
    bool loaded() const;

signals:
    void sourceChanged();
    void colorChanged();
    void vertexCountChanged();
    void faceCountChanged();
    void loadedChanged();

private:
    void loadModel();
    void updateGeometry();

    QString m_source;
    QColor m_color;
    QVector<Vertex> m_vertices;
    QVector<unsigned int> m_indices;
    int m_vertexCount;
    int m_faceCount;
    bool m_loaded;
};

#endif // CUSTOM3DMODEL_H
