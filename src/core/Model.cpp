#include "Model.h"

Model::Model(QObject *parent) : QObject(parent)
{}

Model::~Model()
{}

QString Model::modelPath() const
{
    return m_modelPath;
}

void Model::setModelPath(const QString &path)
{
    if (m_modelPath != path)
    {
        m_modelPath = path;
        emit modelPathChanged();
    }
}

QString Model::videoPath() const
{
    return m_videoPath;
}

void Model::setVideoPath(const QString &path)
{
    if (m_videoPath != path)
    {
        m_videoPath = path;
        emit videoPathChanged();
    }
}
