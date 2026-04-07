#pragma once

#include <QObject>
#include <QString>
#include <QVector3D>
#include <QThread>

class OtherModelLoader : public QObject {
    Q_OBJECT

public:
    explicit OtherModelLoader(QObject *parent = nullptr);
    ~OtherModelLoader();

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(int vertexCount READ getVertexCount NOTIFY modelLoaded)
    Q_PROPERTY(int faceCount READ getFaceCount NOTIFY modelLoaded)
    Q_PROPERTY(QVector3D boundingBoxMin READ getBoundingBoxMin NOTIFY modelLoaded)
    Q_PROPERTY(QVector3D boundingBoxMax READ getBoundingBoxMax NOTIFY modelLoaded)

    Q_INVOKABLE void loadModel(const QString& filePath);
    Q_INVOKABLE int getVertexCount() const;
    Q_INVOKABLE int getFaceCount() const;
    Q_INVOKABLE QVector3D getBoundingBoxMin() const;
    Q_INVOKABLE QVector3D getBoundingBoxMax() const;

    QString source() const { return m_source; }
    void setSource(const QString& source);
    bool loaded() const { return m_loaded; }

signals:
    void modelLoaded(int vertexCount, int faceCount, const QString& modelData);
    void modelLoadFailed(const QString& errorMsg);
    void sourceChanged();
    void loadedChanged();
    void loadProgress(int progress, const QString& message);

private:
    int m_vertexCount = 0;
    int m_faceCount = 0;
    QVector3D m_boundingBoxMin;
    QVector3D m_boundingBoxMax;
    QString m_source;
    bool m_loaded = false;
};
