#ifndef VIDEOFRAMEIMAGEPROVIDER_H
#define VIDEOFRAMEIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>
#include <QMutex>

class VideoFrameImageProvider : public QQuickImageProvider
{
public:
    VideoFrameImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    void updateFrame(const QImage &frame);

private:
    QMutex m_mutex;
    QImage m_image;
};

#endif
