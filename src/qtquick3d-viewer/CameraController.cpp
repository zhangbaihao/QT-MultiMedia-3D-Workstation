// CameraController.cpp
#include "CameraController.h"
#include <QtMath>

CameraController::CameraController(QObject* parent) : QObject(parent) {}

void CameraController::handleMousePress(float x, float y, int button) {
    m_lastMousePos = QPointF(x, y);
    m_lastButton = button;
}

void CameraController::handleMouseMove(float x, float y, int buttons) {
    if (!(buttons & (Qt::LeftButton | Qt::RightButton))) return;
    
    float dx = x - m_lastMousePos.x();
    float dy = y - m_lastMousePos.y();
    
    if (buttons & Qt::LeftButton) {
        // 旋转计算
        QQuaternion newRotation = calculateRotation(dx, dy);
        setRotation(newRotation);
        
        // 更新相机位置（保持距离）
        QVector3D forward = m_rotation * QVector3D(0, 0, 1);
        setPosition(m_target - forward * m_distance);
    } 
    else if (buttons & Qt::RightButton) {
        // 平移计算
        QVector3D panDelta = calculatePan(dx, dy, m_rotation);
        m_target += panDelta;
        setPosition(m_position + panDelta);
    }
    
    m_lastMousePos = QPointF(x, y);
}

void CameraController::handleWheel(float delta) {
    float zoomFactor = 1.0f + delta * m_zoomSpeed;
    float newDistance = m_distance * zoomFactor;
    newDistance = qBound(m_minDistance, newDistance, m_maxDistance);
    
    setDistance(newDistance);
    
    // 更新位置
    QVector3D forward = m_rotation * QVector3D(0, 0, 1);
    setPosition(m_target - forward * newDistance);
}

QQuaternion CameraController::calculateRotation(float dx, float dy) {
    // 使用四元数避免万向锁！
    float pitch = dy * m_rotationSpeed;
    float yaw = dx * m_rotationSpeed;
    
    // 创建四元数旋转
    QQuaternion yawRot = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw);
    QVector3D right = m_rotation * QVector3D(1, 0, 0);
    QQuaternion pitchRot = QQuaternion::fromAxisAndAngle(right, pitch);
    
    return pitchRot * yawRot * m_rotation;
}

QVector3D CameraController::calculatePan(float dx, float dy, const QQuaternion& camRot) {
    // 在相机坐标系中计算平移
    QVector3D right = camRot * QVector3D(1, 0, 0);
    QVector3D up = camRot * QVector3D(0, 1, 0);
    
    return (-right * dx + up * dy) * m_panSpeed;
}

void CameraController::reset() {
    setRotation(QQuaternion());
    setDistance(20.0f);
    m_target = QVector3D(0, 0, 0);
    
    QVector3D forward = m_rotation * QVector3D(0, 0, 1);
    setPosition(m_target - forward * m_distance);
}

// 属性访问器实现...
QVector3D CameraController::position() const { return m_position; }
QQuaternion CameraController::rotation() const { return m_rotation; }
float CameraController::distance() const { return m_distance; }

void CameraController::setPosition(const QVector3D& pos) {
    if (m_position != pos) {
        m_position = pos;
        emit positionChanged(pos);
    }
}

void CameraController::setRotation(const QQuaternion& rot) {
    if (m_rotation != rot) {
        m_rotation = rot;
        emit rotationChanged(rot);
    }
}

void CameraController::setDistance(float dist) {
    if (!qFuzzyCompare(m_distance, dist)) {
        m_distance = dist;
        emit distanceChanged(dist);
    }
}