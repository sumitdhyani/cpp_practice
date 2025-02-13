#include<stdint.h>

template <class T>
struct list
{
  struct Node{
    Node(const T& val) : m_val(val), m_prev(nullptr), m_next(nullptr)
    {}
    
    Node(T&& val) : m_val(val), m_prev(nullptr), m_next(nullptr)
    {}

    template<class... Args>
    Node(const Args&... args) : m_val(args...), m_prev(nullptr), m_next(nullptr)
    {}

    Node(Node* prev, Node* next, const T& val) : m_val(val), m_prev(prev), m_next(next)
    {}

    Node(Node* prev, Node* next, T&& val) : m_val(val), m_prev(prev), m_next(next)
    {}

    template<class... Args>
    Node(Node* prev, Node* next, const Args&... args) : m_val(args...), m_prev(prev), m_next(next)
    {}

    T m_val;
    Node* m_next;
    Node* m_prev;
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
      m_node = m_node->m_next;
      return *this;
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

  struct reverse_iterator
  {
    reverse_iterator(Node* node) : m_node(node)
    {}

    reverse_iterator operator ++()
    {
      m_node = m_node->m_prev;
      return *this;
    }

    T* operator->()
    {
      return &m_node->m_val;
    }

    T& operator*()
    {
      return m_node->m_val;
    }

    bool operator!=(const reverse_iterator& other)
    {
      return m_node != other.m_node;
    }

    private:
    Node* m_node;
  };

  struct const_reverse_iterator
  {
    const_reverse_iterator(Node* node) : m_node(node)
    {}

    const_reverse_iterator operator ++()
    {
      m_node = m_node->m_prev;
      return *this;
    }

    const T* operator->()
    {
      return &m_node->m_val;
    }

    const T& operator*()
    {
      return m_node->m_val;
    }

    bool operator!=(const const_reverse_iterator& other)
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

  reverse_iterator rbegin()
  {
    if (m_size)
    {
      return reverse_iterator(m_end);
    }

    return rend();
  }

  reverse_iterator rend()
  {
    static reverse_iterator end(nullptr);
    return end;
  }

  const_reverse_iterator rbegin() const
  {
    if (m_size)
    {
      return const_reverse_iterator(m_end);
    }

    return end();
  }

  const_reverse_iterator rend() const
  {
    static const_reverse_iterator end(nullptr);
    return end;
  }

  void push_back(const T& val)
  {
    if (m_start)
    {
      m_end->m_next = new Node(m_end, (Node*)nullptr, val);
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
      Node* newStart = new Node(nullptr, m_start, val);
      m_start = newStart;
    }
    else
    {
      m_start = m_end = new Node(val);
    }

    ++m_size;
  }

  void push_back(const T&& val)
  {
    if (m_start)
    {
      m_end->m_next = new Node(m_end, (Node*)nullptr, val);
      m_end = m_end->m_next;
    }
    else
    {
      m_start = m_end = new Node(val);
    }

    ++m_size;
  }

  void push_front(const T&& val)
  {
    if (m_start)
    {
      Node* newStart = new Node(nullptr, m_start, val);
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
      m_end->m_next = new Node(m_end, (Node*)nullptr, args...);
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
      Node* newStart = new Node(nullptr, m_start, args...);
      m_start = newStart;
    }
    else
    {
      m_start = m_end = new Node(args...);
    }

    ++m_size;
  }

  void pop_back()
  {
    if (!m_size)
    {
      return;
    }

    cleanupTillEnd(m_end);
    --m_size;
  }

  void pop_front()
  {
    if (!m_size)
    {
      return;
    }

    cleanupTillStart(m_start);
    --m_size;
  }

  iterator erase(iterator pos)
  {
    Node* node = pos.m_node;
    Node* prev = node->m_prev;
    Node* next = node->m_next;

    pos++;

    if (prev)
    {
      prev->m_next = next;
    }

    if(next)
    {
      next->m_prev = prev;
    }

    cleanupTillEnd(node, node);
    return pos;
  }

  const_iterator erase(const_iterator pos)
  {
    iterator res = erase(iterator(pos.m_node));
    return const_iterator(res.m_node);
  }

  iterator erase(iterator start, iterator end)
  {
    cleanupTillEnd(start.m_node, end != end()? end.m_node->m_prev : (Node*)nullptr);
    return end;
  }

  const_iterator erase(const_iterator start, const_iterator end)
  {
    erase(iterator(start.m_node), iterator(end.m_node));
    return end;
  }

  reverse_iterator erase(reverse_iterator end, reverse_iterator start)
  {
    cleanupTillStart(end.m_node, start != rend()? start.m_node->m_next : (Node*)nullptr);
    return end;
  }

  const_reverse_iterator erase(const_reverse_iterator end, const_reverse_iterator start)
  {
    erase(reverse_iterator(end.m_node), reverse_iterator(start.m_node));
    return end;
  }

  bool empty()
  {
    return m_size == 0;
  }

  uint32_t size()
  {
    return m_size;
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

  void cleanupTillStart(Node* end)
  {
    if (end)
    {
      cleanupTillEnd(end->m_prev);
      delete end;
    }
  }

  void cleanupTillEnd(Node* start, Node* end)
  {
    if (!end)
    {
      cleanupTillEnd(start);
    }
    else
    {
      if (start != end)
      {
        cleanupTillEnd(start->m_next, end);
      }

      delete start;
    }
  }

  void cleanupTillStart(Node* end, Node* start)
  {
    if (!start)
    {
      cleanupTillStart(end);
    }
    else
    {
      if (end != start)
      {
        cleanupTillStart(end->m_prev, start);
      }

      delete end;
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
    l.push_back(std::move(X(i)));
  }

  for (auto it = l.rbegin(); it != l.rend(); ++it)
  {
    std::cout << *it << std::endl;
  }

  for (auto it = l.rbegin(); it != l.rend(); ++it)
  {
    std::cout << *it << std::endl;
  }

  return 0;
}