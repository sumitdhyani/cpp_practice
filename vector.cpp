#include <stdint.h>
#include <cstring>
#include <functional>

template <class T>
struct vector
{

  vector() : m_size(0), m_capacity(0), m_arr(nullptr) {}

  void push_back(const T& val)
  {
    if(!m_capacity)
    {
      m_capacity = 1;
      m_arr = reinterpret_cast<T*>(malloc(sizeof(T)));
    }
    else if (m_size == m_capacity)
    {
      m_capacity << 2;
      T* tempBuff = reinterpret_cast<T*>(malloc(sizeof(T) * m_capacity));
      for (uint32_t i = 0 ; i < m_size; i++)
      {
        // Copy construction using placement new
        new (tempBuff + i) T(m_arr[i]);

        // Manually call the destructor as we didn't use new for performance reasons
        m_arr[i].~T();
      }

      free(m_arr);
      m_arr = tempBuff;
    }

    ++m_size;
    new (m_arr + m_size - 1) T(val);
  }

  uint32_t size()
  {
    return m_size;
  }

  uint32_t capacity()
  {
    return m_capacity;
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
      m_capacity = m_size;
      T* tempBuff = reinterpret_cast<T*>(malloc(sizeof(T) * m_capacity));
      for (uint32_t i = 0; i < m_size; i++)
      {
        new (tempBuff + i) T(m_arr[i]); 
      }
      free(m_arr);
      m_arr = tempBuff; 
    }
  }
private:
  uint32_t  m_size;
  uint32_t  m_capacity;
  T* m_arr;
};

#include <iostream>

struct X
{
  X(uint32_t x) {
    _x = x;
    std::cout << "param X::ctor" << std::endl;
  }

  X() {
    _x = 0;
    std::cout << "default X::ctor" << std::endl;
  }

  X(const X& x) {
    _x = x._x;
    std::cout << "copy X::ctor" << std::endl;
  }

  X(X&& x) {
    _x = x._x;
    x._x = 0;
    std::cout << "move X::ctor" << std::endl;
  }

  uint32_t _x;
};

void operator <<(std::ostream& stream, const X& x)
{
  stream << x._x << std::endl;
}

int main()
{
  vector<X> v;
  for(uint32_t i = 0; i < 100; i++)
  {
    v.push_back(X(i));
  } 

  for (uint32_t i = 0; i < v.size(); i++)
  {
    std::cout << v[i];
  }

  std::cout << "Deleting!" << std::endl;

  uint32_t size = v.size();
  for(uint32_t i = 0; i < size; i++)
  {
    v.pop_back();
  }

  for (uint32_t i = 0; i < v.size(); i++)
  {
    std::cout << v[i];
  }
  
  std::cout << "End!" << std::endl;
  return 0;
}