#include "AsyncPipeline.h"

AsyncPipeline::AsyncPipeline(QObject *parent)
    : QObject(parent)
    , m_running(false)
{
    // 初始化线程池，默认线程数
    m_decodePool = std::make_unique<ThreadPool>(4);
    m_processPool = std::make_unique<ThreadPool>(4);
    m_renderPool = std::make_unique<ThreadPool>(2);

    // 初始化处理线程
    m_decodeThread = new QThread();
    m_processThread = new QThread();
    m_renderThread = new QThread();

    // 移动到线程
    moveToThread(m_decodeThread);

    // 连接信号槽
    connect(m_decodeThread, &QThread::started, this, &AsyncPipeline::processDecode);
    connect(m_processThread, &QThread::started, this, &AsyncPipeline::processProcess);
    connect(m_renderThread, &QThread::started, this, &AsyncPipeline::processRender);

    connect(m_decodeThread, &QThread::finished, m_decodeThread, &QThread::deleteLater);
    connect(m_processThread, &QThread::finished, m_processThread, &QThread::deleteLater);
    connect(m_renderThread, &QThread::finished, m_renderThread, &QThread::deleteLater);
}

AsyncPipeline::~AsyncPipeline()
{
    stop();
}

void AsyncPipeline::setDecodeFunction(const DecodeFunction &func)
{
    m_decodeFunc = func;
}

void AsyncPipeline::setProcessFunction(const ProcessFunction &func)
{
    m_processFunc = func;
}

void AsyncPipeline::setRenderFunction(const RenderFunction &func)
{
    m_renderFunc = func;
}

void AsyncPipeline::start()
{
    if (!m_running)
    {
        m_running = true;
        m_decodeThread->start();
        m_processThread->start();
        m_renderThread->start();
    }
}

void AsyncPipeline::stop()
{
    if (m_running)
    {
        m_running = false;
        m_decodeCondition.wakeAll();
        m_processCondition.wakeAll();
        m_renderCondition.wakeAll();

        m_decodeThread->wait();
        m_processThread->wait();
        m_renderThread->wait();
    }
}

void AsyncPipeline::pushData(void *data)
{
    std::lock_guard<QMutex> lock(m_decodeMutex);
    m_decodeQueue.push(data);
    m_decodeCondition.wakeOne();
}

void AsyncPipeline::setThreadPoolSize(size_t decodeThreads, size_t processThreads, size_t renderThreads)
{
    m_decodePool = std::make_unique<ThreadPool>(decodeThreads);
    m_processPool = std::make_unique<ThreadPool>(processThreads);
    m_renderPool = std::make_unique<ThreadPool>(renderThreads);
}

void AsyncPipeline::processDecode()
{
    while (m_running)
    {
        void *data = nullptr;
        {
            std::unique_lock<QMutex> lock(m_decodeMutex);
            m_decodeCondition.wait(lock, [this] { return !m_running || !m_decodeQueue.empty(); });
            if (!m_running) break;
            if (!m_decodeQueue.empty())
            {
                data = m_decodeQueue.front();
                m_decodeQueue.pop();
            }
        }

        if (data && m_decodeFunc)
        {
            m_decodePool->enqueue([this, data]() {
                m_decodeFunc(data);
                
                // 传递到处理阶段
                std::lock_guard<QMutex> lock(m_processMutex);
                m_processQueue.push(data);
                m_processCondition.wakeOne();
            });
        }
    }
}

void AsyncPipeline::processProcess()
{
    while (m_running)
    {
        void *data = nullptr;
        {
            std::unique_lock<QMutex> lock(m_processMutex);
            m_processCondition.wait(lock, [this] { return !m_running || !m_processQueue.empty(); });
            if (!m_running) break;
            if (!m_processQueue.empty())
            {
                data = m_processQueue.front();
                m_processQueue.pop();
            }
        }

        if (data && m_processFunc)
        {
            m_processPool->enqueue([this, data]() {
                m_processFunc(data);
                
                // 传递到渲染阶段
                std::lock_guard<QMutex> lock(m_renderMutex);
                m_renderQueue.push(data);
                m_renderCondition.wakeOne();
            });
        }
    }
}

void AsyncPipeline::processRender()
{
    while (m_running)
    {
        void *data = nullptr;
        {
            std::unique_lock<QMutex> lock(m_renderMutex);
            m_renderCondition.wait(lock, [this] { return !m_running || !m_renderQueue.empty(); });
            if (!m_running) break;
            if (!m_renderQueue.empty())
            {
                data = m_renderQueue.front();
                m_renderQueue.pop();
            }
        }

        if (data && m_renderFunc)
        {
            m_renderPool->enqueue([this, data]() {
                m_renderFunc(data);
            });
        }
    }
}
