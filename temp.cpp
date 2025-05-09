#include <iostream>
#include <vector>
#include <stdint.h>

uint32_t dc  = 0;
uint32_t pc  = 0;
uint32_t cc  = 0;
uint32_t mc  = 0;
uint32_t ao  = 0;
uint32_t mao = 0;
uint32_t dd  = 0;


struct X
{
    X(uint32_t x)
    {

      _x = x;
      ++pc;
      //std::cout << "param X::ctor " << _x << std::endl;
    }

    X()
    {

      _x = 0;
      ++dc;
      //std::cout << "default X::ctor" << std::endl;
    }

    X(const X& x)
    {
      _x = x._x;
      ++cc;
      //std::cout << "copy X::ctor " << _x << std::endl;
    }

    X(X&& x)
    {
      _x = x._x;
      x._x = 0;
      ++mc;
      //std::cout << "move X::ctor " << _x << std::endl;
    }

    const X& operator=(const X& x)
    {
      _x = x._x;
      ++ao;
      return *this;
      //std::cout << "move X::ctor " << _x << std::endl;
    }

    const X& operator=(X&& x)
    {
      _x = x._x;
      ++mao;
      return *this;
      //std::cout << "move X::ctor " << _x << std::endl;
    }

    ~X()
    {
      ++dd;
      //std::cout << "X::dtor " << _x << std::endl;
    }
    uint32_t _x;
};

#include <list>
int main() {
    std::list<X> v1 = {X(1), X(2), X(3)};
    std::list<X> v2 = {X(10), X(20)};
    printf("dc:%u\npc:%u\ncc:%u\nmc:%u\nao:%u\nmao:%u\ndd:%u\n", dc, pc, cc, mc, ao, mao, dd);

    std::cout << "Assigning v1 to v2..." << std::endl;
    v2 = v1;  // Assignment: old elements of v2 are destroyed, new elements are copied
    printf("dc:%u\npc:%u\ncc:%u\nmc:%u\nao:%u\nmao:%u\ndd:%u\n", dc, pc, cc, mc, ao, mao, dd);

    std::cout << "End of main function" << std::endl;
    return 0;
}