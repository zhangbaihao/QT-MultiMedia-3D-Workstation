#include "ModelLoader.h"
#include <QDebug>
#include <QtConcurrent>
#include <QMetaObject>

OtherModelLoader::OtherModelLoader(QObject *parent) : QObject(parent) {
}

OtherModelLoader::~OtherModelLoader() {
}

void OtherModelLoader::loadModel(const QString& filePath) {
    qDebug() << "OtherModelLoader::loadModel called with:" << filePath;
    
    // 异步执行模型加载
    QtConcurrent::run([this, filePath]() {
        // 发送加载开始的进度信号
        QMetaObject::invokeMethod(this, "loadProgress", Qt::QueuedConnection, Q_ARG(int, 0), Q_ARG(QString, "开始加载模型..."));
        
        // 模拟模型加载的耗时操作
        for (int i = 0; i <= 100; i += 10) {
            QThread::msleep(200);
            QMetaObject::invokeMethod(this, "loadProgress", Qt::QueuedConnection, Q_ARG(int, i), Q_ARG(QString, QString("加载中... %1%").arg(i)));
        }
        
        try {
            // 这里可以实现模型加载逻辑
            // 目前只是模拟加载成功
            m_vertexCount = 1000;
            m_faceCount = 2000;
            m_boundingBoxMin = QVector3D(-1.0f, -1.0f, -1.0f);
            m_boundingBoxMax = QVector3D(1.0f, 1.0f, 1.0f);
            
            QString modelData = QString("{\"vertexCount\":%1,\"faceCount\":%2,\"boundingBox\":{\"min\":[%3,%4,%5],\"max\":[%6,%7,%8]}}")
                .arg(m_vertexCount)
                .arg(m_faceCount)
                .arg(m_boundingBoxMin.x()).arg(m_boundingBoxMin.y()).arg(m_boundingBoxMin.z())
                .arg(m_boundingBoxMax.x()).arg(m_boundingBoxMax.y()).arg(m_boundingBoxMax.z());
            
            // 更新loaded状态并发送信号
            m_loaded = true;
            QMetaObject::invokeMethod(this, "loadedChanged", Qt::QueuedConnection);
            
            // 发送加载完成的进度信号
            QMetaObject::invokeMethod(this, "loadProgress", Qt::QueuedConnection, Q_ARG(int, 100), Q_ARG(QString, "加载完成！"));
            
            // 发送信号，通知QML模型加载完成
            QMetaObject::invokeMethod(this, "modelLoaded", Qt::QueuedConnection, Q_ARG(int, m_vertexCount), Q_ARG(int, m_faceCount), Q_ARG(QString, modelData));
        } catch (const std::exception& e) {
            // 模拟加载失败
            m_loaded = false;
            QMetaObject::invokeMethod(this, "loadedChanged", Qt::QueuedConnection);
            QMetaObject::invokeMethod(this, "modelLoadFailed", Qt::QueuedConnection, Q_ARG(QString, QString("加载失败: %1").arg(e.what())));
        } catch (...) {
            // 模拟加载失败
            m_loaded = false;
            QMetaObject::invokeMethod(this, "loadedChanged", Qt::QueuedConnection);
            QMetaObject::invokeMethod(this, "modelLoadFailed", Qt::QueuedConnection, Q_ARG(QString, "加载失败: 未知错误"));
        }
    });
}

void OtherModelLoader::setSource(const QString& source) {
    qDebug() << "OtherModelLoader::setSource called with:" << source;
    
    if (m_source != source) {
        m_source = source;
        emit sourceChanged();
        
        // 当source变化时，自动加载模型
        if (!source.isEmpty()) {
            loadModel(source);
        }
    }
}

int OtherModelLoader::getVertexCount() const {
    return m_vertexCount;
}

int OtherModelLoader::getFaceCount() const {
    return m_faceCount;
}

QVector3D OtherModelLoader::getBoundingBoxMin() const {
    return m_boundingBoxMin;
}

QVector3D OtherModelLoader::getBoundingBoxMax() const {
    return m_boundingBoxMax;
}
