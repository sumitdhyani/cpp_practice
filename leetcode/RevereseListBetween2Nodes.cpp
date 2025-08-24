// Reverse Nodes Between Two Positions
// Reverse nodes in a singly linked list from position m to n in one pass,
// in - place and with O(1) space.Positions are 1 - indexed.
//                                                  Function Signature :
// cpp ListNode *reverseBetween(ListNode *head, int m, int n);

#include <cassert>
#include <iostream>
#include <vector>
#include "Reuseables.h"

using Node = LinkedListNode<int>;
using vector = std::vector<int>;


Node* reverse(Node* start)
{
  if(!start) return start;

  Node* next = start->m_next;
  start->m_next = nullptr;

  while (next)
  {
    auto temp = next->m_next;
    next->m_next = start;
    start = next;
    next = temp;
  }

  return start;
}

// Assumptions 0 < m <= n <= length(head) and HEAD is non-null
Node* reverseBetween(Node *head, int m, int n)
{
  Node* leftBoundary = (m >= 2)? jump(head, m-2) : nullptr;
  Node* start = leftBoundary? leftBoundary->m_next : jump(head, m - 1);
  Node* end = start? jump(start, n-m) : nullptr;
  Node* rightBoundary = end->m_next;

  // Set to nullptr as reverse method expexts bull-terminated list
  end->m_next = nullptr;
  reverse(start);
  
  // Rewire the boundary pointers
  if(leftBoundary) leftBoundary->m_next = end;
  start->m_next = rightBoundary;

  // If the mth node was not the head, it means the the head of new list is still the old head
  // otherwise head will be the head of the reversed list, that is nth node 
  return m > 1? head : end;
}

int main()
{
  Node* start = createSinglyLinkedList(vector{1});
  Node* reversed = reverseBetween(start, 1 , 1);
  assert((createVectorFromList(reversed) == vector{1}));
  deleteLinkedList(reversed);

  start = createSinglyLinkedList(vector{1, 2, 3, 4 , 5, 6});
  reversed = reverseBetween(start, 2, 5);
  assert((createVectorFromList(reversed) == vector{1, 5, 4, 3, 2, 6}));
  deleteLinkedList(reversed);

  start = createSinglyLinkedList(vector{1, 2, 3, 4, 5, 6});
  reversed = reverseBetween(start, 1, 6);
  assert((createVectorFromList(reversed) == vector{6, 5, 4, 3, 2, 1}));
  deleteLinkedList(reversed);

  start = createSinglyLinkedList(vector{1, 2});
  reversed = reverseBetween(start, 1, 1);
  assert((createVectorFromList(reversed) == vector{1, 2}));
  deleteLinkedList(reversed);

  start = createSinglyLinkedList(vector{1, 2});
  reversed = reverseBetween(start, 2, 2);
  assert((createVectorFromList(reversed) == vector{1, 2}));
  deleteLinkedList(reversed);

  start = createSinglyLinkedList(vector{1, 2});
  reversed = reverseBetween(start, 1, 2);
  assert((createVectorFromList(reversed) == vector{2, 1}));
  deleteLinkedList(reversed);

  /////////////////////////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;

  return 0;
}