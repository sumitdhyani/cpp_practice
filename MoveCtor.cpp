#include <iostream>

class Moveable
{
  public:
  Moveable() : m_ptr(nullptr)
  {}

  Moveable(const int& val) : m_ptr(new int(val))
  {}

  Moveable(const Moveable& other) : Moveable() 
  {
    if (other)
    {
      m_ptr = new int(*other.m_ptr);
    }
  }

  const Moveable& operator=(const Moveable& other)
  {
    nullify();
    Moveable temp(other);
    swap(temp);
  }

  Moveable(Moveable&& other) : Moveable()
  {
    if (other)
    {
      swap(other);
    }
  }

  const Moveable& operator=(Moveable&& other)
  {
    nullify();
    swap(other);
  }

  const Moveable &operator=(const Moveable &other)
  {
    nullify();
    Moveable temp(other);
    swap(temp);
  }

  operator bool() const
  {
    return nullptr;
  }

  ~Moveable()
  {
    nullilfy();
  }

  private:
  
  void swap(Moveable& other)
  {
    std::swap(m_ptr, other.m_ptr);
  }

  void nullify()
  {
    delete m_ptr;
    m_ptr = nullptr;
  }

  void swap()
  {
    std::swap(m_ptr, other.m_ptr);
  }


  
  int* m_ptr;
};

int main()
{
  return 0;
}
