#ifndef ASYNCPIPELINE_H
#define ASYNCPIPELINE_H

#include <QObject>
#include <QThread>
#include <QFuture>
#include <QMutex>
#include <QWaitCondition>
#include <functional>
#include <memory>
#include <queue>
#include "../thread/ThreadPool.h"

class AsyncPipeline : public QObject
{
    Q_OBJECT
public:
    explicit AsyncPipeline(QObject *parent = nullptr);
    ~AsyncPipeline();

    // 定义流水线阶段的函数类型
    using DecodeFunction = std::function<void(void*)>;
    using ProcessFunction = std::function<void(void*)>;
    using RenderFunction = std::function<void(void*)>;

    // 设置流水线函数
    void setDecodeFunction(const DecodeFunction &func);
    void setProcessFunction(const ProcessFunction &func);
    void setRenderFunction(const RenderFunction &func);

    // 启动流水线
    void start();
    void stop();

    // 推送数据到流水线
    void pushData(void *data);

    // 设置线程池大小
    void setThreadPoolSize(size_t decodeThreads, size_t processThreads, size_t renderThreads);

private slots:
    void processDecode();
    void processProcess();
    void processRender();

private:
    // 线程池
    std::unique_ptr<ThreadPool> m_decodePool;
    std::unique_ptr<ThreadPool> m_processPool;
    std::unique_ptr<ThreadPool> m_renderPool;

    // 流水线函数
    DecodeFunction m_decodeFunc;
    ProcessFunction m_processFunc;
    RenderFunction m_renderFunc;

    // 数据队列
    std::queue<void*> m_decodeQueue;
    std::queue<void*> m_processQueue;
    std::queue<void*> m_renderQueue;

    // 同步原语
    QMutex m_decodeMutex;
    QMutex m_processMutex;
    QMutex m_renderMutex;
    QWaitCondition m_decodeCondition;
    QWaitCondition m_processCondition;
    QWaitCondition m_renderCondition;

    // 控制标志
    std::atomic<bool> m_running;

    // 处理线程
    QThread *m_decodeThread;
    QThread *m_processThread;
    QThread *m_renderThread;
};

#endif // ASYNCPIPELINE_H
