#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Task.h"

class ThreadPool
{
public:
    ThreadPool(size_t threadCount = 4);
    ~ThreadPool();

    void enqueue(Task::FunctionType func);
    void waitForCompletion();
    size_t getThreadCount() const;

private:
    void workerThread();

    std::vector<std::thread> m_threads;
    std::queue<Task> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stop;
    std::atomic<size_t> m_activeTasks;
};

#endif // THREADPOOL_H
