#include <cassert>
#include <tuple>
#include <iostream>
#include "Reuseables.h"

using Node = LinkedListNode<int>;
using KeyValVector = std::vector<int>;
using tuple = std::tuple<Node*, Node*>;

// Time Complexity: O(n*lg(n))
// Space complexity: O(1)


// Does in-place merging of sorted lists
Node* mergeLists(Node* l1, Node* l2)
{
  if (!l1) return l2;
  else if (!l2) return l1;

  auto [smaller, larger] = (*l1 <= *l2) ? tuple{l1, l2} : tuple{l2, l1};

  // After the while loop, *smaller will be > larger or will be null
  // We'd need to keep track of last non-null node that was <= *larger
  // prevSmaller will play that role
  Node *prevSmaller = nullptr;
  Node *ret = smaller;
  while (smaller && *smaller <= *larger)
  {
    prevSmaller = smaller;
    smaller = smaller->m_next;
  }

  prevSmaller->m_next = larger;
  mergeLists(smaller, larger);

  return ret;
}

Node* mergeSort(Node* start)
{
  if (!start || !start->m_next) return start;

  // Using slow-fast pointer mechaism to find the mid of the list
  Node *slow = start;
  Node *fast = start;
  while (fast = jump(fast, 2))
  {
    slow = jump(slow, 1);
  } 

  // Sort the 2nd half
  Node* sortedLeft = mergeSort(slow->m_next);

  // This method expects null-terminated lists, so temporarily breaking the list from
  // middle before sorting the 1st half, they will be re-joined in the megeLists method
  slow->m_next = nullptr;
  Node* sortedRight = mergeSort(start);
  return mergeLists(sortedLeft, sortedRight);
}

int main()
{
  using vector = std::vector<int>;
  Node* sorted = mergeSort(createSinglyLinkedList(vector{1}));
  assert((createVectorFromList(sorted) == vector{1}));
  deleteLinkedList(sorted);

  sorted =  mergeSort(createSinglyLinkedList(vector{1,2}));
  assert((createVectorFromList(sorted) == vector{1,2}));
  deleteLinkedList(sorted);

  sorted = mergeSort(createSinglyLinkedList(vector{2, 1}));
  assert((createVectorFromList(sorted) == vector{1, 2}));
  deleteLinkedList(sorted);

  sorted = mergeSort(createSinglyLinkedList(vector{}));
  assert((createVectorFromList(sorted) == vector{}));
  deleteLinkedList(sorted);

  sorted = mergeSort(createSinglyLinkedList(vector{5,4,3,2,1}));
  assert((createVectorFromList(sorted) == vector{1,2,3,4,5}));
  deleteLinkedList(sorted);

  sorted = mergeSort(createSinglyLinkedList(vector{1,2,3,5,4}));
  assert((createVectorFromList(sorted) == vector{1,2,3,4,5}));
  deleteLinkedList(sorted);
  
  return 0;
}