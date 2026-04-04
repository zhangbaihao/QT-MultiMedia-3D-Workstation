#include "GLWidget.h"
#include "GLRenderer.h"
#include "GLModel.h"
#include "GLCamera.h"
#include <QDebug>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_renderer(nullptr)
    , m_model(nullptr)
    , m_camera(nullptr)
    , m_mousePressed(false)
    , m_autoRotate(false)
    , m_renderMode(0)
{
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
    delete m_renderer;
    delete m_model;
    delete m_camera;
}

void GLWidget::loadModel(const QString &path)
{
    if (!m_model) {
        m_model = new GLModel();
    }
    
    if (m_model->loadModel(path)) {
        m_model->setupBuffers();
        
        // 自动调整相机位置以适应模型
        QVector3D size = m_model->boundingBoxMax() - m_model->boundingBoxMin();
        float maxSize = qMax(size.x(), qMax(size.y(), size.z()));
        float distance = maxSize * 2;
        m_camera->setPosition(QVector3D(0, 0, distance));
        
        // 发送模型加载完成信号
        emit modelLoaded(
            m_model->vertexCount(),
            m_model->faceCount(),
            QString("%1 to %2").arg(m_model->boundingBoxMin().toString()).arg(m_model->boundingBoxMax().toString())
        );
        
        update();
    }
}

void GLWidget::setRenderMode(int mode)
{
    m_renderMode = mode;
    if (m_renderer) {
        m_renderer->setRenderMode(mode);
    }
    update();
}

void GLWidget::setAutoRotate(bool enabled)
{
    m_autoRotate = enabled;
    update();
}

void GLWidget::resetView()
{
    if (m_camera) {
        m_camera->setPosition(QVector3D(0, 0, 10));
        m_camera->setRotation(QQuaternion());
    }
    update();
}

void GLWidget::fitModelToView()
{
    if (m_model) {
        QVector3D size = m_model->boundingBoxMax() - m_model->boundingBoxMin();
        float maxSize = qMax(size.x(), qMax(size.y(), size.z()));
        float distance = maxSize * 2;
        m_camera->setPosition(QVector3D(0, 0, distance));
        update();
    }
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    m_renderer = new GLRenderer();
    m_renderer->initialize();
    m_renderer->setRenderMode(m_renderMode);
    
    m_camera = new GLCamera();
    m_camera->setPosition(QVector3D(0, 0, 10));
    m_camera->setAspectRatio(float(width()) / height());
    
    m_model = new GLModel();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    if (m_camera) {
        m_camera->setAspectRatio(float(w) / h);
    }
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (m_model && m_renderer && m_camera) {
        QMatrix4x4 modelMatrix;
        QMatrix4x4 viewMatrix = m_camera->viewMatrix();
        QMatrix4x4 projectionMatrix = m_camera->projectionMatrix();
        
        // 自动旋转
        if (m_autoRotate) {
            static float angle = 0.0f;
            angle += 0.01f;
            modelMatrix.rotate(angle, 0, 1, 0);
        }
        
        m_renderer->render(m_model, modelMatrix, viewMatrix, projectionMatrix);
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousePressed = true;
    m_lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mousePressed && m_camera) {
        QPoint delta = event->pos() - m_lastMousePos;
        
        if (event->buttons() & Qt::LeftButton) {
            // 旋转相机
            m_camera->rotate(delta.x() * 0.01f, delta.y() * 0.01f);
        } else if (event->buttons() & Qt::RightButton) {
            // 平移相机
            m_camera->translate(-delta.x() * 0.01f, delta.y() * 0.01f, 0);
        }
        
        m_lastMousePos = event->pos();
        update();
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (m_camera) {
        float zoomFactor = 1.0f + event->angleDelta().y() * 0.001f;
        m_camera->zoom(zoomFactor);
        update();
    }
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_R:
        resetView();
        break;
    case Qt::Key_F:
        fitModelToView();
        break;
    default:
        QOpenGLWidget::keyPressEvent(event);
        break;
    }
}