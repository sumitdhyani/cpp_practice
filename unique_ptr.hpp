template <class T>
struct unique_ptr
{
  const unique_ptr& operator=(const unique_ptr&) = delete;
  unique_ptr(const unique_ptr&) = delete;
  
  unique_ptr()
  {
    m_ptr = nullptr;
  }

  unique_ptr(const nullptr_t& null) : unique_ptr()
  {}

  unique_ptr(T* ptr) : m_ptr(ptr)
  {}

  const unique_ptr& operator=(unique_ptr&& other)
  {
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
  }

  template <class... Args>
  unique_ptr(const Args&... args)
  {
    m_ptr = new T(args...);
  }

  unique_ptr(unique_ptr&& other) : unique_ptr()
  {
    if(other)
    {
      m_ptr = other.m_ptr;
      other.m_ptr = nullptr;
    }
  }

  operator bool()
  {
    return m_ptr;
  }

  ~unique_ptr()
  {
    if (*this)
    {
      delete m_ptr;
    }
  }

  private:
  T* m_ptr;
};