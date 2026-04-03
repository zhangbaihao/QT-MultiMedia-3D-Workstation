#ifndef RENDERER_H
#define RENDERER_H

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector>
#include "Camera.h"
#include "ModelLoader.h"

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Renderer(QWidget *parent = nullptr);
    ~Renderer();

    void loadModel(const QString &path);
    void setCameraPosition(const QVector3D &position);
    void setCameraTarget(const QVector3D &target);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void setupShaders();
    void setupBuffers();

    Camera m_camera;
    ModelLoader m_modelLoader;
    QVector<Vertex> m_vertices;
    QVector<unsigned int> m_indices;
    GLuint m_program;
    GLuint m_vbo;
    GLuint m_ebo;
    QPoint m_lastMousePos;
    bool m_isDragging;
};

#endif // RENDERER_H
