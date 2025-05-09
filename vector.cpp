#include <stdint.h>
#include <cstring>
#include <functional>
#include <concepts>

template <class T>
struct vector
{
  struct iterator
  {
    friend class vector;
    iterator& operator=(const iterator& other)
    {
      m_idx = other.m_idx;
    }

    iterator& operator++()
    {
      if (m_idx == (uint32_t)-1 || ++m_idx == m_thisVector->size())
      {
        m_idx = (uint32_t)-1;
      }

      return *this;
    }

    T* operator->()
    {
      return &(*m_thisVector)[m_idx];
    }

    T& operator*()
    {
      return (*m_thisVector)[m_idx];
    }

    iterator(const iterator& other) = delete;
    iterator(iterator&& other) = delete;

    private:

    iterator(vector* thisVector, const uint32_t idx) :
      m_thisVector(thisVector),
      m_idx(idx)
    {}

    uint32_t m_idx;
    vector* m_thisVector;
  };

  struct const_iterator
  {
    friend class vector;
    const const_iterator& operator=(const const_iterator& other)
    {
      m_idx = other.m_idx;
    }

    bool operator==(const const_iterator& other)
    {
      return m_idx == other.m_idx;
    }

    bool operator!=(const const_iterator& other)
    {
      return m_idx != other.m_idx;
    }

    const_iterator& operator++()
    {
      if (m_idx == (uint32_t)-1 || ++m_idx == m_thisVector->size())
      {
        m_idx = (uint32_t)-1;
      }

      return *this;
    }

    const T* operator->()
    {
      return &(*m_thisVector)[m_idx];
    }

    const T& operator*()
    {
      return (*m_thisVector)[m_idx];
    }

    private:

    const_iterator(const vector* thisVector, const uint32_t idx) :
      m_thisVector(thisVector),
      m_idx(idx)
    {}

    uint32_t m_idx;
    const vector* m_thisVector;
  };

  iterator begin()
  {
    static iterator end(this, (uint32_t)-1);
    if (!m_size)
    {
      return end;
    }

    return iterator(this, 0);
  }

  iterator end()
  {
    static iterator end(this, (uint32_t)-1);
    return end;
  }

  const_iterator begin() const
  {
    static const_iterator end(this, (uint32_t)-1);
    if (!size())
    {
      return end;
    }

    return const_iterator(this, 0);
  }

  const const_iterator end() const
  {
    static const_iterator end(this, (uint32_t)-1);
    return end;
  }

  vector() : m_size(0), m_capacity(0), m_arr(nullptr) {}

  void reserve(uint32_t capacity)
  {
    if(capacity <= m_capacity)
    {
      return;
    }

    uint32_t shift = 0;    
    while(1 << shift < capacity)
    {
      ++shift;
    }

    relocate(1 << shift);
  }

  void clear()
  {
    cleanup_deallocate_nullify();
  }

  vector(vector&& other) : vector()
  {
    if (!other.m_capacity)
    {
      return;
    }
    
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    m_arr = other.m_arr;
    
    other.m_capacity = 0;
    other.m_size = 0;
    other.m_arr = nullptr;
  }

  vector(const vector& other) : vector()
  {
    if (!other.m_capacity)
    {
      return;
    }

    m_capacity = other.m_capacity;
    m_size = other.m_size;
    m_arr = reinterpret_cast<T*>(malloc(sizeof(T) * m_capacity));

    if constexpr (std::is_fundamental_v<T>)
    {
      memcpy(m_arr, other.m_arr, sizeof(T) * m_size);
    }
    else
    {
      for (uint32_t i = 0; i < m_size; i++)
      {
        // Copy construction using placement new
        new (m_arr + i) T(other[i]);
      }
    }
  }

  const vector& operator=(const vector& other)
  {
    if (!other.size())
    {
      cleanup_deallocate_nullify();
    }
    else
    {
      if (m_capacity >= other.m_capacity)
      {
        cleanup();
      }
      else
      {
        cleanup_deallocate_nullify();
        m_arr = reinterpret_cast<T*>(malloc(sizeof(T)*other.m_capacity));
        m_capacity = other.m_capacity;
      }

      m_size = other.m_size;

      if constexpr (std::is_fundamental_v<T>)
      {
        memcpy(m_arr, other.m_arr, sizeof(T) * m_size);
      }
      else
      {
        for (uint32_t i = 0; i < m_size; i++)
        {
          // Copy construction using placement new
          new (m_arr + i) T(other[i]);
        }
      }
    }

    return *this;
  }


  const vector& operator=(const vector&& other)
  {
    cleanup_deallocate_nullify();
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    m_arr = other.m_arr;
    
    other.m_capacity = 0;
    other.m_size = 0;
    other.m_arr = nullptr;

    return *this;
  }

  void push_back(const T& val)
  {
    if(!m_capacity)
    {
      m_capacity = 1;
      m_arr = reinterpret_cast<T*>(malloc(sizeof(T)));
    }
    else if (m_size == m_capacity)
    {
      relocate(m_capacity*2);
    }

    ++m_size;
    new (m_arr + m_size - 1) T(val);
  }

  template <class... Args>
  void emplace_back(const Args&... args)
  {
    if(!m_capacity)
    {
      m_capacity = 1;
      m_arr = reinterpret_cast<T*>(malloc(sizeof(T)));
    }
    else if (m_size == m_capacity)
    {
      relocate(m_capacity * 2);
    }

    ++m_size;
    new (m_arr + m_size - 1) T(args...);
  }

  uint32_t size() const
  {
    return m_size;
  }

  uint32_t capacity() const
  {
    return m_capacity;
  }

  bool empty()
  {
    return m_size != 0;
  }

  T& operator[](const uint32_t idx)
  {
    return m_arr[idx];
  }

  const T& operator[](const uint32_t idx) const
  {
    return m_arr[idx];
  }

  void pop_back()
  {
    if(!m_size)
    {
      return;
    }

    m_arr[m_size-1].~T();
    --m_size;
    //There is no element now, delete the buffer and nullify the internal book
    if (!m_size)
    {
      free(m_arr);
      m_arr = nullptr;
      m_size = 0;
      m_capacity = 0;
    }
    //Check if the no. of elements now is power of 2
    else if (m_size == m_capacity >> 1)
    {
      relocate(m_size);
    }
  }

  ~vector()
  {
    clear();
  }

private:

  void relocate(const uint32_t& newSize)
  {
    m_capacity = newSize;
    T *tempBuff = reinterpret_cast<T*>(malloc(sizeof(T) * m_capacity));

    if constexpr (std::is_fundamental_v<T>)
    {
      memcpy(tempBuff, m_arr, sizeof(T) * m_size);
    }
    else if constexpr (std::is_nothrow_move_constructible_v<T>)
    {
      for (uint32_t i = 0; i < m_size; i++)
      {
        // Move construction using placement new
        new (tempBuff + i) T(std::move(m_arr[i]));
      }
    }
    else
    {
      for (uint32_t i = 0; i < m_size; i++)
      {
        // Copy construction using placement new
        new (tempBuff + i) T(m_arr[i]);
      }
    }

    cleanup();
    free(m_arr);
    m_arr = tempBuff;
  }

  void cleanup()
  {
    if constexpr (std::is_fundamental_v<T>) return;

    for (uint32_t i = 0; i < m_size; i++)
    {
      m_arr[i].~T();
    }
  }
  
  //Only to be called from destructor and assignment operators
  void cleanup_deallocate_nullify()
  {
    if (!m_capacity)
    {
      return;
    }

    //Cleanup
    cleanup();

    // Deallocate
    free(m_arr);

    // Nullify
    m_arr = nullptr;
    m_size = 0;
    m_capacity = 0;

  }
  uint32_t  m_size;
  uint32_t  m_capacity;
  T* m_arr;
};

#include <iostream>

int pc = 0;
int dc = 0;
int cc = 0;
int mc = 0;
int ao = 0;
int mao = 0;
int dd = 0;

struct X
{
    X(uint32_t x)
    {

      _x = x;
      ++pc;
      //std::cout << "param X::ctor " << _x << std::endl;
    }

    X()
    {

      _x = 0;
      ++dc;
      //std::cout << "default X::ctor" << std::endl;
    }

    X(const X& x)
    {
      _x = x._x;
      ++cc;
      //std::cout << "copy X::ctor " << _x << std::endl;
    }

    X(X&& x) noexcept
    {
      _x = x._x;
      //x._x = 0;
      ++mc;
      //std::cout << "move X::ctor " << _x << std::endl;
    }

    const X& operator=(const X& x)
    {
      _x = x._x;
      ++ao;
      return *this;
      //std::cout << "move X::ctor " << _x << std::endl;
    }

    const X& operator=(X&& x)
    {
      _x = x._x;
      ++mao;
      return *this;
      //std::cout << "move X::ctor " << _x << std::endl;
    }

    ~X()
    {
      ++dd;
      //std::cout << "X::dtor " << _x << std::endl;
    }
    uint32_t _x;
    uint32_t arr[1024];
  };

std::ostream& operator <<(std::ostream& stream, const X& x)
{
  stream << x._x;
  return stream;
}

template <class T>
void printVector(const T& list)
{
  for (auto it = list.begin(); it != list.end(); ++it)
  {
    std::cout << *it << std::endl;
  }
}

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#define NUM_ELEMENTS 1000000

#define VecType X
int main(int argc, char** argv)
{
  if (argc > 1)
  {
    auto start_time = Clock::now();
    vector<VecType> vec;
    //vec.reserve(NUM_ELEMENTS);
    
    for(uint32_t i = 0; i < NUM_ELEMENTS; i++)
    {
      //std::cout << "Inserting " << i << std::endl;
      //std::cout << "Pushing " << i << "th element to the vector" << std::endl;
      vec.emplace_back(i);
    }
    
    auto end_time = Clock::now();

    std::cout << "For objects of size " << sizeof(VecType) << ", in a custom vector,   cost to insert " << NUM_ELEMENTS << " elements:    " << std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() << " nanoseconds" << std::endl;
    //printVector(vec);
  }
  else
  {

    auto start_time = Clock::now();
    std::vector<VecType> vec;
    //vec.reserve(NUM_ELEMENTS);
    
    for(uint32_t i = 0; i < NUM_ELEMENTS; i++)
    {
      //std::cout << "Pushing " << i << "th element to the vector" << std::endl;
      vec.emplace_back(i);
    }

    auto end_time = Clock::now();

    std::cout << "For objects of size " << sizeof(VecType) << ", in a standard vector, cost to insert " << NUM_ELEMENTS << " elements:    " << std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() << " nanoseconds" << std::endl;
  }

  return 0;
}
