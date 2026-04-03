#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QString>

class Model : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modelPath READ modelPath WRITE setModelPath NOTIFY modelPathChanged)
    Q_PROPERTY(QString videoPath READ videoPath WRITE setVideoPath NOTIFY videoPathChanged)

public:
    explicit Model(QObject *parent = nullptr);
    ~Model();

    QString modelPath() const;
    void setModelPath(const QString &path);

    QString videoPath() const;
    void setVideoPath(const QString &path);

signals:
    void modelPathChanged();
    void videoPathChanged();

private:
    QString m_modelPath;
    QString m_videoPath;
};

#endif // MODEL_H
