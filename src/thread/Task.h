#ifndef TASK_H
#define TASK_H

#include <functional>
#include <future>

class Task
{
public:
    using FunctionType = std::function<void()>;
    
    Task() = default;
    Task(FunctionType func);
    ~Task() = default;
    
    void execute();
    
private:
    FunctionType m_func;
};

#endif // TASK_H
