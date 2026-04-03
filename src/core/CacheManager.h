#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>
#include <QCache>
#include <QMap>
#include <QString>
#include <QMutex>
#include <memory>

class CacheManager : public QObject
{
    Q_OBJECT
public:
    explicit CacheManager(QObject *parent = nullptr);
    ~CacheManager();

    static CacheManager* instance();

    // CPU缓存管理
    template<typename T>
    void addToCPUCache(const QString &key, T *data, int cost = 1);

    template<typename T>
    T* getFromCPUCache(const QString &key);

    void clearCPUCache();

    // GPU显存管理
    void addToGPUCache(const QString &key, void *buffer, size_t size);
    void* getFromGPUCache(const QString &key);
    void clearGPUCache();

    // 磁盘缓存管理
    void addToDiskCache(const QString &key, const QByteArray &data);
    QByteArray getFromDiskCache(const QString &key);
    void clearDiskCache();

private:
    static CacheManager* m_instance;

    // CPU缓存
    QCache<QString, void> m_cpuCache;
    QMutex m_cpuCacheMutex;

    // GPU缓存
    QMap<QString, std::pair<void*, size_t>> m_gpuCache;
    QMutex m_gpuCacheMutex;

    // 磁盘缓存
    QString m_diskCachePath;
    QMutex m_diskCacheMutex;

    void initializeDiskCache();
    QString getDiskCacheFilePath(const QString &key);
};

// 模板方法实现
template<typename T>
void CacheManager::addToCPUCache(const QString &key, T *data, int cost)
{
    std::lock_guard<QMutex> lock(m_cpuCacheMutex);
    m_cpuCache.insert(key, static_cast<void*>(data), cost);
}

template<typename T>
T* CacheManager::getFromCPUCache(const QString &key)
{
    std::lock_guard<QMutex> lock(m_cpuCacheMutex);
    return static_cast<T*>(m_cpuCache.object(key));
}

#endif // CACHEMANAGER_H
