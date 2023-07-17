#include<iostream>
#define PRINT(str) std::cout << str << std::endl

struct C
{
    C(){ PRINT("C::C()");}
  
    C(const C& other){ PRINT("C::C(const C&)");}

    C(C&& other){ PRINT("C::C(C&&)");}
};

struct B : C
{
    B(){ PRINT("B::B()");}
  
    //B(const B& other){ PRINT("B::B(const B&)");}

    //B(B&& other){ PRINT("B::B(B&&)");}
    //B(B&& other) = default;

    //C c;
};

struct D : B
{
    D(){ PRINT("D::D()");}
  
    //D(const D& other){ PRINT("D::D(const D&)");}

    //D(D&& other) = default;//{ PRINT("D::D(D&&)");}
    //C c2;
};

int main()
{
    D d1;
    std::cout << "====================================" << std::endl;
    D d2(d1);

    return 0;
}