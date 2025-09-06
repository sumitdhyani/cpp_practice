// <!-- 3224. Minimum Array Changes to Make Differences Equal
// Medium
// Topics
// Companies
// Hint
// You are given an integer array nums of size n where n is even, and an integer k.

// You can perform some changes on the array, where in one change you can replace any element in the array with any integer in the range from 0 to k.

// You need to perform some changes (possibly none) such that the final array satisfies the following condition:

// There exists an integer X such that abs(a[i] - a[n - i - 1]) = X for all (0 <= i < n).
// Return the minimum number of changes required to satisfy the above condition.

 

// Example 1:

// Input: nums = [1,0,1,2,4,3], k = 4

// Output: 2

// Explanation:
// We can perform the following changes:

// Replace nums[1] by 2. The resulting array is nums = [1,2,1,2,4,3].
// Replace nums[3] by 3. The resulting array is nums = [1,2,1,3,4,3].
// The integer X will be 2.

// Example 2:

// Input: nums = [0,1,2,3,3,6,5,4], k = 6

// Output: 2

// Explanation:
// We can perform the following operations:

// Replace nums[3] by 0. The resulting array is nums = [0,1,2,0,3,6,5,4].
// Replace nums[4] by 4. The resulting array is nums = [0,1,2,0,4,6,5,4].
// The integer X will be 4.

 

// Constraints:

// 2 <= n == nums.length <= 105
// n is even.
// 0 <= nums[i] <= k <= 105 -->

#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
using namespace std;

int minChanges(vector<int> &nums, int k)
{
  int numPairs = nums.size() / 2;
  int* allArray = new int[(k+1)*2 + 1]{0};
  int* freqArray = allArray;
  int* rangeArray = freqArray + k + 1;
  for (int i = 0; i < numPairs; ++i)
  {
    ++freqArray[abs(nums[i] - nums[nums.size()-1-i])];

    int smaller = std::min(nums[i], nums[nums.size() - 1 - i]);
    int larger = std::max(nums[i], nums[nums.size() - 1 - i]);
    int maxDiff = std::max(larger, k - smaller);
    ++rangeArray[0];
    --rangeArray[maxDiff+1];
  }

  int minRequiredChanges = 0x7fffffff;
  int rangeIterator = 0;
  for(int i = 0; i <= k; i++)
  {
    rangeIterator += rangeArray[i];
    int currRequiredChanges = rangeIterator - freqArray[i] +
                              (numPairs - rangeIterator) * 2;
    // if (numPairs > rangeIterator)
    // {
    //   currRequiredChanges += (numPairs - rangeIterator) * 2;
    // }

    if (currRequiredChanges < minRequiredChanges)
    {
      minRequiredChanges = currRequiredChanges;
    }
  }

  delete[] allArray;
  return minRequiredChanges;
}

int main()
{
  std::vector<int> vec;
  vec = {1, 0, 1, 2, 4, 3};
  assert(minChanges(vec, 4) == 2);

  vec = {0, 1, 2, 3, 3, 6, 5, 4};
  assert(minChanges(vec, 6) == 2);

  /////////////////////////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;
}
//      1 3
//0,1,2,3,4,5,6