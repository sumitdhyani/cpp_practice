#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_map>

using Vector = std::vector<int>;
using HashMap = std::unordered_map<int, int>;

Vector twoSum(const Vector &nums, int k)
{
  Vector res;
  // Key: value at the index, Value: index
  // Keyed with value at idx because while traversing the array,
  // the potential pair-mate of the current element
  HashMap visitedElements;

  for(int i = 0; i < nums.size(); ++i)
  {
    // If the pair-mate has alreadybeen traversed, wer'e done
    // return the pair of the pair-mate and the current index
    if (auto it = visitedElements.find(k - nums[i]);
        it != visitedElements.end())
    {
      res = Vector{it->second, i};
      break;
    }
    else // No pair-mate found, just add this elemts to the 'visitedElements' and continue
    {
      visitedElements[nums[i]] = i;
    }
  }

  return res;
}

int main()
{
  Vector input{1,2,3,4,5};
  assert((maxSlidingWindow(input, 6) == Vector{1,3}));

  input = {};
  assert((maxSlidingWindow(input, 6) == Vector{}));

  input = {1};
  assert((maxSlidingWindow(input, 1) == Vector{}));

  input = {1,2};
  assert((maxSlidingWindow(input, 3) == Vector{0,1}));

  input = {1, 2, 3, 4, 5};
  assert((maxSlidingWindow(input, 15) == Vector{}));

  input = {1, 2, 3, 4, 5};
  assert((maxSlidingWindow(input, 15) == Vector{}));

  input = {1, 2, 3, 4, 5, 6, 7 , 8 , 9, 10};
  assert((maxSlidingWindow(input, 19) == Vector{8,9}));

  std::cout << "All tests passed!" << std::endl;
  return 0;
}