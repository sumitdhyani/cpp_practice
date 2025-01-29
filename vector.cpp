#include <stdint.h>
#include <cstring>
#include <optional>
#include <functional>
#include <functional>

struct vector
{
  typedef std::optional<std::reference_wrapper<uint32_t>> optional_ref;
  typedef std::optional<std::reference_wrapper<const uint32_t>> optional_const_ref;

  vector() : m_size(0), m_capacity(0), m_arr(nullptr) {}

  void push_back(const uint32_t& val)
  {
    if(!m_capacity)
    {
      m_capacity = 1;
      m_arr = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t)));
    }
    else if (m_size == m_capacity)
    {
      m_capacity *= 2;
      uint32_t* tempBuff = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t) * m_capacity));
      memcpy(reinterpret_cast<void*>(tempBuff), reinterpret_cast<void*>(m_arr), sizeof(uint32_t)*m_size);
      delete[] m_arr;
      m_arr = tempBuff;
    }

    m_arr[m_size++] = val;
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
    if(!m_size)
    {
      return;
    }

    --m_size;
    //There is no element now, delete the buffer and nullify the internal book
    if (!m_size)
    {
      delete[] m_arr;
      m_arr = nullptr;
      m_size = 0;
      m_capacity = 0;
    }
    //Check if the no. of elements now is power of 2
    else
    {
      for (uint32_t power = 1, exp = 1 >> power;
           !(m_size & exp) && (power < sizeof(uint32_t) * 8);
           exp >> 1, power++)
      {
        if (m_size & exp)
        {
          m_capacity = m_size;
          uint32_t* tempBuff = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t) * m_capacity));
          memcpy(reinterpret_cast<void*>(tempBuff), reinterpret_cast<void*>(m_arr), sizeof(uint32_t)*m_size);
          delete[] m_arr;
          m_arr = tempBuff; 
          break;
        }
      }
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