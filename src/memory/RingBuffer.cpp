#include "RingBuffer.h"
#include <cstring>

RingBuffer::RingBuffer(size_t capacity, size_t elementSize)
    : m_capacity(capacity)
    , m_elementSize(elementSize)
    , m_buffer(capacity * elementSize)
    , m_head(0)
    , m_tail(0)
    , m_size(0)
{}

RingBuffer::~RingBuffer()
{}

bool RingBuffer::push(const void* data)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (isFull())
    {
        return false;
    }
    
    size_t offset = m_tail * m_elementSize;
    memcpy(&m_buffer[offset], data, m_elementSize);
    
    m_tail = (m_tail + 1) % m_capacity;
    m_size++;
    
    return true;
}

bool RingBuffer::pop(void* data)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (isEmpty())
    {
        return false;
    }
    
    size_t offset = m_head * m_elementSize;
    memcpy(data, &m_buffer[offset], m_elementSize);
    
    m_head = (m_head + 1) % m_capacity;
    m_size--;
    
    return true;
}

bool RingBuffer::isFull() const
{
    return m_size == m_capacity;
}

bool RingBuffer::isEmpty() const
{
    return m_size == 0;
}

size_t RingBuffer::getCapacity() const
{
    return m_capacity;
}

size_t RingBuffer::getSize() const
{
    return m_size;
}
