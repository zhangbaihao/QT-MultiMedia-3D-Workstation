#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QQuaternion>
#include <QVector3D>
#include <vector>
#include "ModelLoader.h"



class OpenGLRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
{
public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void synchronize(QQuickFramebufferObject *item) override;
    void render() override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

private:
    QString m_loadedModel;
    qreal m_offsetX = 0;
    qreal m_offsetY = 0;
    qreal m_rotationX = 0;
    qreal m_rotationY = 0;
    qreal m_scaleFactor = 1.0;
    qreal m_translationX = 0;
    qreal m_translationY = 0;
    QQuaternion m_currentRotation;

    QVector3D center;
    float m_time;
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer m_vbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_ebo{QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject m_vao;
    int m_indicesCount;

    void initShaders();
    void loadMesh(const QString &filePath);
    void initializeBuffers(const QVector<Vertex> &vertices, const QVector<unsigned int> &indices);
};

class OpenGLItem : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(QColor modelColor READ modelColor WRITE setModelColor NOTIFY modelColorChanged)
    Q_PROPERTY(qreal scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(qreal translationX READ translationX WRITE setTranslationX NOTIFY translationXChanged)
    Q_PROPERTY(qreal translationY READ translationY WRITE setTranslationY NOTIFY translationYChanged)

public:
    explicit OpenGLItem(QQuickItem *parent = nullptr);

    Renderer *createRenderer() const override;

    // Getter 和 Setter 函数
    QColor modelColor() const { return m_modelColor; }
    void setModelColor(const QColor &color);

    qreal scaleFactor() const { return m_scaleFactor; }
    void setScaleFactor(qreal value);

    qreal translationX() const { return m_translationX; }
    void setTranslationX(qreal value);

    qreal translationY() const { return m_translationY; }
    void setTranslationY(qreal value);

    qreal rotationX() const { return m_rotationX; }
    qreal rotationY() const { return m_rotationY; }
    qreal offsetX() const { return m_offsetX; }
    qreal offsetY() const { return m_offsetY; }

    Q_INVOKABLE void updateRotation(qreal deltaX, qreal deltaY);
    Q_INVOKABLE void loadModel(const QString &filename);
    Q_INVOKABLE void resetView();

    QString modelFilename() const;

 signals:
    void modelColorChanged();
    void scaleFactorChanged();
    void translationXChanged();
    void translationYChanged();

private:
    QString m_modelFilename; // 存储模型路径
    QColor m_modelColor;
    qreal m_offsetX = 0;
    qreal m_offsetY = 0;
    qreal m_rotationX = 0;
    qreal m_rotationY = 0;
    qreal m_scaleFactor = 1.0;
    qreal m_translationX = 0;
    qreal m_translationY = 0;
};

#endif // OPENGLITEM_H