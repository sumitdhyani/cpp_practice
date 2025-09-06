#include <cassert>
#include <iostream>
#include <string>
#include <stdint.h>

using namespace std;
enum class ExitCondition
{
  FAILED,
  PASSED,
  NONE
};

bool isValidParenthesisEnclosedString(const std::string& str)
{
  ExitCondition exitCondition = ExitCondition::NONE;
  int parenthesisWeight = 0;
  bool stringAlreadyCompleted = false;
  int i = 0;
  do 
  {
    if (str[i] == '(')
      ++parenthesisWeight;
    else
      --parenthesisWeight;
  }
  while(++i < str.length() && parenthesisWeight > 0);

  if (!parenthesisWeight)
    return i == str.length();
  else
    return false;
}

int main()
{
  assert(isValidParenthesisEnclosedString("()"));
  assert(isValidParenthesisEnclosedString("(())"));
  assert(!isValidParenthesisEnclosedString("(())()"));
  /////////////////////////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;
}