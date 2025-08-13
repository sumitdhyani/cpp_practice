#pragma once
#include <vector>
#include <functional>

template <class T>
struct LinkedListNode
{
  T m_val;
  LinkedListNode *m_next;

  int operator <=>(const LinkedListNode&  other) const
  {
    return m_val - other.m_val;
  }

  LinkedListNode(const T &val, LinkedListNode* next) :
    m_val(val),
    m_next(next)
  {}
};


// Returns nullptr if nullptr is encountered before 'len' steps
template <class T>
LinkedListNode<T> *jump(LinkedListNode<T>* node, int len)
{
  while(node && len--) node = node->m_next;
  return node;
}


// Same as Returns only the start pointer
template <class T>
std::tuple<LinkedListNode<T> *, LinkedListNode<T> *> createNormalLinkedList(const std::vector<T> &arr)
{
  using Node = LinkedListNode<T>;
  Node *start = nullptr;
  Node *end = nullptr;

  for (auto const &val : arr)
  {
    Node *node = new Node(val, nullptr);
    !end? end = start = node :
          end = end->m_next = node;
  }

  return {start, end};
}

// Returns only the start pointer
template <class T>
LinkedListNode<T> *createSinglyLinkedList(const std::vector<T> &arr)
{
  return std::get<0>(createNormalLinkedList(arr));
}

template <class T>
std::tuple<LinkedListNode<T>*, LinkedListNode<T>*> createCircularLinkedList(const std::vector<T> &arr)
{
  auto [start, end] = createNormalLinkedList(arr);
  if(end) end->m_next = start;
  return {start, end};
}

// Delete start to end and return node- m_next if existed, otherwise nullptr
// Both params should be not-null
template <class T>
LinkedListNode<T> *deleteLinkedList(LinkedListNode<T> *start, LinkedListNode<T> *end)
{
  using Node = LinkedListNode<T>;
  
  while (start != end)
  {
    auto temp = start->m_next;
    delete start;
    start = temp;
  }

  Node* ret = jump(end, 1);
  delete end;
  return ret;
}

// Expexts a singly linked list, deletes to the end
template <class T>
void deleteLinkedList(LinkedListNode<T>* start)
{
  deleteLinkedList(start, (LinkedListNode<T>*)nullptr);
} 

template <class T>
void deleteCircularLinkedList(LinkedListNode<T> *start)
{
  using Node = LinkedListNode<T>;
  Node* curr = start? start->m_next : nullptr;
  while (curr != start)
  {
    auto temp = curr->m_next;
    delete curr;
    curr = temp;
  }

  delete start;
}

// For circularly linked list, i don't know where it ends
// Also can be used when the operation is to be performed on a s-b-section of the list
// both sart and end should be non-null
template <class T>
std::vector<T> createVectorFromList(const LinkedListNode<T> *start, LinkedListNode<T> *end)
{
  std::vector<T> res;
  if (!start) return res;

  while (start != end)
  {
    res.push_back(start->m_val);
    start = start->m_next;
  }

  res.push_back(end->m_val);
  return res;
}

template <class T>
std::vector<T> createVectorFromList(const LinkedListNode<T>* start)
{
  std::vector<T> res;
  while (start)
  {
    res.push_back(start->m_val);
    start = start->m_next;
  }

  return res;
}

template <class T>
void printList(LinkedListNode<T>* start, const std::function<void(const T&)>& print)
{
  while (start)
  {
    print(start->m_val);
    start = start->m_next;
  }
}

template <class T>
LinkedListNode<T> *createListFromStream(const std::function<T()>& fetch, const int &len)
{
  using Node = LinkedListNode<T>;

  if (!len) return nullptr;
  std::tuple<Node *, Node *> list = {nullptr, nullptr};
  auto &[start, end] = list;
  for (int i = 0; i < len; ++i)
  {
    Node *node = new Node(fetch(), nullptr);
    if (!start)
    {
      start = end = node;
    }
    else
    {
      end->m_next = node;
      end = end->m_next;
    }
  }
  
  return start;
}

template <class T>
LinkedListNode<T> *createLInkedListStack(const std::vector<T> &arr)
{
  using Node = LinkedListNode<T>;

  Node* start = nullptr;
  for(auto const& val : arr)
  {
    Node* node = new Node(start);
    start = node;
  }

  return start;
}

template <class T>
std::tuple<LinkedListNode<T> *, LinkedListNode<T> *> createLInkedListQueue(const std::vector<T> &arr)
{
  using Node = LinkedListNode<T>;

  Node* start = nullptr;
  Node* end = nullptr;

  for(auto const& val : arr)
  {
    !end? start = end = new Node(val, nullptr) :
          end = end->m_next = new Node(val, nullptr);
          
  }

  return {start, end};
}
