#include <cassert>
#include <iostream>
#include "Reuseables.h"

using Node = LinkedListNode<int>;

bool isCyclePresent(Node* start)
{
  Node *stroller = start;
  Node *jumper = jump(start);

  while (jumper)
  {
    if (jumper == stroller)
    {
      return true;
    }
    else
    {
      stroller = stroller->m_next;
      jumper = jumper->m_next;
      jumper = jumper? jumper->m_next : jumper;
    }
  }

  return false;
}

std::tuple<Node *, Node *> createNormalLinkedList(int *arr, int n);
std::tuple<Node *, Node *> createCircularLinkedList(int *arr, int n);

std::tuple<Node *, Node *> createNormalLinkedList(const std::vector<int>& arr)
{
  if (!arr.size()) return {nullptr, nullptr};

  Node *start = new Node(arr[0], nullptr);
  Node *end = start;

  for (int i = 1; i < arr.size(); ++i)
  {
    end->m_next = new Node(arr[i], nullptr);
    end = end->m_next;
  }

  return {start, end};
}

int main()
{
  std::vector arr = {1,2,3,4,5};
  
  auto [start, end] = createNormalLinkedList(arr);
  assert(!isCyclePresent(start));
  deleteLinkedList(start, end);


  std::tie(start, end) = createCircularLinkedList(arr);
  assert(isCyclePresent(start));
  deleteLinkedList(start, end);

  /////////////////////////////////////////////////////////////////
  arr = {1};

  std::tie(start, end) = createNormalLinkedList(arr);
  assert(!isCyclePresent(start));
  deleteLinkedList(start, end);

  std::tie(start, end) = createCircularLinkedList(arr);
  assert(isCyclePresent(start));
  deleteLinkedList(start, end);

  /////////////////////////////////////////////////////////////////
  arr = {};

  std::tie(start, end) = createNormalLinkedList(arr);
  assert(!isCyclePresent(start));
  deleteLinkedList(start, end);

  std::tie(start, end) = createCircularLinkedList(arr);
  assert(!isCyclePresent(start));
  deleteLinkedList(start, end);
}