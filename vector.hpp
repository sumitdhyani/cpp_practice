#include <stdint.h>
#include <cstring>
#include <iostream>

struct vector
{
  vector() : m_size(0), m_capacity(0) {}

  void push_back(const uint32_t& val)
  {
    std::cout<< "push_back " << val << std::endl;
    if(!m_capacity)
    {
      m_capacity = 1;
      m_arr = new uint32_t[1];
    }
    else if (m_size == m_capacity)
    {
      std::cout<< "resizing from " << m_capacity << " to " << m_capacity*2 << std::endl;
      m_capacity *= 2;
      uint32_t* tempBuff = new uint32_t[m_capacity];
      memcpy(reinterpret_cast<void*>(tempBuff), reinterpret_cast<void*>(m_arr), sizeof(uint32_t)*m_capacity);
      delete[] m_arr;
      m_arr = tempBuff;
    }

    ++m_size;
    m_arr[m_size-1] = val;
  }

  uint32_t size()
  {
    return m_size;
  }

  uint32_t capacity()
  {
    return m_capacity;
  }

  uint32_t& operator[](const uint32_t idx)
  {
    return m_arr[idx];
  }

  const uint32_t& operator[](const uint32_t idx) const
  {
    return m_arr[idx];
  }

private:
  uint32_t  m_size;
  uint32_t  m_capacity;
  uint32_t* m_arr;

};


int main()
{
  vector v;
  for(uint32_t i = 0; i < 100; i++)
  {
    v.push_back(i);
  }

  for (uint32_t i = 0; i < v.size(); i++)
  {
    std::cout << v[i] << std::endl;
  }
  
  std::cout << "End!" << std::endl;
  return 0;
}