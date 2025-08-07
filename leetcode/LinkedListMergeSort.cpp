#include <cassert>
#include <tuple>
#include <iostream>
#include "Reuseables.h"

using Node = LinkedListNode<int>;
using vector = std::vector<int>;

// Both non-null
void joinLists(Node *left, Node *right)
{
  Node* prev = nullptr;
  while (left)
  {
    prev = left;
    left = left->m_next;
  }

  prev->m_next = right;
}

// Both non-null
// returns next node to be copied from
Node* copyList(Node* from, Node* to)
{
  while (from && to)
  {
    to->m_val = from->m_val;
    from = from->m_next;
    to = to->m_next;
  }

  return from;
}


// Both should be non-null
void mergeLists(Node* l1, Node* l2)
{
  Node *currL1 = l1;
  Node *currL2 = l2;
  Node *resStart = nullptr;
  Node *resEnd = nullptr;

  while(currL1 && currL2)
  {
    Node* smallerNode = currL1->m_val < currL2->m_val? currL1 : currL2;
    Node *nextNode = new Node(smallerNode->m_val, nullptr);

    !resStart? resStart = resEnd = nextNode :
               resEnd = resEnd->m_next = nextNode;

    currL1 == smallerNode? currL1 = currL1->m_next:
                           currL2 = currL2->m_next;
  }

  while (currL1)
  {
    resEnd = resEnd->m_next = new Node(currL1->m_val, nullptr);
    currL1 = currL1->m_next;
  }

  while (currL2)
  {
    resEnd = resEnd->m_next = new Node(currL2->m_val, nullptr);
    currL2 = currL2->m_next;
  }

  copyList(copyList(resStart, l1), l2);
  joinLists(l1, l2);

  // Delete the intermediate list
  deleteLinkedList(resStart);
}

// Param should be non-null
Node* jump(Node* start, int len)
{
  while(start && len--) start = start->m_next;
  return start;
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
  mergeSort(slow->m_next);

  // This methoed expects null-terminated lists, so temporarily breaking the list from
  // between, they will be re-joined in the megeList method, but we have to save the original link
  // between the 2 fragments as it will be later required in the mergeLists method
  Node* temp = slow->m_next;
  slow->m_next = nullptr;
  mergeSort(start);
  mergeLists(start, temp);
  return start;
}

int main()
{
  std::function<void(const int&)> printer = [](const int& val) { std::cout << val << " "; };

  Node* sorted = mergeSort(std::get<0>(createNormalLinkedList(vector{1})));
  assert((createVectorFromList(sorted) == vector{1}));
  deleteLinkedList(sorted);

  sorted =  mergeSort(std::get<0>(createNormalLinkedList(vector{1,2})));
  assert((createVectorFromList(sorted) == vector{1,2}));
  deleteLinkedList(sorted);

  sorted = mergeSort(std::get<0>(createNormalLinkedList(vector{2,1})));
  assert((createVectorFromList(sorted) == vector{1,2}));
  deleteLinkedList(sorted);

  sorted = mergeSort(std::get<0>(createNormalLinkedList(vector{})));
  assert((createVectorFromList(sorted) == vector{}));
  deleteLinkedList(sorted);

  sorted = mergeSort(std::get<0>(createNormalLinkedList(vector{5,4,3,2,1})));
  assert((createVectorFromList(sorted) == vector{1,2,3,4,5}));
  deleteLinkedList(sorted);

  sorted = mergeSort(std::get<0>(createNormalLinkedList(vector{1,2,3,5,4})));
  assert((createVectorFromList(sorted) == vector{1,2,3,4,5}));
  deleteLinkedList(sorted);

  return 0;
}