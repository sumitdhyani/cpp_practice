#include <cassert>
#include <iostream>
#include <vector>
#include <stack>
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

// Utility function to set the prev and next simultaneously, to
// avoid repeatedly assigning next and prev
void setLink(Node *prev, Node *next)
{
  if (prev) prev->next = next;
  if (next) next->prev = prev;
}

// Flatten and return the end node of the result
// Required because the core idea is to implant the flattenned child between the
// head and its next pointer, for that we need the beginning of the flattenned child
// we already have the beginning, so we return end
Node *flattenAndReturnEnd(Node *head)
{

  // If head is a terminal or a leaf node, return head, no work to do
  if (!head || !(head->child || head->next)) return head;

  // First flatten the child, if exists, and then later
  // implant it between the head and former next, if present
  if (head->child)
  {
    auto formerNext = head->next;
    auto flattennedChild = flattenAndReturnEnd(head->child);

    // ==== Process of implanting the flattenned chils between head and its current next ====
    setLink(head, head->child);
    setLink(flattennedChild, formerNext);
    head->child = nullptr;
    // ======================================================================================
    
    // If the former next is non-null, then flatten it too
    return formerNext ? flattenAndReturnEnd(formerNext) : flattennedChild;
  }
  // If child is null just return the flattening of the next pointer
  else
  {
    return flattenAndReturnEnd(head->next);
  }
}

Node* flatten(Node* start)
{
  Node *res = flattenAndReturnEnd(start);
  // Go to the beginning of the list, as the function above returns the end of flattenned list
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