#include <stdint.h>
#include <cstring>
#include <optional>
#include <functional>

struct vector
{
  typedef std::optional<std::reference_wrapper<uint32_t>> optional_ref;
  typedef std::optional<std::reference_wrapper<const uint32_t>> optional_const_ref;

  vector() : m_size(0), m_capacity(0) {}

  void push_back(const uint32_t& val)
  {
    if(!m_capacity)
    {
      m_capacity = 1;
      m_arr = new uint32_t[1];
    }
    else if (m_size == m_capacity)
    {
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

  optional_ref operator[](const uint32_t idx)
  {
    if (idx >= m_size)
    {
      return std::nullopt;
    }

    return std::ref(m_arr[idx]);
  }

  optional_const_ref operator[](const uint32_t idx) const
  {
    if (idx >= m_size)
    {
      return std::nullopt;
    }

    return std::cref(m_arr[idx]);
  }

  void pop_back()
  {
    if (m_size)
    {
      --m_size;
    }
  }

private:
  uint32_t  m_size;
  uint32_t  m_capacity;
  uint32_t* m_arr;

};

#include <iostream>

int main()
{
  vector v;
  for(uint32_t i = 0; i < 100; i++)
  {
    v.push_back(i);
  }

  for (uint32_t i = 0; i < v.size(); i++)
  {
    if (v[i])
    {
      std::cout << v[i].value() << std::endl;
    }
  }

  std::cout << "Deleting!" << std::endl;

  uint32_t size = v.size();
  for(uint32_t i = 0; i < size; i++)
  {
    v.pop_back();
  }

  for (uint32_t i = 0; i < v.size(); i++)
  {
    if (v[i])
    {
      std::cout << v[i].value() << std::endl;
    }
  }
  
  std::cout << "End!" << std::endl;
  return 0;
}