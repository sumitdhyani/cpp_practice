#include <cassert>
#include <iostream>
#include <vector>
#include <map>

using vector = std::vector<int>;


using map = std::map<int, int, std::greater<int>>;


vector maxSlidingWindow(const vector& nums, int k)
{
  if (k > nums.size()) return vector{};

  // Result vector
  vector res;

  // First element will be the greatest, so that the begin() will yield the largest in the window
  // Key: the element, Value: Frequency
  // This key-value dynamics will allow storage of duplicate elements.
  // Also there is one more purpose to haing frequency as thevalue:
  // When the we move to the next window, and remove the 1st element of the window, then
  // having the frequency will let us know if we ought to remove the element or retain it in this map
  // For example if the removed element had a frequency 1, then it occurred in the window once and thus this
  // key-value entry should be removed
  map frequencyTable;

  for(int i = 0, currWindowSize = 0; i < nums.size(); ++i)
  {
    if (auto it = frequencyTable.find(nums[i]); it != frequencyTable.end())
    {
      // Element already existed, increase the frequency by 1
      ++it->second;
    }
    else
    {
      // Element inserted for the 1st time
      frequencyTable[nums[i]] = 1;
    }

    // If the currWindowSize is = k, then it's time to put the max in the res vector
    // and remove the 1st element of this window and currWindowSize becomes k-1
    if (++currWindowSize == k)
    {
      res.push_back(frequencyTable.begin()->first);
      auto it = frequencyTable.find(nums[i-k+1]);
      if(!(--it->second)) frequencyTable.erase(it);
      --currWindowSize;
    }
  }

  return res;
}

int main()
{
  vector nums{};
  assert((maxSlidingWindow(nums, 1) == vector{}));

  nums = {1};
  assert((maxSlidingWindow(nums, 1) == vector{1}));

  nums = {1, 2};
  assert((maxSlidingWindow(nums, 3) == vector{}));

  nums = {1, 2};
  assert((maxSlidingWindow(nums, 1) == vector{1,2}));

  nums = {1, 2};
  assert((maxSlidingWindow(nums, 2) == vector{2}));

  nums = {1, 2, 3};
  assert((maxSlidingWindow(nums, 1) == vector{1,2,3}));

  nums = {1, 2, 3};
  assert((maxSlidingWindow(nums, 2) == vector{2, 3}));

  nums = {3, 2, 1, 2, 2, 5};
  assert((maxSlidingWindow(nums, 3) == vector{3, 2, 2, 5}));

  nums = {6, 5, 4, 3, 2, 1};
  assert((maxSlidingWindow(nums, 3) == vector{6, 5, 4, 3}));

  nums = {2, 1, 2, 2, 1, 2};
  assert((maxSlidingWindow(nums, 3) == vector{2, 2, 2, 2}));

  nums = {4, 4, 4, 4, 3, 3, 5, 5, 6};
  assert((maxSlidingWindow(nums, 4) == vector{4, 4, 4, 5, 5, 6}));

  nums = {1, 2, 3, 2, 2, 3};
  assert((maxSlidingWindow(nums, 3) == vector{3, 3, 3, 3}));
  /////////////////////////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;
}