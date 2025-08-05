#pragma once
#include <vector>
#include <functional>

template <class T>
struct LinkedListNode
{
  T m_val;
  LinkedListNode *m_next;

  LinkedListNode(const T &val, LinkedListNode* next) :
    m_val(val),
    m_next(next)
  {}
};

template <class T>
LinkedListNode<T> *jump(LinkedListNode<T>* node)
{
  node = node ? node->m_next : node;
  node = node ? node->m_next : node;
  return node;
}

template <class T>
std::tuple<LinkedListNode<T>*, LinkedListNode<T>*> createNormalLinkedList(const std::vector<T> &arr)
{
  using Node = LinkedListNode<T>;
  if (!arr.size())
    return {nullptr, nullptr};

  Node* start = new Node(arr[0], nullptr);
  Node* end = start;

  for (int i = 1; i < arr.size(); ++i)
  {
    end->m_next = new Node(arr[i], nullptr);
    end = end->m_next;
  }

  return {start, end};
}

template <class T>
std::tuple<LinkedListNode<T>*, LinkedListNode<T>*> createCircularLinkedList(const std::vector<T> &arr)
{
  if (!arr.size())
    return {nullptr, nullptr};

  auto [start, end] = createNormalLinkedList(arr);
  end->m_next = start;
  return {start, end};
}

// For circularly linked list, i don't know where it ends, i will need the number of nodes in that case
template <class T>
void deleteLinkedList(LinkedListNode<T>* start, LinkedListNode<T> *end)
{
  if (!start)
    return;

  while (start != end)
  {
    delete start;
    start = start->m_next;
  }

  delete end;
}

// For circularly linked list, i don't know where it ends, i will need the number of nodes in that case
template <class T>
void deleteLinkedList(LinkedListNode<T> *start)
{
  while (start)
  {
    auto temp = start->m_next;
    delete start;
    start = temp;
  }
}

// For circularly linked list, i don't know where it ends, i will need the number of nodes in that case
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