#include "FramebufferObject.h"
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLContext>

FramebufferObject::FramebufferObject(QQuickItem* parent) 
    : QQuickFramebufferObject(parent), m_renderer(new OpenGLRenderer()) {
}

FramebufferObject::~FramebufferObject() {
    delete m_renderer;
}

QQuickFramebufferObject::Renderer* FramebufferObject::createRenderer() const {
    return new FramebufferRenderer(m_renderer);
}

void FramebufferObject::setModel(const QString& modelPath) {
    m_renderer->setModel(modelPath.toStdString());
    update();
}

void FramebufferObject::setColor(float r, float g, float b) {
    m_renderer->setColor(r, g, b);
    update();
}

void FramebufferObject::setTransform(const QVector3D& position, const QQuaternion& rotation, float scale) {
    m_renderer->setTransform(position, rotation, scale);
    update();
}

FramebufferRenderer::FramebufferRenderer(OpenGLRenderer* renderer) 
    : m_renderer(renderer), m_lastSize(0, 0) {
}

FramebufferRenderer::~FramebufferRenderer() {
}

QOpenGLFramebufferObject* FramebufferRenderer::createFramebufferObject(const QSize& size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

void FramebufferRenderer::synchronize(QQuickFramebufferObject* item) {
    QSize currentSize = item->size().toSize();
    if (currentSize != m_lastSize) {
        m_lastSize = currentSize;
        if (currentSize.width() > 0 && currentSize.height() > 0) {
            if (!m_renderer->isInitialized()) {
                m_renderer->initialize();
                m_renderer->resize(currentSize.width(), currentSize.height());
                // 发送初始化完成信号
                FramebufferObject* fbo = static_cast<FramebufferObject*>(item);
                emit fbo->initialized();
            } else {
                m_renderer->resize(currentSize.width(), currentSize.height());
            }
        }
    }
    
    // 确保渲染器被初始化，即使尺寸为0
    if (!m_renderer->isInitialized() && currentSize.width() > 0 && currentSize.height() > 0) {
        m_renderer->initialize();
        m_renderer->resize(currentSize.width(), currentSize.height());
        // 发送初始化完成信号
        FramebufferObject* fbo = static_cast<FramebufferObject*>(item);
        emit fbo->initialized();
    }
}

void FramebufferRenderer::render() {
    if (m_renderer->isInitialized()) {
        m_renderer->render();
    }
}
