#include<stdint.h>

template <class T>
struct list
{
  struct Node{
    Node(const T& val) : m_val(val), m_next(nullptr)
    {}

    Node(const T& val, T* next) : Node(val)
    {
      m_next = next;
    }

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
      Node* newStart = new Node(m_start);
      m_start = newStart;
    }
    else
    {
      m_start = m_end = new Node(val);
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

int main()
{
  list<uint32_t> l;
  for (uint32_t i = 0; i < 100; i++)
  {
    l.push_back(i);
  }

  for (auto it = l.begin(); it != l.end(); ++it)
  {
    std::cout << *it << std::endl;
  }

  return 0;
}