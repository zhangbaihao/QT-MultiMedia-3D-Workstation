#include <QTest>
#include <QCoreApplication>
#include "../src/core/CacheManager.h"
#include "../src/core/AsyncPipeline.h"

class TestCore : public QObject
{
    Q_OBJECT

private slots:
    void testCacheManager();
    void testAsyncPipeline();
};

void TestCore::testCacheManager()
{
    // 测试CPU缓存
    CacheManager* cacheManager = CacheManager::instance();
    
    // 添加数据到CPU缓存
    int* testData = new int(42);
    cacheManager->addToCPUCache("test_key", testData);
    
    // 从CPU缓存获取数据
    int* retrievedData = cacheManager->getFromCPUCache<int>("test_key");
    QVERIFY(retrievedData != nullptr);
    QCOMPARE(*retrievedData, 42);
    
    // 清除CPU缓存
    cacheManager->clearCPUCache();
    retrievedData = cacheManager->getFromCPUCache<int>("test_key");
    QVERIFY(retrievedData == nullptr);
    
    delete testData;
}

void TestCore::testAsyncPipeline()
{
    AsyncPipeline pipeline;
    
    // 测试标志
    bool decodeCalled = false;
    bool processCalled = false;
    bool renderCalled = false;
    
    // 设置流水线函数
    pipeline.setDecodeFunction([&](void* data) {
        decodeCalled = true;
    });
    
    pipeline.setProcessFunction([&](void* data) {
        processCalled = true;
    });
    
    pipeline.setRenderFunction([&](void* data) {
        renderCalled = true;
    });
    
    // 启动流水线
    pipeline.start();
    
    // 推送数据
    int testData = 42;
    pipeline.pushData(&testData);
    
    // 等待一段时间
    QThread::msleep(100);
    
    // 停止流水线
    pipeline.stop();
    
    // 验证函数是否被调用
    QVERIFY(decodeCalled);
    QVERIFY(processCalled);
    QVERIFY(renderCalled);
}

QTEST_MAIN(TestCore)
#include "test_core.moc"
