#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <vector>
#include <mutex>
#include <atomic>

class MemoryPool
{
public:
    MemoryPool(size_t blockSize, size_t blockCount);
    ~MemoryPool();

    void* allocate();
    void deallocate(void* ptr);
    size_t getBlockSize() const;
    size_t getBlockCount() const;
    size_t getFreeBlocks() const;

private:
    size_t m_blockSize;
    size_t m_blockCount;
    std::vector<char> m_memory;
    std::vector<bool> m_freeBlocks;
    std::mutex m_mutex;
    std::atomic<size_t> m_freeCount;
};

#endif // MEMORYPOOL_H
