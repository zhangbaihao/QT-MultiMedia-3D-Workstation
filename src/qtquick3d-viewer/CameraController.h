// CameraController.h
#pragma once
#include <QObject>
#include <QVector3D>
#include <QQuaternion>
#include <QPointF>

class CameraController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(float distance READ distance WRITE setDistance NOTIFY distanceChanged)
    
public:
    explicit CameraController(QObject* parent = nullptr);
    
    // 暴露给QML的接口
    Q_INVOKABLE void handleMousePress(float x, float y, int button);
    Q_INVOKABLE void handleMouseMove(float x, float y, int buttons);
    Q_INVOKABLE void handleWheel(float delta);
    Q_INVOKABLE void reset();
    
    // 属性访问器
    QVector3D position() const;
    QQuaternion rotation() const;
    float distance() const;
    
public slots:
    void setPosition(const QVector3D& pos);
    void setRotation(const QQuaternion& rot);
    void setDistance(float dist);
    
signals:
    void positionChanged(const QVector3D& pos);
    void rotationChanged(const QQuaternion& rot);
    void distanceChanged(float dist);
    
private:
    // 核心计算函数
    QQuaternion calculateRotation(float dx, float dy);
    QVector3D calculatePan(float dx, float dy, const QQuaternion& camRot);
    
    // 内部状态
    QVector3D m_position = QVector3D(0, 0, 20);
    QQuaternion m_rotation = QQuaternion();
    float m_distance = 20.0f;
    
    // 鼠标状态
    QPointF m_lastMousePos;
    int m_lastButton = 0;
    QVector3D m_target = QVector3D(0, 0, 0);
    
    // 配置参数
    float m_rotationSpeed = 0.005f;
    float m_panSpeed = 0.01f;
    float m_zoomSpeed = 0.001f;
    float m_minDistance = 1.0f;
    float m_maxDistance = 100.0f;
};