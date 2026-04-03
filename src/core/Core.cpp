#include "Core.h"

Core* Core::m_instance = nullptr;

Core::Core(QObject *parent) : QObject(parent)
{
    m_instance = this;
}

Core::~Core()
{
    m_instance = nullptr;
}

Core* Core::instance()
{
    if (!m_instance)
    {
        m_instance = new Core();
    }
    return m_instance;
}
