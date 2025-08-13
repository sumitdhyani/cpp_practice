#include <cassert>
#include <iostream>
#include "Reuseables.h"

using Node = LinkedListNode<int>;

bool isCyclePresent(Node* start)
{
  Node *stroller = start;
  Node *jumper = jump(start, 1);

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

int main()
{
  std::vector arr = {1,2,3,4,5};
  
  Node* start = createSinglyLinkedList(arr);
  deleteLinkedList(start);


  start = std::get<0>(createCircularLinkedList(arr));
  assert(isCyclePresent(start));
  deleteLinkedList(start);

  /////////////////////////////////////////////////////////////////
  arr = {1};

  start = createSinglyLinkedList(arr);
  assert(!isCyclePresent(start));
  deleteLinkedList(start);

  start = std::get<0>(createCircularLinkedList(arr));
  assert(isCyclePresent(start));
  deleteLinkedList(start);

  /////////////////////////////////////////////////////////////////
  arr = {};

  start = createSinglyLinkedList(arr);
  assert(!isCyclePresent(start));
  deleteLinkedList(start);

  start = std::get<0>(createCircularLinkedList(arr));
  assert(!isCyclePresent(start));
  deleteLinkedList(start);
}