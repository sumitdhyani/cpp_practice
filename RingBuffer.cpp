#include <concepts>
#include <iostream>
#include <string>
#include <queue>

template <class T>
class RingBuffer
{
  enum class LastOperation
  {
    NONE,
    PUSH,
    POP
  };

  T* m_elementBuff;
  size_t m_head;
  size_t m_tail;
  LastOperation m_lastOperation;
  size_t m_capacity;

public:
  RingBuffer(const size_t &capacity) : m_capacity(capacity),
                                       m_elementBuff(reinterpret_cast<T*>(malloc(sizeof(T) * capacity))),
                                       m_head(0),
                                       m_tail(0),
                                       m_lastOperation(LastOperation::NONE)
  {}

  ~RingBuffer()
  {
    auto currSize = size();
    for (size_t i = 0; i < currSize; ++i)
    {
      m_elementBuff[(m_tail + i) % m_capacity].~T();
    }

    free(reinterpret_cast<void*>(m_elementBuff));
  }

  void push(const T &item)
  {
    if (full())
    {
      pop();
    }

    new (m_elementBuff + m_head) T(item);
    m_head = (m_head + 1) % m_capacity;
    m_lastOperation = LastOperation::PUSH;
  }

  void push(T&& item)
  {
    if (full())
    {
      pop();
    }

    new (m_elementBuff + m_head) T(item);
    m_head = (m_head + 1) % m_capacity;
    m_lastOperation = LastOperation::PUSH;
  }

  template <class... Args>
  void emplace(const Args &...args)
  {
    if (full())
      pop();

    new (m_elementBuff + m_head) T(args...);
    m_head = (m_head + 1) % m_capacity;
    m_lastOperation = LastOperation::PUSH;
  }

  bool pop()
  {
    if (empty())
      return false;

    front().~T();
    m_tail = (m_tail + 1) % m_capacity;
    m_lastOperation = LastOperation::POP;
    return true;
  }

  size_t size()
  {
    size_t ret = 0;
    if (m_head == m_tail)
    {
      ret = m_lastOperation == LastOperation::PUSH ? m_capacity : 0;
    }
    else if (m_head > m_tail)
    {
      ret = m_head - m_tail;
    }
    else
    {
      ret = m_capacity - (m_tail - m_head);
    }

    return ret;
  }

  bool empty()
  {
    return (size() == 0);
  }

  bool full()
  {
    return (size() == m_capacity);
  }

  T &front()
  {
    if (empty())
      throw std::runtime_error("Trying to access element when buffer is empty");

    return m_elementBuff[m_tail];
  }

  T &back()
  {
    if (empty())
      throw std::runtime_error("Trying to access element when buffer is empty");

    return m_elementBuff[m_tail + size() - 1];
  }

  const T &front() const
  {
    if (empty())
      throw std::runtime_error("Trying to access element when buffer is empty");

    return m_elementBuff[m_tail];
  }

  const T &back() const
  {
    if (empty())
      throw std::runtime_error("Trying to access element when buffer is empty");

    return m_elementBuff[m_tail + size() - 1];
  }
};

namespace ULCommonUtils
{
  template <class T>
  class RingBuffer
  {
    std::queue<T> m_queue;
    size_t m_capacity;

  public:
    RingBuffer(size_t capacity) : m_capacity(capacity)
    {}

    void push(T item)
    {
      if (full())
        pop();

      m_queue.push(item);
    }

    void pop()
    {
      if (empty())
        throw std::runtime_error("Trying to pop empty queue");

      m_queue.pop();
    }

    size_t size()
    {
      return m_queue.size();
    }

    size_t empty()
    {
      return (size() == 0);
    }

    bool full()
    {
      return (size() == m_capacity);
    }

    T front()
    {
      if (empty())
        throw std::runtime_error("Trying to access element when buffer is empty");

      return m_queue.front();
    }

    T back()
    {
      if (empty())
        throw std::runtime_error("Trying to access element when buffer is empty");

      return m_queue.back();
    }
  };
}

struct X
{
  X(int n)
  {
    rand();
    //std::cout << "X::PCtor" << std::endl;
  }

  ~X()
  {
    rand();
    //std::cout << "X::Dtor" << std::endl;
  }

  X(const X &other)
  {
    rand();
    //std::cout << "X::CCtor" << std::endl;
  }

  X(X &&other)
  {
    rand();
    //std::cout << "X::MCtor" << std::endl;
  }
  private:
  int arr[512];
};

#include <chrono>

int main(int argc, char** argv)
{
  if(argc > 1)
  {
    auto start = std::chrono::high_resolution_clock().now();
    {
      RingBuffer<X> buff(3);
      for (int i = 0; i < 1000; ++i)
      {
        buff.push(i);
      }
    }
    auto duration = std::chrono::high_resolution_clock().now() - start;
    std::string durationStr = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / (double)1000000000) + " s";
    std::cout << "Total Duration with HP RingBuffer: " << durationStr;
  }
  else
  {
    auto start = std::chrono::high_resolution_clock().now();
    {
      ULCommonUtils::RingBuffer<X> buff(3);
      for (int i = 0; i < 1000; ++i)
      {
        buff.push(i);
      }
    }
    auto duration = std::chrono::high_resolution_clock().now() - start;
    std::string durationStr = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / (double)1000000000) + " s";
    std::cout << "Total Duration with HP RingBuffer: " << durationStr;
  }

  return 0;
}