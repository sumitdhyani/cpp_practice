#include <functional>

template <class T>
struct optional
{
  optional() : m_isNull(true)
  {}

  optional(const T& val) : m_isNull(false)
  {
    new (reinterpret_cast<T*>) T(val);
  }

  optional(const T&& val) : m_isNull(false)
  {
    new (reinterpret_cast<T*>) T(val);
  }

  const optional& operator=(const T& other)
  {
    if ()
  }

  template<class... Args>
  optional(const Args& args) : m_isNull(false)
  {
    new (reinterpret_cast<T*>) T(args...);
  }

  bool has_value()
  {
    return m_isNull;
  }

  T& value() const
  {
    if (has_value())
    {
      return *reinterpret_cast<T*>(m_buff);
    }
  }

  const T& value() const
  {
    if (has_value())
    {
      return *reinterpret_cast<T*>(m_buff);
    }
  }

  operator bool() const
  {
    return m_isNull;
  }

  private:
  char m_buff[sizeof(T)];
  bool m_isNull;
};