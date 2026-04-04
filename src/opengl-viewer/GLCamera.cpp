#include "GLCamera.h"

GLCamera::GLCamera()
    : m_position(0.0f, 0.0f, 10.0f)
    , m_rotation()
    , m_fov(45.0f)
    , m_aspect(1.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(1000.0f)
{
}

GLCamera::~GLCamera()
{
}

void GLCamera::setPosition(const QVector3D &position)
{
    m_position = position;
}

void GLCamera::setRotation(const QQuaternion &rotation)
{
    m_rotation = rotation;
}

void GLCamera::setFieldOfView(float fov)
{
    m_fov = fov;
}

void GLCamera::setAspectRatio(float aspect)
{
    m_aspect = aspect;
}

void GLCamera::setNearPlane(float nearPlane)
{
    m_nearPlane = nearPlane;
}

void GLCamera::setFarPlane(float farPlane)
{
    m_farPlane = farPlane;
}

QVector3D GLCamera::position() const
{
    return m_position;
}

QQuaternion GLCamera::rotation() const
{
    return m_rotation;
}

QMatrix4x4 GLCamera::viewMatrix() const
{
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(-m_position);
    viewMatrix.rotate(m_rotation);
    return viewMatrix;
}

QMatrix4x4 GLCamera::projectionMatrix() const
{
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
    return projectionMatrix;
}

void GLCamera::rotate(float deltaX, float deltaY)
{
    QQuaternion rotX = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -deltaY * 180.0f / M_PI);
    QQuaternion rotY = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), deltaX * 180.0f / M_PI);
    m_rotation = rotX * rotY * m_rotation;
}

void GLCamera::translate(float deltaX, float deltaY, float deltaZ)
{
    QVector3D translation(deltaX, deltaY, deltaZ);
    m_position += m_rotation.rotatedVector(translation);
}

void GLCamera::zoom(float factor)
{
    m_position *= factor;
}