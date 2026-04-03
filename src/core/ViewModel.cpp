#include "ViewModel.h"

ViewModel::ViewModel(QObject *parent) : QObject(parent)
{
    m_model = new Model(this);
}

ViewModel::~ViewModel()
{}

Model* ViewModel::model() const
{
    return m_model;
}

void ViewModel::loadModel(const QString &path)
{
    m_model->setModelPath(path);
}

void ViewModel::loadVideo(const QString &path)
{
    m_model->setVideoPath(path);
}
