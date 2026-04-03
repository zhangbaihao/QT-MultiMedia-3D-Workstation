#include "Camera.h"

Camera::Camera()
    : m_position(0.0f, 0.0f, 5.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_yaw(0.0f)
    , m_pitch(0.0f)
{
    m_projection.setToIdentity();
}

Camera::~Camera()
{}

void Camera::setPosition(const QVector3D &position)
{
    m_position = position;
}

void Camera::setTarget(const QVector3D &target)
{
    m_target = target;
}

void Camera::setUp(const QVector3D &up)
{
    m_up = up;
}

void Camera::rotate(float yaw, float pitch)
{
    m_yaw += yaw;
    m_pitch += pitch;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    QVector3D front;
    front.setX(cos(m_yaw * M_PI / 180.0f) * cos(m_pitch * M_PI / 180.0f));
    front.setY(sin(m_pitch * M_PI / 180.0f));
    front.setZ(sin(m_yaw * M_PI / 180.0f) * cos(m_pitch * M_PI / 180.0f));
    m_target = m_position + front.normalized();
}

void Camera::translate(const QVector3D &offset)
{
    m_position += offset;
    m_target += offset;
}

void Camera::zoom(float factor)
{
    QVector3D direction = m_target - m_position;
    direction.normalize();
    m_position += direction * factor;
}

QMatrix4x4 Camera::viewMatrix() const
{
    QMatrix4x4 view;
    view.lookAt(m_position, m_target, m_up);
    return view;
}

QMatrix4x4 Camera::projectionMatrix() const
{
    return m_projection;
}

void Camera::setPerspective(float fov, float aspect, float nearPlane, float farPlane)
{
    m_projection.setToIdentity();
    m_projection.perspective(fov, aspect, nearPlane, farPlane);
}
