// Time Complexity: O(n)

#include <cassert>
#include <iostream>
#include "Reuseables.h"
using Node = LinkedListNode<int>;

// Reverse a sub-section of the list
// start and end should both be non-null
Node *reverse(Node *start, Node *end)
{
  if (start != end)
  {
    Node *prevNextNode = reverse(start->m_next, end);
    prevNextNode->m_next = start;
  }

  return start;
}

// Reverse a group and return the 'start' of the transformed group
Node *reverseInGroups(Node *start, int groupSize)
{
  if (!start)
    return nullptr;
  else if (1 == groupSize)
    return start;

  int currLen = 0;
  Node *groupEnd = start;
  for (int i = 1; groupEnd && i < groupSize; ++i)
  {
    groupEnd = groupEnd->m_next;
  }

  if (groupEnd)
  {
    Node *temp = groupEnd->m_next;
    reverse(start, groupEnd);
    // Start still points to its original m_next, need to set it
    // to the result of the operation in the next group
    start->m_next = reverseInGroups(temp, groupSize);
    return groupEnd;
  }
  else
  {
    return start;
  }
}

int main()
{
  using vector = std::vector<int>;
  Node *start = nullptr;
  Node *end = nullptr;

  int numTestCases;
  std::cin >> numTestCases;

  std::function<void(const int &)> printMethod =
  [](const int &n)
  {
    std::cout << n << " ";
  };

  while (numTestCases--)
  {
    int n, k;
    std::cin >> n >> k;

    std::vector<int> arr;
    arr.reserve(n);
    while (n--)
    {
      int val;
      std::cin >> val;
      arr.push_back(val);
    }

    auto [start, _] = createNormalLinkedList(arr);
    auto transformed = reverseInGroups(start, k);
    printList(transformed, printMethod);
    std::cout << std::endl;
  }

  return 0;
}
