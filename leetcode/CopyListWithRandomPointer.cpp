#include <cassert>
#include <vector>

using KeyValVector = std::vector<int>;

struct Node
{
  int val;
  Node *next;
  Node *random;

  Node(int x) : val(x), next(nullptr), random(nullptr) {}
};

KeyValVector createVectorFromList(Node* start)
{
  KeyValVector res;
  while (start)
  {
    res.push_back(start->val);
    start = start->next;
  }

  return res;
}

Node* createRandomListWithAllRandomsAsNullPtr(const KeyValVector& arr)
{
  Node *start = nullptr;
  Node *end = nullptr;

  for (auto const& num : arr)
  {
    Node* node = new Node(num);
    !end? end = start = node :
          end = end->next = node;
  }

  return start;
}

Node *copyRandomList(Node *head)
{
  if (!head) return nullptr;

  Node* curr = head;

  // Insert the 'copy' nodes between the original nodes
  while (curr)
  {
    Node* originalNext = curr->next;
    Node* copy = new Node(curr->val);

    curr->next = copy;
    copy->next = originalNext;

    curr = curr->next;
  }

  Node* ret = head->next;
  curr = head;
  while (curr)
  {
    Node *copy = curr->next;
    Node *random = curr->random;

    // Since the nodes are duplicated in the previous loop,
    // the 'next' or random will point to its copy, which is assigned to the 'random' of copy
    copy->random = random? random->next : nullptr;

    // Reset the next pointer of original node to where it originally pointed 
    curr->next = copy->next;

    // set the curr to the original next, that's pointed by the 'next' of copy
    // if it's non-null, then set copy->next to the original's copy, which is now curr->next
    if(curr = copy->next)
    {
      copy->next = curr->next;
    }
  }

  return ret;

}

// Example test to verify correctness
void randomPointerTest()
{
  // Construct original list: 1 -> 2 -> 3
  Node *n1 = new Node(1);
  Node *n2 = new Node(2);
  Node *n3 = new Node(3);

  n1->next = n2;
  n2->next = n3;
  n3->next = nullptr;

  n1->random = n3; // random points from 1 to 3
  n2->random = nullptr;
  n3->random = n2; // random points from 3 to 2

  Node *copy = copyRandomList(n1);

  // Tests for deep copy correctness:
  // - Copy nodes are distinct objects
  // - Values match original
  // - Random pointers point inside copy correctly

  assert(copy != nullptr);
  assert(copy != n1);
  assert(copy->val == 1);

  assert(copy->next != nullptr && copy->next != n2);
  assert(copy->next->val == 2);

  assert(copy->next->next != nullptr && copy->next->next != n3);
  assert(copy->next->next->val == 3);

  // Verify random pointers copy
  assert(copy->random == copy->next->next);       // 1's random points to 3
  assert(copy->next->random == nullptr);          // 2's random is nullptr
  assert(copy->next->next->random == copy->next); // 3's random points to 2

  // Cleanup omitted for brevity
}

#include <iostream>
int main()
{
  randomPointerTest();
  /////////////////////////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;
}