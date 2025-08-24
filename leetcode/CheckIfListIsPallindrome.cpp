// Palindrome Linked List
//     Problem : Determine if a singly linked list is a palindrome by value,
//               in O(1) extra space.
//               Function Signature :
//               bool isPalindrome(ListNode *head);

#include <cassert>
#include <iostream>
#include <vector>
#include "Reuseables.h"

using Node = LinkedListNode<int>;
using vector = std::vector<int>;

Node* jump(Node* node, int len)
{
  while (node && len--) node = node->m_next;
  return node;
}

Node* reverse(Node* start)
{
  if(!start) return start;

  auto next = start->m_next;
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

// Basic idea:
// Find the mid(actually start of the right half) using the slow-fast
// pointer technique
// Reverse the right half and then compare node by node the
// the left hasf and the 'reversed' right half
// Reverse the reversed right half the comparison to brng it back to the original shape
bool isPalindrome(Node *start)
{
  if(!start || !start->m_next) return true;


  // Slow and fast pointers, not named exactly so because of the role they play
  // is more important than what they are IMO
  Node* mid = start;
  Node* end = start;

  while ((mid = jump(mid, 1)) && (end = jump(end, 2)));

  // Reverse the latter half, so that the traversal is reverse of
  // of what it was originally
  // This is needed because just like in the regular pallindrome alogos
  // 1st element from beginning of the left half is to be equal to the 1st elem from end
  // of right half, second from 1st half beginning to 2nd from right half end and so forth
  mid = reverse(mid);
  // Temporary variable storing curret version of mid, before we start traversal below
  // Will be used when we reverse the mid again, to put back the overall list back
  // in its original form, after the comparison is over  
  auto origMid = mid;
  
  while(mid && start->m_val == mid->m_val)
  {
    start = start->m_next;
    mid = mid->m_next;
  }

  reverse(origMid);

  // If mid is non-null then it means that there
  // was a value mismatch somewhere before the sub-lists ended
  // otherwise it means that values kept matching until the sub-lists exhausted
  return !mid;
}


int main()
{
  Node* start = createSinglyLinkedList(vector{1});
  assert(isPalindrome(start));
  deleteLinkedList(start);

  start = createSinglyLinkedList(vector{});
  assert(isPalindrome(start));
  deleteLinkedList(start);

  start = createSinglyLinkedList(vector{1,2});
  assert(!isPalindrome(start));
  deleteLinkedList(start);

  start = createSinglyLinkedList(vector{1, 1});
  assert(isPalindrome(start));
  deleteLinkedList(start);

  start = createSinglyLinkedList(vector{1, 2, 3});
  assert(!isPalindrome(start));
  deleteLinkedList(start);

  start = createSinglyLinkedList(vector{1, 2, 1});
  assert(isPalindrome(start));
  deleteLinkedList(start);

  start = createSinglyLinkedList(vector{1, 1, 1, 1, 1});
  assert(isPalindrome(start));
  deleteLinkedList(start);

  start = createSinglyLinkedList(vector{1, 2, 3, 2, 1});
  assert(isPalindrome(start));
  deleteLinkedList(start);

  return 0;
}