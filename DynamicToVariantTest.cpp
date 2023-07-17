#include <iostream>
#include <variant>
#include <memory>
struct B{};

struct D1 : B{};
struct D2 : B{};

int main()
{
    auto p = std::shared_ptr<B>(new D1());
    auto var = std::dynamic_pointer_cast<std::variant<D1,D2>&>(*p);
    if(nullptr == var)
    {
        std::cout << "Fail" << std::endl;
    }
    else
    {
        std::cout << "Success" << std::endl;
    }

    return 0;
}