#include <cassert>
#include <iostream>
#include <vector>
#include "Reuseables.h"

using Node = LinkedListNode<int>;
using vector = std::vector<int>;

bool detectCycle(Node *head)
{
  if (!head) return false;

  Node *slow = head;
  Node *fast = head;
  while (fast)
  {
    slow = jump(slow, 1);
    fast = jump(fast, 2);

    if (slow == fast) break;
  }

  return fast != nullptr;
}

Node* find(Node* head,
           const std::function<bool(Node*)>& predicate)
{
  while (head)
  {
    if (predicate(head)) return head;
    else head = head->m_next;
  }

  return nullptr;
}

void detectAndRemoveCycle(Node* head)
{
  if(!head) return;

  Node* slow = head;
  Node* fast = head;
  
  while(fast)
  {
    slow = jump(slow, 1);
    fast = jump(fast, 2);
    if (slow == fast)
    {
      break;
    }
  }

  if(!fast) return;// No cycle found

  // Reset slow to head
  // this time make fast jump by 1 step so that they meet at the cycle start
  // and then find the cycle end by making one more circular trip
  // This also handles when the list is a circular list
  while(slow != fast)
  {
    slow = jump(slow, 1);
    fast = jump(fast, 1);
  }

  Node* cycleEnd = find(slow, [slow](Node* node) { return (node->m_next == slow); });
  cycleEnd->m_next = nullptr;
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

  std::tie(start, end) = createCircularLinkedList(vector{1, 2});
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

  std::tie(start, end) = createNormalLinkedList(vector{1, 2, 3, 4, 5, 6, 7});
  // the cycle starts fron the middle of the list,
  // instead of being a circular linked list like in other cases
  end->m_next = jump(start, 4);
  assert(detectCycle(start));
  detectAndRemoveCycle(start);
  assert(!detectCycle(start));
  deleteLinkedList(start);

  return 0;
}
