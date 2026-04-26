#include "VideoFrameImageProvider.h"
#include <QDebug>

VideoFrameImageProvider::VideoFrameImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage VideoFrameImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);
    QMutexLocker locker(&m_mutex);

    if (m_image.isNull()) {
        if (size)
            *size = QSize(1, 1);
        // 返回一个 1x1 透明像素，避免 QML Image 组件因空 source 产生警告
        return QImage(1, 1, QImage::Format_ARGB32_Premultiplied);
    }

    if (size)
        *size = m_image.size();

    if (requestedSize.isValid() && requestedSize.width() > 0 && requestedSize.height() > 0) {
        return m_image.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return m_image.copy();
}

void VideoFrameImageProvider::updateFrame(const QImage &frame)
{
    QMutexLocker locker(&m_mutex);
    if (!frame.isNull())
        m_image = frame;
}
