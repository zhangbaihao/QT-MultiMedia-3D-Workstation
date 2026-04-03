#include "Task.h"

Task::Task(FunctionType func)
    : m_func(func)
{}

void Task::execute()
{
    if (m_func)
    {
        m_func();
    }
}
