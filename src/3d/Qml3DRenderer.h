#ifndef QML3DRENDERER_H
#define QML3DRENDERER_H

#include <QQuickItem>
#include <QColor>
#include <QVector>
#include <QQuaternion>
#include <QVector3D>
#include "ModelLoader.h"

class Qml3DRenderer : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString modelPath READ modelPath WRITE setModelPath NOTIFY modelPathChanged)
    Q_PROPERTY(QColor modelColor READ modelColor WRITE setModelColor NOTIFY modelColorChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    
    Q_PROPERTY(qreal cameraX READ cameraX WRITE setCameraX NOTIFY cameraXChanged)
    Q_PROPERTY(qreal cameraY READ cameraY WRITE setCameraY NOTIFY cameraYChanged)
    Q_PROPERTY(qreal cameraZ READ cameraZ WRITE setCameraZ NOTIFY cameraZChanged)
    
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged)
    Q_PROPERTY(bool showNormals READ showNormals WRITE setShowNormals NOTIFY showNormalsChanged)
    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe NOTIFY wireframeChanged)
    Q_PROPERTY(bool doubleSided READ doubleSided WRITE setDoubleSided NOTIFY doubleSidedChanged)
    
    Q_PROPERTY(int vertexCount READ vertexCount NOTIFY vertexCountChanged)
    Q_PROPERTY(int faceCount READ faceCount NOTIFY faceCountChanged)

public:
    explicit Qml3DRenderer(QQuickItem *parent = nullptr);
    ~Qml3DRenderer();

    QString modelPath() const;
    void setModelPath(const QString &path);

    QColor modelColor() const;
    void setModelColor(const QColor &color);

    qreal zoom() const;
    void setZoom(qreal zoom);

    qreal cameraX() const;
    void setCameraX(qreal x);
    qreal cameraY() const;
    void setCameraY(qreal y);
    qreal cameraZ() const;
    void setCameraZ(qreal z);

    Q_INVOKABLE void rotateByMouse(qreal dx, qreal dy);
    Q_INVOKABLE void panByMouse(qreal dx, qreal dy);
    Q_INVOKABLE void resetView();

    bool showGrid() const;
    void setShowGrid(bool showGrid);
    bool showNormals() const;
    void setShowNormals(bool showNormals);
    bool wireframe() const;
    void setWireframe(bool wireframe);
    bool doubleSided() const;
    void setDoubleSided(bool doubleSided);
    
    int vertexCount() const;
    int faceCount() const;

    signals:
    void modelPathChanged();
    void modelColorChanged();
    void zoomChanged();
    void cameraXChanged();
    void cameraYChanged();
    void cameraZChanged();
    void showGridChanged();
    void showNormalsChanged();
    void wireframeChanged();
    void doubleSidedChanged();
    void vertexCountChanged();
    void faceCountChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    QString m_modelPath;
    QColor m_modelColor;
    qreal m_zoom;
    
    QQuaternion m_rotation;
    qreal m_cameraX;
    qreal m_cameraY;
    qreal m_cameraZ;
    
    bool m_showGrid;
    bool m_showNormals;
    bool m_wireframe;
    bool m_doubleSided;
    
    int m_vertexCount;
    int m_faceCount;
    
    QVector<Vertex> m_vertices;
    QVector<unsigned int> m_indices;
    bool loadModelFile();
    void updateModelInfo();
    QVector3D applyRotation(const QVector3D& v) const;
};

#endif
