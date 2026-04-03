#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <QtGui/QVector3D>
#include <QtGui/QVector2D>
#include <QVector>
#include <QString>
#include <QByteArray>

struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector2D texCoord;
};

class ModelLoader
{
public:
    ModelLoader();
    ~ModelLoader();

    bool loadSTL(const QString &path, QVector<Vertex> &vertices, QVector<unsigned int> &indices);
    bool loadPLY(const QString &path, QVector<Vertex> &vertices, QVector<unsigned int> &indices);

private:
    bool parseSTLAscii(const QString &content, QVector<Vertex> &vertices, QVector<unsigned int> &indices);
    bool parseSTLBinary(const QByteArray &content, QVector<Vertex> &vertices, QVector<unsigned int> &indices);
    bool parsePLY(const QString &content, QVector<Vertex> &vertices, QVector<unsigned int> &indices);
};

#endif // MODELLOADER_H
