#include "ModelLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

ModelLoader::ModelLoader()
{}

ModelLoader::~ModelLoader()
{}

bool ModelLoader::loadSTL(const QString &path, QVector<Vertex> &vertices, QVector<unsigned int> &indices)
{
    qDebug() << "ModelLoader::loadSTL called with path:" << path;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open STL file:" << path << "Error:" << file.errorString();
        return false;
    }

    QByteArray content = file.readAll();
    file.close();
    qDebug() << "Read" << content.size() << "bytes from file";

    // 检测是ASCII还是二进制STL
    if (content.startsWith("solid"))
    {
        qDebug() << "Detected ASCII STL format";
        bool result = parseSTLAscii(QString(content), vertices, indices);
        qDebug() << "parseSTLAscii result:" << result << "vertices:" << vertices.size() << "indices:" << indices.size();
        return result;
    }
    else
    {
        qDebug() << "Detected Binary STL format";
        bool result = parseSTLBinary(content, vertices, indices);
        qDebug() << "parseSTLBinary result:" << result << "vertices:" << vertices.size() << "indices:" << indices.size();
        return result;
    }
}

bool ModelLoader::loadPLY(const QString &path, QVector<Vertex> &vertices, QVector<unsigned int> &indices)
{
    qDebug() << "ModelLoader::loadPLY called with path:" << path;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open PLY file:" << path << "Error:" << file.errorString();
        return false;
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    qDebug() << "Read" << content.size() << "bytes from file";

    bool result = parsePLY(content, vertices, indices);
    qDebug() << "parsePLY result:" << result << "vertices:" << vertices.size() << "indices:" << indices.size();
    return result;
}

bool ModelLoader::parseSTLAscii(const QString &content, QVector<Vertex> &vertices, QVector<unsigned int> &indices)
{
    QTextStream stream(const_cast<QString*>(&content));
    QString line;
    QVector3D normal;
    QVector<Vertex> faceVertices;

    while (!stream.atEnd())
    {
        stream >> line;
        if (line == "facet")
        {
            stream >> line; // normal
            float nx, ny, nz;
            stream >> nx >> ny >> nz;
            normal.setX(nx);
            normal.setY(ny);
            normal.setZ(nz);
            faceVertices.clear();
        }
        else if (line == "vertex")
        {
            Vertex vertex;
            float x, y, z;
            stream >> x >> y >> z;
            vertex.position.setX(x);
            vertex.position.setY(y);
            vertex.position.setZ(z);
            vertex.normal = normal;
            vertex.texCoord = QVector2D(0.0f, 0.0f);
            faceVertices.append(vertex);
        }
        else if (line == "endfacet")
        {
            if (faceVertices.size() >= 3)
            {
                for (int i = 0; i < 3; ++i)
                {
                    vertices.append(faceVertices[i]);
                    indices.append(vertices.size() - 1);
                }
            }
        }
    }

    return !vertices.isEmpty();
}

bool ModelLoader::parseSTLBinary(const QByteArray &content, QVector<Vertex> &vertices, QVector<unsigned int> &indices)
{
    if (content.size() < 84)
        return false;

    // 跳过80字节的头部
    int offset = 80;

    // 读取三角面数量
    unsigned int triangleCount = *reinterpret_cast<const unsigned int*>(content.constData() + offset);
    offset += 4;

    for (unsigned int i = 0; i < triangleCount; ++i)
    {
        if (offset + 50 > content.size())
            break;

        // 读取法向量
        QVector3D normal;
        normal.setX(*reinterpret_cast<const float*>(content.constData() + offset));
        normal.setY(*reinterpret_cast<const float*>(content.constData() + offset + 4));
        normal.setZ(*reinterpret_cast<const float*>(content.constData() + offset + 8));
        offset += 12;

        // 读取三个顶点
        for (int j = 0; j < 3; ++j)
        {
            Vertex vertex;
            vertex.position.setX(*reinterpret_cast<const float*>(content.constData() + offset));
            vertex.position.setY(*reinterpret_cast<const float*>(content.constData() + offset + 4));
            vertex.position.setZ(*reinterpret_cast<const float*>(content.constData() + offset + 8));
            vertex.normal = normal;
            vertex.texCoord = QVector2D(0.0f, 0.0f);
            vertices.append(vertex);
            indices.append(vertices.size() - 1);
            offset += 12;
        }

        // 跳过2字节的属性
        offset += 2;
    }

    return !vertices.isEmpty();
}

bool ModelLoader::parsePLY(const QString &content, QVector<Vertex> &vertices, QVector<unsigned int> &indices)
{
    QTextStream stream(const_cast<QString*>(&content));
    QString line;
    int vertexCount = 0;
    int faceCount = 0;
    bool inHeader = true;
    bool hasNormals = false;
    bool hasTextureCoords = false;

    while (!stream.atEnd() && inHeader)
    {
        stream >> line;
        if (line == "element")
        {
            stream >> line;
            if (line == "vertex")
            {
                stream >> vertexCount;
            }
            else if (line == "face")
            {
                stream >> faceCount;
            }
        }
        else if (line == "property")
        {
            stream >> line; // float或double
            stream >> line; // 属性名
            if (line == "nx" || line == "ny" || line == "nz")
            {
                hasNormals = true;
            }
            else if (line == "s" || line == "t" || line == "u" || line == "v")
            {
                hasTextureCoords = true;
            }
        }
        else if (line == "end_header")
        {
            inHeader = false;
        }
    }

    qDebug() << "PLY文件信息: 顶点数=" << vertexCount << "面数=" << faceCount << "有法向量=" << hasNormals << "有纹理坐标=" << hasTextureCoords;

    // 读取顶点
    for (int i = 0; i < vertexCount; ++i)
    {
        Vertex vertex;
        float x, y, z;
        stream >> x >> y >> z;
        vertex.position.setX(x);
        vertex.position.setY(y);
        vertex.position.setZ(z);
        
        // 只读取实际存在的属性
        if (hasNormals)
        {
            float nx, ny, nz;
            stream >> nx >> ny >> nz;
            vertex.normal.setX(nx);
            vertex.normal.setY(ny);
            vertex.normal.setZ(nz);
        } else {
            // 如果没有法向量，设置默认值
            vertex.normal = QVector3D(0.0f, 0.0f, 1.0f);
        }
        
        if (hasTextureCoords)
        {
            float tx, ty;
            stream >> tx >> ty;
            vertex.texCoord.setX(tx);
            vertex.texCoord.setY(ty);
        } else {
            // 如果没有纹理坐标，设置默认值
            vertex.texCoord = QVector2D(0.0f, 0.0f);
        }
        
        vertices.append(vertex);
    }

    // 读取面
    for (int i = 0; i < faceCount; ++i)
    {
        int count;
        stream >> count;
        for (int j = 0; j < count; ++j)
        {
            unsigned int index;
            stream >> index;
            indices.append(index);
        }
    }

    qDebug() << "成功加载PLY文件: 顶点数=" << vertices.size() << "面数=" << indices.size()/3;
    return !vertices.isEmpty();
}
