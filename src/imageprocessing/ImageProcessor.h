#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>

// OpenCV头文件（暂时注释，后续可根据实际情况配置）
// #include <opencv2/opencv.hpp>

class ImageProcessor : public QObject
{
    Q_OBJECT

public:
    explicit ImageProcessor(QObject *parent = nullptr);
    ~ImageProcessor();

    // 滤波操作
    QImage applyBlur(const QImage &image, int kernelSize);
    QImage applyGaussianBlur(const QImage &image, int kernelSize, double sigmaX, double sigmaY);
    QImage applyMedianBlur(const QImage &image, int kernelSize);

    // 边缘检测
    QImage applyCanny(const QImage &image, int threshold1, int threshold2, int apertureSize);
    QImage applySobel(const QImage &image, int dx, int dy, int kernelSize);

    // 特征提取
    QImage detectFeatures(const QImage &image, int maxFeatures);

    // 颜色空间转换
    QImage convertToGrayscale(const QImage &image);
    QImage convertToHSV(const QImage &image);

    // 形态学操作
    QImage applyErosion(const QImage &image, int kernelSize);
    QImage applyDilation(const QImage &image, int kernelSize);

private:
    void initializeOpenCL();
    void initializeOpenMP();

    bool m_openclInitialized;
    bool m_openmpInitialized;
};

#endif // IMAGEPROCESSOR_H
