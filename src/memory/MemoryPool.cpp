#include "MemoryPool.h"

MemoryPool::MemoryPool(size_t blockSize, size_t blockCount)
    : m_blockSize(blockSize)
    , m_blockCount(blockCount)
    , m_memory(blockSize * blockCount)
    , m_freeBlocks(blockCount, true)
    , m_freeCount(blockCount)
{}

MemoryPool::~MemoryPool()
{}

void* MemoryPool::allocate()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (size_t i = 0; i < m_blockCount; ++i)
    {
        if (m_freeBlocks[i])
        {
            m_freeBlocks[i] = false;
            m_freeCount--;
            return &m_memory[i * m_blockSize];
        }
    }
    
    return nullptr;
}

void MemoryPool::deallocate(void* ptr)
{
    if (!ptr)
        return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t offset = static_cast<char*>(ptr) - m_memory.data();
    size_t index = offset / m_blockSize;
    
    if (index < m_blockCount)
    {
        m_freeBlocks[index] = true;
        m_freeCount++;
    }
}

size_t MemoryPool::getBlockSize() const
{
    return m_blockSize;
}

size_t MemoryPool::getBlockCount() const
{
    return m_blockCount;
}

size_t MemoryPool::getFreeBlocks() const
{
    return m_freeCount;
}
