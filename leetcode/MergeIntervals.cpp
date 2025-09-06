#include <cassert>
#include <iostream>
#include <tuple>
#include <vector>
#include <algorithm>

using namespace std;

typedef std::tuple<int, int> Interval;
typedef std::vector<Interval> Intervals;

Intervals mergeIntervals(const Intervals& intervals)
{
  if (intervals.size() <= 1) return intervals;

  Intervals result{intervals[0]};
  Interval toMerge = intervals[0];
  for (int i = 1, j = 0; i < intervals.size(); ++i)
  {
    auto const &[l1, r1] = result.back();
    auto const &[l2, r2] = intervals[i];
    if(l2 <= r1)
    {
      result.back() = Interval{l1, r2};
    }
    else
    {
      result.push_back(intervals[i]);
    }
  }

  return result;
}

int main()
{
  Intervals intervals = {{1,2}, {3,4}};
  Intervals res = {{1, 2}, {3, 4}};
  assert(mergeIntervals(intervals) == res);

  intervals = {{1, 2}, {2, 5}, {4, 9}};
  res = {{1, 9}};
  assert(mergeIntervals(intervals) == res);

  intervals = {{1, 2}, {3, 5}, {4, 9}};
  res = {{1, 2}, {3, 9}};
  assert(mergeIntervals(intervals) == res);

  intervals = {{1, 5}, {2, 4}, {4, 9}};
  res = {{1, 9}};
  assert(mergeIntervals(intervals) == res);

  /////////////////////////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;
}
