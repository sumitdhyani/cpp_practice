// Move Zeroes(In - place manipulation)
//     Problem : Given an array,
//               move all zeroes to the end while maintaining the relative order of non - zero elements,
//               doing so in - place.

//     Function Signature :
//       void moveZeroes(std::vector<int> &nums);
#include <cassert>
#include <iostream>
#include <vector>

using vector = std::vector<int>;

namespace V1
{
  // Enum representing the curr state of iteration
  enum class State
  {
    ZERO_SECTION,
    NON_ZERO_SECTION,
    END
  };

  // Used to determine the state we start with
  State onInit(const vector &nums)
  {
    if (!nums.size()) return State::END;
    else return !nums[0] ? State::ZERO_SECTION : State::NON_ZERO_SECTION;
  }

  // Do the shifting and increase the 'zeroesToMove' variable
  State onZeroSection(vector &nums,
                      int &currIdx,
                      int &zeroesToMove)
  {
    int temp = currIdx;
    while (0 == nums[currIdx] && currIdx < nums.size())
    {
      nums[currIdx - zeroesToMove] = nums[currIdx];
      ++currIdx;
    }

    zeroesToMove += currIdx - temp;
    return currIdx < nums.size() ? State::NON_ZERO_SECTION : State::END;
  }

  // Just do the shifting
  State onNonZeroSection(vector &nums,
                         int &currIdx,
                         int zeroesToMove)
  {
    while (nums[currIdx] != 0 && currIdx < nums.size())
    {
      nums[currIdx - zeroesToMove] = nums[currIdx];
      ++currIdx;
    }

    return currIdx < nums.size() ? State::ZERO_SECTION : State::END;
  }

  // The idea:
  // The iteration is divided ito 3 states
  // a.) Curr section is a stream of 1 or more zeroes
  // b.) Curr section is a stream of 1 or more non-zeroes
  // c.) Iteration end
  // We have to keep track of no. of zeroes(zeroesToMove) ecountered during iteration
  // While iteration, the curr element will be shifted back by this amount
  // If the the curr section is a non-zero section(state a), we just do the shifting part
  // If the curr section is a zero section we do shifting and also increase the 'zero counter'
  // When we reach the end of the array. we zero-fill the the last 'zeroesToMove' slots
  void moveZeroes(vector &nums)
  {
    auto currState = onInit(nums);
    if (State::END == currState) return;

    int currIdx = 0;
    int zeroesToMove = 0;
    while (State::END != currState)
    {
      currState = currState == State::ZERO_SECTION ?
                  onZeroSection(nums, currIdx, zeroesToMove) :
                  onNonZeroSection(nums, currIdx, zeroesToMove);
    }

    // Zero fill the last 'zeroesToMove' elements
    for (int i = nums.size() - zeroesToMove; i < nums.size(); ++i)
    {
      nums[i] = 0;
    }
  }
}

namespace V2
{
  void moveZeroes(vector& nums)
  {
    int lastNonZeroFoundAt = 0;

    // Place each non-zero element at the front, maintaining their order
    for (int i = 0; i < nums.size(); ++i)
    {
      if (nums[i] != 0)
      {
        nums[lastNonZeroFoundAt++] = nums[i];
      }
    }

    // Fill rest of the array with zeros
    for (int i = lastNonZeroFoundAt; i < nums.size(); ++i)
    {
      nums[i] = 0;
    }
  }
}

namespace V3
{
  void moveZeroes(vector& nums)
  {
    int slow = 0;
    for (int fast = 0; fast < nums.size(); ++fast)
    {
      if (nums[fast] != 0)
      {
        std::swap(nums[slow++], nums[fast]);
      }
    }
  }
}

int main()
{
  vector arr{0,1};
  V3::moveZeroes(arr);
  assert((arr == vector{1,0}));

  arr = {};
  V3::moveZeroes(arr);
  assert((arr == vector{}));

  arr = {0};
  V3::moveZeroes(arr);
  assert((arr == vector{0}));

  arr = {0, 0, 1};
  V3::moveZeroes(arr);
  assert((arr == vector{1, 0, 0}));

  arr = {0, 0};
  V3::moveZeroes(arr);
  assert((arr == vector{0, 0}));

  arr = {1,2,0,0};
  V3::moveZeroes(arr);
  assert((arr == vector{1,2,0,0}));

  arr = {1, 0, 2, 0};
  V3::moveZeroes(arr);
  assert((arr == vector{1, 2, 0, 0}));

  arr = {0, 1, 0, 2};
  V3::moveZeroes(arr);
  assert((arr == vector{1, 2, 0, 0}));

  arr = {0, 0, 1, 2};
  V3::moveZeroes(arr);
  assert((arr == vector{1, 2, 0, 0}));

  arr = {0, 0, 1, 2, 0, 0};
  V3::moveZeroes(arr);
  assert((arr == vector{1, 2, 0, 0, 0, 0}));

  arr = {0, 0, 1, 2, 0, 0, 3};
  V3::moveZeroes(arr);
  assert((arr == vector{1, 2, 3, 0, 0, 0, 0}));

  //////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;
}