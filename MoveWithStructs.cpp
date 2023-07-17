#include <iostream>
#include <vector>
#include <functional>
#define PRINT(str) std::cout << str << std::endl

struct C
{

    C(){ PRINT("C::C()");}
  
    C(const C& other){ PRINT("C::C(const C&)");}

    const C& operator=(const C& other){ PRINT("C::operator=(const C&)");}

    C(C&& other){ PRINT("C::C(C&&)");}
};

int add(int n1, int n2){ return n1 + n2;}

int main()
{
    PRINT(std::function<int(int, int)>(add)(2, 3));
    std::vector<C> v1{C(), C(), C(), C(), C()};
    std::vector<C> v2{C(), C(), C(), C(), C()};

    PRINT("=======================");
    v1.insert(v1.end(), std::make_move_iterator(v2. begin()) , std::make_move_iterator(v2.end()));

    return 0;
}
