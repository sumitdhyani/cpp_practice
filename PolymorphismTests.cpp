#include <iostream>
#include <stdint.h>

class Base1
{
  virtual void print()
  {
    std::cout << "Base1::print" << std::endl;
  }
};

class Base2
{
  virtual void print()
  {
    std::cout << "Base2::print" << std::endl;
  }
};

class Derived : public Base1, public Base2
{
  virtual void print()
  {
    std::cout << "Derived::print" << std::endl;
  }
};

int main()
{
  Derived* d = new Derived();
  Base1* p1 = d;
  Base2* p2 = d;

  std::cout << reinterpret_cast<uint64_t>(d) << std::endl;
  std::cout << reinterpret_cast<uint64_t>(p1) << std::endl;
  std::cout << reinterpret_cast<uint64_t>(p2) << std::endl;

  return 0;
}
