#include <iostream>
#include <cassert>
#include "Reuseables.h"

using Node = LinkedListNode<int>;
using vector = std::vector<int>;

int length(Node* start)
{
  int ret = 0;
  while(start)
  {
    ++ret;
    start = start->m_next;
  }

  return ret;
}

Node* jump(Node* start, int len)
{
  if (!(len && start)) return start;

  while(start && len--)
  {
    start = start->m_next;
  }

  return start;
}


// Overall idea is to find the nodes in each list from where they are equidistant from
// the intersecting node(which will be nullptr if they don't intersect), using the difference
// in length of the lists.
// The longer list is jumped equal to the length-difference so when iterated, they are bound to
// become equal eventually, and there where our intersection node is 
Node* getIntersectionNode(Node* start1, Node* start2)
{
  if (!(start1 && start2)) return nullptr;

  int l1 = length(start1);
  int l2 = length(start2);

  int diff = l1 - l2;

  // Bring the heads equidistant from the intersecting
  // Node(which will be nullptr if they don't intersect)
  diff > 0? start1 = jump(start1, diff):
            start2 = jump(start2, -diff);

  // We don't chech for null-ness here as both will become nullptr together
  // so equality check will take care of that
  while (start1 != start2)
  {
    start1 = start1->m_next;
    start2 = start2->m_next;
  }

  return start1;
}

// Helper: construct two lists that intersect at a given position.
void basicIntersectionTest()
{
  // List A: 1 -> 2 -> 3 \
    //                      6 -> 7
  // List B:       4 -> 5 /
  Node *intersect = new Node(6, nullptr);
  intersect->m_next = new Node(7, nullptr);

  // First list
  Node *a = new Node(1, nullptr);
  a->m_next = new Node(2, nullptr);
  a->m_next->m_next = new Node(3, nullptr);
  a->m_next->m_next->m_next = intersect;
  // Second list
  Node *b = new Node(4, nullptr);
  b->m_next = new Node(5, nullptr);
  b->m_next->m_next = intersect;

  assert(getIntersectionNode(a, b) == intersect);
  assert(getIntersectionNode(a, nullptr) == nullptr);
  assert(getIntersectionNode(nullptr, b) == nullptr);

  // Clean up memory
  delete a->m_next->m_next->m_next->m_next; // 7
  delete a->m_next->m_next->m_next;       // 6
  delete a->m_next->m_next;             // 3
  delete a->m_next;                   // 2
  delete a;                         // 1
  delete b->m_next;                   // 5
  delete b;                         // 4
}



int main()
{
  basicIntersectionTest();

  auto [s1, e1] = createNormalLinkedList(vector{1, 2, 3});
  auto [s2, e2] = createNormalLinkedList(vector{4, 5, 6});
  assert(getIntersectionNode(s1, s2) == nullptr);

  assert(getIntersectionNode(s1, s1) == s1);

  assert(getIntersectionNode(nullptr, s1) == nullptr);
  assert(getIntersectionNode(nullptr, nullptr) == nullptr);
  return 0;
}
