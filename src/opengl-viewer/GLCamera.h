#ifndef GLCAMERA_H
#define GLCAMERA_H

#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>

class GLCamera
{
public:
    GLCamera();
    ~GLCamera();

    void setPosition(const QVector3D &position);
    void setRotation(const QQuaternion &rotation);
    void setFieldOfView(float fov);
    void setAspectRatio(float aspect);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);

    QVector3D position() const;
    QQuaternion rotation() const;
    QMatrix4x4 viewMatrix() const;
    QMatrix4x4 projectionMatrix() const;

    void rotate(float deltaX, float deltaY);
    void translate(float deltaX, float deltaY, float deltaZ);
    void zoom(float factor);

private:
    QVector3D m_position;
    QQuaternion m_rotation;
    float m_fov;
    float m_aspect;
    float m_nearPlane;
    float m_farPlane;
};

#endif // GLCAMERA_H