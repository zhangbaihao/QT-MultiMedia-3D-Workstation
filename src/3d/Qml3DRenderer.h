#ifndef QML3DRENDERER_H
#define QML3DRENDERER_H

#include <QQuickItem>
#include <QColor>

class Qml3DRenderer : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString modelPath READ modelPath WRITE setModelPath NOTIFY modelPathChanged)
    Q_PROPERTY(QColor modelColor READ modelColor WRITE setModelColor NOTIFY modelColorChanged)
    Q_PROPERTY(qreal rotationX READ rotationX WRITE setRotationX NOTIFY rotationXChanged)
    Q_PROPERTY(qreal rotationY READ rotationY WRITE setRotationY NOTIFY rotationYChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

public:
    explicit Qml3DRenderer(QQuickItem *parent = nullptr);
    ~Qml3DRenderer();

    QString modelPath() const;
    void setModelPath(const QString &path);

    QColor modelColor() const;
    void setModelColor(const QColor &color);

    qreal rotationX() const;
    void setRotationX(qreal rotation);

    qreal rotationY() const;
    void setRotationY(qreal rotation);

    qreal zoom() const;
    void setZoom(qreal zoom);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

signals:
    void modelPathChanged();
    void modelColorChanged();
    void rotationXChanged();
    void rotationYChanged();
    void zoomChanged();

private:
    QString m_modelPath;
    QColor m_modelColor;
    qreal m_rotationX;
    qreal m_rotationY;
    qreal m_zoom;
};

#endif // QML3DRENDERER_H
