// Clone a Linked List with Multiple Random Pointers
//     Node Structure : 
//     struct MultiRandomNode
// {
//   int val;
//   MultiRandomNode *next;
//   std::vector<MultiRandomNode *> randoms;
//   MultiRandomNode(int x) : val(x), next(nullptr) {}
// };
// MultiRandomNode *copyMultiRandomList(MultiRandomNode *head);

#include <cassert>
#include <iostream>
#include <vector>

    struct Node
{
  int val;
  Node *next;
  std::vector<Node*> randoms;
  Node(int x, Node* ptr = nullptr) : val(x), next(ptr) {}
};

Node* copyMultiRandomList(Node *head)
{
  if (!head) return nullptr;

  // Inserting the copy nodes between original nodes
  Node* start = head;
  while (start)
  {
    Node* copy = new Node(start->val, start->next);
    start->next = copy;
    start = copy->next;
  }

  // Setting the random pointers for copy nodes
  Node* orig  = head;
  Node* copy = orig->next;
  Node* ret = copy;

  while(orig)
  {
    for (auto random : orig->randoms)
    {
      copy->randoms.push_back(random->next);
    }
    
    orig = copy->next;
    copy = orig? orig->next : nullptr;
  }

  // Detaching the copied fron the original list
  orig = head;
  copy = orig->next;
  while (orig)
  {
    Node* nextOrig = copy->next;
    orig->next = nextOrig;
    if (nextOrig) copy->next = nextOrig->next;
    orig = nextOrig;
    copy = orig? orig->next : nullptr;
  }

  return ret;

}


void printList(Node* head) {
  Node* curr = head;
  int idx = 0;
  while (curr) {
    std::cout << "Node " << idx << ": val=" << curr->val << ", randoms=[";
    for (auto r : curr->randoms) {
      std::cout << (r ? r->val : -1) << ",";
    }
    std::cout << "]\n";
    curr = curr->next;
    ++idx;
  }
}

void testCopyMultiRandomList_Simple() {
  // Create nodes
  Node* n1 = new Node(1);
  Node* n2 = new Node(2);
  Node* n3 = new Node(3);
  n1->next = n2;
  n2->next = n3;
  // Set randoms
  n1->randoms = {n3};
  n2->randoms = {n1, n3};
  n3->randoms = {n2};

  Node* copy = copyMultiRandomList(n1);

  // Print original and copy
  std::cout << "Original list:\n";
  printList(n1);
  std::cout << "Copied list:\n";
  printList(copy);

  // Check values and structure
  assert(copy != n1);
  assert(copy->val == n1->val);
  assert(copy->next->val == n2->val);
  assert(copy->next->next->val == n3->val);
  // Check randoms
  assert(copy->randoms.size() == 1);
  assert(copy->randoms[0]->val == 3);
  assert(copy->next->randoms.size() == 2);
  assert(copy->next->randoms[0]->val == 1);
  assert(copy->next->randoms[1]->val == 3);
  assert(copy->next->next->randoms.size() == 1);
  assert(copy->next->next->randoms[0]->val == 2);

  // Check deep copy (nodes are different)
  assert(copy != n1);
  assert(copy->next != n2);
  assert(copy->next->next != n3);
}

void testCopyMultiRandomList_Empty() {
  Node* copy = copyMultiRandomList(nullptr);
  assert(copy == nullptr);
}

void testCopyMultiRandomList_SingleNode() {
  Node* n1 = new Node(42);
  n1->randoms = {n1};
  Node* copy = copyMultiRandomList(n1);
  assert(copy != n1);
  assert(copy->val == 42);
  assert(copy->randoms.size() == 1);
  assert(copy->randoms[0] == copy);
}

int main()
{
  testCopyMultiRandomList_Empty();
  testCopyMultiRandomList_SingleNode();
  testCopyMultiRandomList_Simple();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}