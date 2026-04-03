#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <vector>
#include <mutex>
#include <atomic>

class RingBuffer
{
public:
    RingBuffer(size_t capacity, size_t elementSize);
    ~RingBuffer();

    bool push(const void* data);
    bool pop(void* data);
    bool isFull() const;
    bool isEmpty() const;
    size_t getCapacity() const;
    size_t getSize() const;

private:
    size_t m_capacity;
    size_t m_elementSize;
    std::vector<char> m_buffer;
    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;
    std::atomic<size_t> m_size;
    std::mutex m_mutex;
};

#endif // RINGBUFFER_H
