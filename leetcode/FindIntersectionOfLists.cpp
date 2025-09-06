#include <iostream>
#include <tuple>
#include <cassert>
#include "Reuseables.h"

using Node = LinkedListNode<int>;

// Should always pass non-null to it
Node* findEndOfList(Node* start)
{
  Node* prev = nullptr;
  while (start)
  {
    prev = start;
    start = start->m_next;
  }

  return prev;
}

bool hasNode(Node* start, Node* node)
{
  while (start)
  {
    if (start == node) return true;
    else start = start->m_next;
  }

  return false;
}

int listsIntersect(Node* start1, Node* start2)
{
  if (!(start1 && start2)) return -1;
  
  auto [end1, end2] = std::make_tuple(findEndOfList(start1), findEndOfList(start2));

  if (hasNode(start1, end2))
  {
    return end2->m_val;
  }
  else if (hasNode(start2, end1))
  {
    return end1->m_val;
  }
  else
  {
    return -1;
  }
}

int main()
{
  using vector = std::vector<int>;
  // Test 1: No intersection
  auto [s1, e1] = createNormalLinkedList(vector{1, 2, 3});
  auto [s2, e2] = createNormalLinkedList(vector{4, 5, 6});
  e2->m_next = s1;
  assert(listsIntersect(s1, s2) == -1);

  std::tie(s1, e1) = createNormalLinkedList(vector{1, 2, 3});
  std::tie(s2, e2) = createNormalLinkedList(vector{3, 2, 1});
  assert(listsIntersect(s1, s2) == -1);

  printList<int>(s1, [](const int& n){ std::cout << n << " ";});
  std::cout << std::endl;
  printList<int>(s2, [](const int& n){ std::cout << n << " ";});
  std::cout << std::endl;
  /////////////////////////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;
}
