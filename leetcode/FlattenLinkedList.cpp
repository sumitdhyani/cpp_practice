#include <cassert>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <tuple>

using vector = std::vector<int>;
struct Node
{
  int val;
  Node *prev;
  Node *next;
  Node *child;

  Node(const int& val, Node* prev, Node* next, Node* child)
  {
    this->val = val;
    this->prev = prev;
    this->next = next;
    this->child = child;
  }

  Node(const int &val) : prev(nullptr), next(nullptr), child(nullptr) 
  {
    this->val = val;
  }
};

using NodeVector = std::vector<Node *>;
using NodeStack = std::stack<Node *>;
using NodeQueue = std::queue<Node *>;

Node* createListFromVector(const vector& nodes)
{
  Node *start = nullptr;
  Node *end = nullptr;
  for( auto const val : nodes)
  {
    !start? start = end = new Node(val):
            end = end->next = new Node(val, end, nullptr, nullptr);
  }

  return start;
}

Node* createChildrenForList(Node* start, const NodeVector& branches)
{
  Node* curr = start;
  for (auto const& branch : branches)
  {
    start->child = branch;
    start = start->next;
  }

  return start;
}

vector createVectorFromList(const Node *start)
{
  vector res;
  while (start)
  {
    res.push_back(start->val);
    start = start->next;
  }

  return res;
}

bool isLeafNode(Node* node)
{
  return !(node->child || node->next);
}

// Utility function to set the prev and next simultaneously, to
// avoid repeatedly assigning next and prev
void setLink(Node *prev, Node *next)
{
  if (prev) prev->next = next;
  if (next) next->prev = prev;
}

// Basic idea:
// We have a queue and a stack
// in the beginning the queue has just the hedad and the stack is empty
// Queue's front is always to be element whose next pointer is to be set
// Stack's top is the element which is to be set as the next pointer of element in the queue
// While traversing the queue, when a node is encountered that has a child, its 'next' is set as its child
// but before that, it's former next is pushed to the stack as it will be the 'next' of some other node,
// also the 'child' is pushed to the queue as it needs to get its new chils now
// A stack has been used for the above mentioned orphanned nodes because the last orphanned node will be the
// first to get a parent later, while the queue is being traversed
Node* flatten(Node *head)
{
  // If head is a terminal or a leaf node, return head, no work to do
  if (!head || isLeafNode(head)) return head;

  NodeQueue parents; parents.push(head);
  NodeStack orphans;

  while(!parents.empty())
  {
    auto front = parents.front();
    if (auto child = front->child; child)
    {
      front->next? orphans.push(front->next) : void();
      setLink(front, child);
      parents.push(child);
    }
    else if (isLeafNode(front))
    {
      // Only if there is an orphan do we assign this leaf node as its parent
      if (!orphans.empty())
      {
        setLink(front, orphans.top());
        parents.push(orphans.top());
        orphans.pop();
      }
    }
    else
    {
      parents.push(front->next);
    }

    // The front has no use now, will be popped, regardless
    // which condition was encountered
    parents.pop();
  }

  return head;
}

void deleteList(Node* start)
{
  if(!start) return;

  auto [child, next] = std::tuple<Node*, Node*>(start->child, start->next);
  delete start;
  deleteList(child);
  deleteList(next);
}

int main()
{
  using intVector = std::vector<int>;
  Node* main = createListFromVector(intVector{1,4});
  createChildrenForList(main, NodeVector{createListFromVector(intVector{2,3})});
  auto flattenned = createVectorFromList(flatten(main));
  assert((flattenned == intVector{1, 2, 3, 4}));
  deleteList(main);

  main = createListFromVector(intVector{1, 4});
  createChildrenForList(main, NodeVector{createListFromVector(intVector{2}), createListFromVector(intVector{3})});
  flattenned = createVectorFromList(flatten(main));
  assert((flattenned == intVector{1, 2, 4, 3}));
  deleteList(main);

  main = createListFromVector(intVector{});
  flattenned = createVectorFromList(flatten(main));
  assert((flattenned == intVector{}));
  deleteList(main);

  main = createListFromVector(intVector{1});
  flattenned = createVectorFromList(flatten(main));
  assert((flattenned == intVector{1}));
  deleteList(main);

  main = createListFromVector(intVector{1,2});
  flattenned = createVectorFromList(flatten(main));
  assert((flattenned == intVector{1,2}));
  deleteList(main);

  /////////////////////////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;
}