#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QObject>
#include "Model.h"

class ViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Model* model READ model CONSTANT)

public:
    explicit ViewModel(QObject *parent = nullptr);
    ~ViewModel();

    Model* model() const;

public slots:
    void loadModel(const QString &path);
    void loadVideo(const QString &path);

private:
    Model* m_model;
};

#endif // VIEWMODEL_H
