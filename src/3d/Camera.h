#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
public:
    Camera();
    ~Camera();

    void setPosition(const QVector3D &position);
    void setTarget(const QVector3D &target);
    void setUp(const QVector3D &up);

    void rotate(float yaw, float pitch);
    void translate(const QVector3D &offset);
    void zoom(float factor);

    QMatrix4x4 viewMatrix() const;
    QMatrix4x4 projectionMatrix() const;

    void setPerspective(float fov, float aspect, float nearPlane, float farPlane);

private:
    QVector3D m_position;
    QVector3D m_target;
    QVector3D m_up;
    QMatrix4x4 m_projection;
    float m_yaw;
    float m_pitch;
};

#endif // CAMERA_H
