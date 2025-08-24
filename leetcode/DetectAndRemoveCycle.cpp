// Detect and Remove Cycle
//     Problem : Detect if a cycle exists in a singly linked list and,
//               if found,
//               remove the cycle so the list becomes properly acyclic.
//               Function Signature : struct ListNode
//                                     {
//                                       int val;
//                                       ListNode *next;
//                                       ListNode(int x) : val(x), next(nullptr) {}
//                                     };
//
//                void detectAndRemoveCycle(ListNode *head);
#include <cassert>
#include <iostream>
#include "Reuseables.h"

using Node = LinkedListNode<int>;
using vector = std::vector<int>;

// start should be non-null
Node* reverse(Node* start)
{
  Node* next = start->m_next;
  start->m_next = nullptr;
  while(next)
  {
    auto temp = next->m_next;
    next->m_next = start;
    start = next;
    next = temp;
  }

  return start;
}

bool detectCycle(Node* head)
{
  if (!head) return false;

  Node *slow = head;
  Node *fast = head;
  while ((fast = jump(fast, 2)) && 
         ((slow = jump(slow, 1)) != fast));

  return fast != nullptr;
}

void detectAndRemoveCycle(Node *head)
{
  if (!head) return;

  // To ensure we are not dealing with a circularly linked list
  // Dealing with a circularly linked list means handling very special case requiring specific code, we will avoid that
  // we will simply move the head 1 step ahead and delete this dummy node when we are done
  head  = new Node(0, head);
  Node* slow = head;
  Node* fast = head;

  
  while ((fast = jump(fast, 2)) && 
         ((slow = jump(slow, 1)) != fast));

  // No cycle detected
  if (!fast) return;

  // Find cycle length, by making them meet again
  int cycleLen = 1;
  while ((slow = jump(slow, 1)) != (fast = jump(fast, 2)));
  {
    ++cycleLen;
  }

  // Determine the total Length
  // Interestingly, reversing a list with a cycle with
  // will lead us back to the start, and we can measure the round-trip length 
  // during the traversal, the actualLength = (roundTripLength + cycleLenth) / 2;
  // A side effect will be that the direction of cycle will be reversed
  // i.e, clockwise cycle will turn anti-clockwise and vice-versa
  // As a result we will have to call reverse once again
  auto start = head;
  Node *next = start->m_next;
  start->m_next = nullptr;
  int roundTripLen = 1;
  while (next)
  {
    auto temp = next->m_next;
    next->m_next = start;
    start = next;
    next = temp;
    ++roundTripLen;
  }

  int totalLen = (roundTripLen + cycleLen) / 2;
  int lengthBeforCycle = totalLen - cycleLen;
  auto cycleNode = jump(head, lengthBeforCycle);
  reverse(head);
  
  // Set the next of 'caycle causing' node' to nullptr
  cycleNode->m_next = nullptr;
  delete head;
}


int main()
{
  auto [start, end] = createCircularLinkedList(vector{1});
  assert(detectCycle(start));
  detectAndRemoveCycle(start);
  assert(!detectCycle(start));
  deleteLinkedList(start);

  std::tie(start, end) = createCircularLinkedList(vector{});
  assert(!detectCycle(start));
  detectAndRemoveCycle(start);
  assert(!detectCycle(start));
  deleteLinkedList(start);

  std::tie(start, end) = createCircularLinkedList(vector{1,2});
  assert(detectCycle(start));
  detectAndRemoveCycle(start);
  assert(!detectCycle(start));
  deleteLinkedList(start);

  std::tie(start, end) = createCircularLinkedList(vector{1, 2});
  assert(detectCycle(start));
  detectAndRemoveCycle(start);
  assert(!detectCycle(start));
  deleteLinkedList(start);

  std::tie(start, end) = createNormalLinkedList(vector{1, 2});
  assert(!detectCycle(start));
  detectAndRemoveCycle(start);
  assert(!detectCycle(start));
  deleteLinkedList(start);

  std::tie(start, end) = createCircularLinkedList(vector{1, 2, 3, 4, 5});
  assert(detectCycle(start));
  detectAndRemoveCycle(start);
  assert(!detectCycle(start));
  deleteLinkedList(start);

  return 0;
}