#include <iostream>
#include <tuple>
#include <stack>
#include <cassert>
#include "Reuseables.h"

using Node = LinkedListNode<int>;
using vector = std::vector<int>;

std::stack<Node*> toStack(Node* start)
{
  std::stack<Node*> ret;
  while (start)
  {
    ret.push(start);
    start = start->m_next;
  }

  return ret;
}

Node* getIntersectionNode(Node* start1, Node* start2)
{
  if (!(start1 && start2)) return nullptr;
  else if (start1 == start2) return start1;

  std::stack<Node*> s1(std::move(toStack(start1)));
  std::stack<Node*> s2(std::move(toStack(start2)));
  if(s1.top() != s2.top()) return nullptr;

  Node* prevCommonNode = nullptr;
  while(s1.top() == s2.top())
  {
    prevCommonNode = s1.top();
    s1.pop(); s2.pop();
  }

  return prevCommonNode;
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
