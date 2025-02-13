#include<stdint.h>

template <class T>
struct list
{
  struct Node{
    Node(const T& val) : m_val(val), m_next(nullptr)
    {}

    template<class... Args>
    Node(const Args&... args) : m_val(args...), m_next(nullptr)
    {}

    Node(Node* next, const T& val) : m_val(val), m_next(next)
    {}

    template<class... Args>
    Node(Node* next, const Args&... args) : m_val(args...), m_next(next)
    {}

    T m_val;
    Node* m_next;
  };

  struct iterator
  {
    iterator(Node* node) : m_node(node)
    {}

    iterator operator ++()
    {
      return m_node = m_node->m_next;
    }

    T* operator->()
    {
      return &m_node->m_val;
    }

    T& operator*()
    {
      return m_node->m_val;
    }

    bool operator!=(const iterator& other)
    {
      return m_node != other.m_node;
    }

    private:
    Node* m_node;
  };

  struct const_iterator
  {
    const_iterator(Node* node) : m_node(node)
    {}

    const_iterator operator ++()
    {
      return m_node = m_node->m_next;
    }

    const T* operator->()
    {
      return &m_node->m_val;
    }

    const T& operator*()
    {
      return m_node->m_val;
    }

    bool operator!=(const const_iterator& other)
    {
      return m_node != other.m_node;
    }

    private:
    Node* m_node;
  };

  list() : m_start(nullptr), m_end(nullptr), m_size(0)
  {}

  

  iterator begin()
  {
    if (m_size)
    {
      return iterator(m_start);
    }

    return end();
  }

  iterator end()
  {
    static iterator end(nullptr);
    return end;
  }

  const_iterator begin() const
  {
    if (m_size)
    {
      return const_iterator(m_start);
    }

    return end();
  }

  const_iterator end() const
  {
    static const_iterator end(nullptr);
    return end;
  }

  void push_back(const T& val)
  {
    if (m_start)
    {
      m_end->m_next = new Node(val);
      m_end = m_end->m_next;
    }
    else
    {
      m_start = m_end = new Node(val);
    }

    ++m_size;
  }

  void push_front(const T& val)
  {
    if (m_start)
    {
      Node* newStart = new Node(m_start, val);
      m_start = newStart;
    }
    else
    {
      m_start = m_end = new Node(val);
    }

    ++m_size;
  }

  template <class... Args>
  void emplace_back(const Args&... args)
  {
    if (m_start)
    {
      m_end->m_next = new Node(args...);
      m_end = m_end->m_next;
    }
    else
    {
      m_start = m_end = new Node(args...);
    }

    ++m_size;
  }

  template <class... Args>
  void emplace_front(const Args&... args)
  {
    if (m_start)
    {
      Node* newStart = new Node(m_start, args...);
      m_start = newStart;
    }
    else
    {
      m_start = m_end = new Node(args...);
    }

    ++m_size;
  }

  ~list()
  {
    cleanupTillEnd(m_start);
  }

  private:

  void cleanupTillEnd(Node* start)
  {
    if (start)
    {
      cleanupTillEnd(start->m_next);
      delete start;
    }
  }
  
  Node* m_start;
  Node* m_end;
  uint32_t m_size;
};

#include <iostream>

struct X
{
    X(uint32_t x)
    {
      _x = x;
      std::cout << "param X::ctor " << _x << std::endl;
    }

    X()
    {
      _x = 0;
      std::cout << "default X::ctor" << std::endl;
    }

    X(const X& x)
    {
      _x = x._x;
      std::cout << "copy X::ctor " << _x << std::endl;
    }

    X(X&& x)
    {
      _x = x._x;
      x._x = 0;
      std::cout << "move X::ctor " << _x << std::endl;
    }

    ~X()
    {
      std::cout << "X::dtor " << _x << std::endl;
    }
    uint32_t _x;
  };

std::ostream& operator <<(std::ostream& stream, const X& x)
{
  stream << x._x;
  return stream;
}

int main()
{
  list<X> l;
  for (uint32_t i = 0; i < 100; i++)
  {
    l.emplace_back(i);
  }

  for (auto it = l.begin(); it != l.end(); ++it)
  {
    std::cout << *it << std::endl;
  }

  return 0;
}