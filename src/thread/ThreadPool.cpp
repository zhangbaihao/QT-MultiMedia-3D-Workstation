#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadCount)
    : m_stop(false)
    , m_activeTasks(0)
{
    for (size_t i = 0; i < threadCount; ++i)
    {
        m_threads.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool()
{
    m_stop = true;
    m_condition.notify_all();
    
    for (auto &thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void ThreadPool::enqueue(Task::FunctionType func)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.emplace(func);
    }
    m_condition.notify_one();
}

void ThreadPool::waitForCompletion()
{
    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_tasks.empty() && m_activeTasks == 0)
            {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

size_t ThreadPool::getThreadCount() const
{
    return m_threads.size();
}

void ThreadPool::workerThread()
{
    while (!m_stop)
    {
        Task task;
        bool hasTask = false;
        
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait(lock, [this] { return m_stop || !m_tasks.empty(); });
            
            if (m_stop && m_tasks.empty())
            {
                break;
            }
            
            if (!m_tasks.empty())
            {
                task = std::move(m_tasks.front());
                m_tasks.pop();
                hasTask = true;
            }
        }
        
        if (hasTask)
        {
            m_activeTasks++;
            task.execute();
            m_activeTasks--;
        }
    }
}
