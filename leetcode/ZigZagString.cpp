// https : // leetcode.com/problems/zigzag-conversion/description/
// The string "PAYPALISHIRING" is written in a zigzag pattern on a given number of rows like this: (you may want to display this pattern in a fixed font for better legibility)
// P   A   H   N
// A P L S I I G
// Y   I   R
// And then read line by line: "PAHNAPLSIIGYIR"
// Write the code that will take a string and make this conversion given a number of rows:
// string convert(string s, int numRows);
//
// Example 1 :
//      Input : s = "PAYPALISHIRING", numRows = 3
//      Output : "PAHNAPLSIIGYIR"
//
// Example 2 :
//     Input : s = "PAYPALISHIRING", numRows = 4
//      Output : "PINALSIGYAHRPI" Explanation :
//      P     I    N
//      A   L S  I G
//      Y A   H R
//      P     I
//
// Example 3 :
// Input : s = "A",
// numRows = 1 Output : "A"
//
// Constraints :
// 1 <= s.length <= 1000 s consists of English letters(lower - case and upper - case), ',' and '.'
// 1 <= numRows <= 1000

#include <cassert>
#include <vector>
#include <string>
using namespace std;

string convert(string s, int numRows)
{
  if (1 == numRows) return s;

  vector<string> zigZag(numRows, string());
  int direction = 1;
  for (int rowIdx = 0, strIdx = 0;
       strIdx < s.length();
       rowIdx += direction, direction = (rowIdx == 0 || rowIdx == numRows - 1) ? -direction : direction, ++strIdx)
  {
    zigZag[rowIdx] += s[strIdx];
  }

  string result;
  for (int i = 0, j = 0; i < numRows; i++)
  {
    for (auto const& ch : zigZag[i])
    {
      result += ch;
    }
  }

  return result;
}

int main()
{
  string s = "PAYPALISHIRING";
  assert(convert(s, 3) == "PAHNAPLSIIGYIR");
  assert(convert(s, 4) == "PINALSIGYAHRPI");
  return 0;
}