#include "CacheManager.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QCryptographicHash>

CacheManager* CacheManager::m_instance = nullptr;

CacheManager::CacheManager(QObject *parent)
    : QObject(parent)
{
    // 设置CPU缓存大小为100MB
    m_cpuCache.setMaxCost(100 * 1024 * 1024);
    initializeDiskCache();
}

CacheManager::~CacheManager()
{
    clearCPUCache();
    clearGPUCache();
    clearDiskCache();
}

CacheManager* CacheManager::instance()
{
    if (!m_instance)
    {
        m_instance = new CacheManager();
    }
    return m_instance;
}

void CacheManager::clearCPUCache()
{
    std::lock_guard<QMutex> lock(m_cpuCacheMutex);
    m_cpuCache.clear();
}

void CacheManager::addToGPUCache(const QString &key, void *buffer, size_t size)
{
    std::lock_guard<QMutex> lock(m_gpuCacheMutex);
    m_gpuCache[key] = std::make_pair(buffer, size);
}

void* CacheManager::getFromGPUCache(const QString &key)
{
    std::lock_guard<QMutex> lock(m_gpuCacheMutex);
    auto it = m_gpuCache.find(key);
    if (it != m_gpuCache.end())
    {
        return it->second.first;
    }
    return nullptr;
}

void CacheManager::clearGPUCache()
{
    std::lock_guard<QMutex> lock(m_gpuCacheMutex);
    // 这里应该释放GPU内存，但简化处理
    m_gpuCache.clear();
}

void CacheManager::addToDiskCache(const QString &key, const QByteArray &data)
{
    std::lock_guard<QMutex> lock(m_diskCacheMutex);
    QString filePath = getDiskCacheFilePath(key);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(data);
        file.close();
    }
}

QByteArray CacheManager::getFromDiskCache(const QString &key)
{
    std::lock_guard<QMutex> lock(m_diskCacheMutex);
    QString filePath = getDiskCacheFilePath(key);
    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly))
    {
        QByteArray data = file.readAll();
        file.close();
        return data;
    }
    return QByteArray();
}

void CacheManager::clearDiskCache()
{
    std::lock_guard<QMutex> lock(m_diskCacheMutex);
    QDir cacheDir(m_diskCachePath);
    if (cacheDir.exists())
    {
        QStringList files = cacheDir.entryList(QDir::Files);
        for (const QString &file : files)
        {
            cacheDir.remove(file);
        }
    }
}

void CacheManager::initializeDiskCache()
{
    m_diskCachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir cacheDir(m_diskCachePath);
    if (!cacheDir.exists())
    {
        cacheDir.mkpath(m_diskCachePath);
    }
}

QString CacheManager::getDiskCacheFilePath(const QString &key)
{
    QString hash = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5).toHex();
    return m_diskCachePath + "/" + hash;
}
