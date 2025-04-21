// https://leetcode.com/problems/count-prefix-and-suffix-pairs-i/?envType=daily-question&envId=2025-01-08
// 3042. Count Prefix and Suffix Pairs I
// Easy
// Topics
// Companies
// Hint
// You are given a 0-indexed string array words.

// Let's define a boolean function isPrefixAndSuffix that takes two strings, str1 and str2:

// isPrefixAndSuffix(str1, str2) returns true if str1 is both a prefix and a suffix of str2, and false otherwise.
// For example, isPrefixAndSuffix("aba", "ababa") is true because "aba" is a prefix of "ababa" and also a suffix, but isPrefixAndSuffix("abc", "abcd") is false.

// Return an integer denoting the number of index pairs (i, j) such that i < j, and isPrefixAndSuffix(words[i], words[j]) is true.

#include <cassert>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

int countPrefixSuffixPairs(vector<string> &words)
{
  if (words.size() <= 1) return 0;

  int res = 0;
  for(int i = 0; i < words.size() - 1; i++)
  {
    for (int j = i+1; j < words.size(); j++)
    {
      if (words[j].length() >= words[i].length() &&
          words[j].find(words[i]) == 0 &&
          words[j].rfind(words[i]) == words[j].length() - words[i].length())
      {
        ++res;
      }
    }
  }

  return res;
}

  int main()
  {
    vector<string> input{"Hello"};
    assert(countPrefixSuffixPairs(input) == 0);

    input = {"aba", "ababa"};
    assert(countPrefixSuffixPairs(input) == 1);

    input = {"aba", "ababab"};
    assert(countPrefixSuffixPairs(input) == 0);

    input = {"aba", "ababa", "ababababa"};
    //std::cout << countPrefixSuffixPairs(input) << std::endl;
    assert(countPrefixSuffixPairs(input) == 3);

    return 0;
  }