#include <cassert>
#include <iostream>
#include <vector>
#include <tuple>

using intVector = std::vector<int>;

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

using NodeVector = std::vector<Node*>;

// Utility function to set the prev and next simultaneously
void setLink(Node* prev, Node* next)
{
  if (prev) prev->next = next;
  if (next) next->prev = prev;
}

Node* createListFromVector(const intVector& nodes)
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

intVector createVectorFromList(const Node *start)
{
  intVector res;
  while (start)
  {
    res.push_back(start->val);
    start = start->next;
  }

  return res;
}

// Flatten and return the end node of the result
// Requires because the next of end of result will need to be joined with the head's current next
Node *flattenCore(Node *head)
{
  if (!head || !(head->child || head->next)) return head;

  if (auto formerNext = head->next; head->child)
  {
    auto flattennedChild = flattenCore(head->child);
    setLink(head, head->child);
    setLink(flattennedChild, formerNext);
    head->child = nullptr;
    
    return formerNext ? flattenCore(formerNext) : flattennedChild;
  }
  else
  {
    return flattenCore(head->next);
  }
}

Node* flatten(Node* start)
{
  Node *res = flattenCore(start);
  // Go to the beginning of the list, as the core also reeturns the end
  while(res && res->prev)
  {
    res = res->prev;
  }

  return res;
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

  return 0;
}