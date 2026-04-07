#pragma once

#include <QQuickFramebufferObject>
#include <QOpenGLContext>
#include "OpenGLRenderer.h"

class FramebufferObject : public QQuickFramebufferObject {
    Q_OBJECT

public:
    FramebufferObject(QQuickItem* parent = nullptr);
    ~FramebufferObject();

    Renderer* createRenderer() const override;

    Q_INVOKABLE void setModel(const QString& modelPath);
    Q_INVOKABLE void setColor(float r, float g, float b);
    Q_INVOKABLE void setTransform(const QVector3D& position, const QQuaternion& rotation, float scale);

signals:
    void initialized();

private:
    mutable OpenGLRenderer* m_renderer;
};

class FramebufferRenderer : public QQuickFramebufferObject::Renderer {
public:
    FramebufferRenderer(OpenGLRenderer* renderer);
    ~FramebufferRenderer();

    void render() override;
    void synchronize(QQuickFramebufferObject* item) override;
    QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;

private:
    OpenGLRenderer* m_renderer;
    QSize m_lastSize;
};
