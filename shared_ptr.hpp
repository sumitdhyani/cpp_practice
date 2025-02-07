#include<stdint.h>
#include<mutex>
#include<cstdlib>


template<class T>
struct shared_ptr
{
  shared_ptr()
  {
    nullify();
  }

  shared_ptr(const nullptr_t& null)
  {
    shared_ptr();
  }

  shared_ptr(T* ptr)
  {
    if(ptr)
    {
      m_ptr = ptr;

      //Control block includes only the mutex and the reference counter
      m_controlBlock = new char[sizeof(*m_mutex) + sizeof(*m_refCount)];
      uint16_t offset = 0;

      m_mutex = new (reinterpret_cast<decltype(m_mutex)>(m_controlBlock+offset)) std::mutex();
      offset += sizeof(*m_mutex);

      m_refCount = new (reinterpret_cast<decltype(m_refCount)>(m_controlBlock+offset)) uint32_t(1);
    }
    else
    {
      shared_ptr();
    }
  }
  
  template <class... Args>
  shared_ptr(Args... args)
  {
    // Control block includes the shared object, mutex and the reference
    // counter, this is the preferred way to create the shared_ptr as this involves
    // only 1 memory alocation and since m_ptr is a part of the control block,
    // this object will enjoy spatial locality and more cache friendly
    try
    {
      m_controlBlock = reinterpret_cast<char*>(malloc(sizeof(T) + sizeof(*m_mutex) + sizeof(*m_refCount)));
      uint16_t offset = 0;
    
      m_ptr = new (reinterpret_cast<decltype(m_ptr)>(m_controlBlock+offset)) T(args...);
      offset += sizeof(*m_ptr);

      m_mutex = new (reinterpret_cast<decltype(m_mutex)>(m_controlBlock+offset)) std::mutex();
      offset += sizeof(*m_mutex);

      m_refCount = new (reinterpret_cast<decltype(m_refCount)>(m_controlBlock+offset)) uint32_t(1);
    }
    catch(const std::exception& e)
    {
      free(m_controlBlock);
      throw e;
    }
  }

  operator bool() const
  {
    return m_ptr != nullptr;
  }

  T* operator ->() const
  {
    return m_ptr;
  }

  T& operator *() const
  {
    return *m_ptr;
  }

  const shared_ptr& operator =(const shared_ptr& other)
  {
    if (this == &other)
    {
      return *this;
    }
    // 1st thing: decrease ref-count for the existing shared_resource and
    // nullify the control block, if its a non-null shared_ptr
    else if (*this)
    {
      decreaseRefCount();
      nullify();
    }

    // If 'other' is null then let this shared_ptr be null, that, we have
    // already done above
    // Otherwise now this shared_ptr should hold the resource held by 'other'
    // That's whats's happenning inside 'if' 
    if (other)
    {
      std::unique_lock<std::mutex> lock(*other.m_mutex);
      ++(*other.m_refCount);
      m_controlBlock = other.m_controlBlock;
      m_ptr = other.m_ptr;
      m_mutex = other.m_mutex;
      m_refCount = other.m_refCount;
    }

    return *this;
  }

  ~shared_ptr()
  {
    if (*this)
    {
      decreaseRefCount();
    }
    nullify();
  }

  private:

  void destroyControlBlock()
  {
    m_mutex->~mutex();
    // m_ptr may or may not be a part of the control block,
    // if yes, we just need to call the destructor
    if (m_ptr == reinterpret_cast<T*>(m_controlBlock))
    {
      m_ptr->~T();
    }
    else
    {
      delete m_ptr;
    }

    free(m_controlBlock);
  }

  void nullify()
  {
    m_ptr = nullptr;
    m_mutex = nullptr;
    m_refCount = nullptr;
    m_controlBlock = nullptr;
  }

  // Logically this method should only be called only from assignment operator
  // and destructor
  void decreaseRefCount()
  {
    std::unique_lock<std::mutex> lock(*m_mutex);
    if (--(*m_refCount) == 0)
    {
      lock.unlock();
      // Destroy the control block if this was the lasr reference
      destroyControlBlock();
    }
  }

  char* m_controlBlock;
  T* m_ptr;
  std::mutex* m_mutex;
  uint32_t* m_refCount;
};

#include <iostream>
#include <ostream>

struct X
{
    int _x;
    int _y;
    
    X(int x, int y) : _x(x), _y(y)
    {}
};

void operator <<(std::ostream& stream, const X& x)
{
    stream << x._x << ":" << x._y << std::endl;
}

int main()
{
  shared_ptr<X> sp1(new X(2,3));
  shared_ptr<X> sp2(new X(3,4));
  shared_ptr<X> sp3(4,5);
  std::cout << *sp1;// << std::endl;
  sp1 = sp2;
  std::cout << *sp1;// << std::endl;
  sp1 = sp3;
  std::cout << *sp1;// << std::endl;
}