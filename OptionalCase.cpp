#include <iostream>
#include <math.h>
#include <stdint.h>

struct Point
{
  double x = 0;
  double y = 0;
};

double calcDistance(const Point& p1, const Point& p2)
{
  return sqrt(pow(p1.x-p2.x, 2)  + pow(p1.y-p2.y, 2));
}

double calcDistance(const Point* p1, const Point &p2)
{
  if (p1)
  {
    return calcDistance(*p1, p2);
  }
  else
  {
    Point p;
    return calcDistance(p, p2);
  }
}

double calcAvg(double* numbers, uint32_t len)
{

  double res = 0;
  if(len)
  {
    for(int i = 0; i < len; i++)
    {
      res += numbers[i];
    }
    res /= len;
  }

  return res;
}


class RefReturnExample
{
  int *val;
  

public:
  RefReturnExample() : val(new int(0))
  {}

  int& getValRef()
  {
    return *val; 
  }

  void reset(const int n)
  {
    int* temp = val;
    val = new int(n);
    delete temp;
  }

  void reAssign(const int& n)
  {
    *val = n;
  }
};

int main()
{
  RefReturnExample refReturnExample;

  int& ref = refReturnExample.getValRef();
  std::cout << ref << std::endl;
  
  refReturnExample.reAssign(3);
  std::cout << ref << std::endl;
  
  refReturnExample.reset(5);
  int &ref2 = refReturnExample.getValRef();

  std::cout << ref2 << std::endl;

    return 0;
}