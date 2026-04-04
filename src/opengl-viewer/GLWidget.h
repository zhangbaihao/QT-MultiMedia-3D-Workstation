#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

class GLRenderer;
class GLModel;
class GLCamera;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    void loadModel(const QString &path);
    void setRenderMode(int mode); // 0: Solid, 1: Wireframe, 2: Points, 3: Solid+Wireframe
    void setAutoRotate(bool enabled);
    void resetView();
    void fitModelToView();

signals:
    void modelLoaded(int vertexCount, int faceCount, const QString &boundingBox);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    GLRenderer *m_renderer;
    GLModel *m_model;
    GLCamera *m_camera;
    
    bool m_mousePressed;
    QPoint m_lastMousePos;
    bool m_autoRotate;
    int m_renderMode;
};

#endif // GLWIDGET_H