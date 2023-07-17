#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <variant>

template <typename ...Args>
std::function<void(Args...)> operator +(const std::function<void(Args...)>& f1, const std::function<void(Args...)>& f2)
{
    return [f1, f2](Args... args) {
        f1(args...);
        f2(args...);
    };
}

template <typename Out, typename ...Args>
std::function<std::vector<Out>(Args...)> operator +(const std::function<Out(Args...)>& f1, const std::function<Out(Args...)>& f2)
{
    return [f1, f2](Args... args) {
        return std::vector<Out>{ std::move(f1(args...)), std::move(f2(args...)) };
    };
}

template <typename Out, typename ...Args>
std::function<std::vector<Out>(Args...)> operator +(const std::function<std::vector<Out>(Args...)>& f1, const std::function<Out(Args...)>& f2)
{
    return [f1, f2](Args... args) {
        auto res = std::move(f1(args...));
        res.push_back(std::move(f2(args...)));
        return res;
    };
}

template <typename Out, typename ...Args>
std::function<std::vector<Out>(Args...)> operator +(const std::function<Out(Args...)>& f1, const std::function<std::vector<Out>(Args...)>& f2)
{
    return [f1, f2](Args... args) {

        auto res1 = std::move(f1(args...));
        auto res2 = std::move(f2(args...));
        std::vector<Out> res;
        res.reserve(res1.size() + res2.size());
        res.insert(res.end(),
            std::make_move_iterator(res1.begin()),
            std::make_move_iterator(res1.end()));
        res.insert(res.end(),
            std::make_move_iterator(res2.begin()),
            std::make_move_iterator(res2.end()));
        return res;
    };
}

template <typename Out, typename ...Args>
std::function<std::vector<Out>(Args...)> operator +(const std::function<std::vector<Out>(Args...)>& f1, const std::function<std::vector<Out>(Args...)>& f2)
{
    return [f1, f2](Args... args) {
        auto res1 = f1(args...);
        auto res2 = f2(args...);
        res1.insert(res1.end(), res2.begin(), res2.end());
        return res1;
    };
}

template <typename Out>
std::function<Out(Out)> operator |=(std::function<Out(Out)>& f1, const std::function<Out(Out)>& f2)
{
    auto temp = f1;
    f1 = [f2, temp](Out param) {
        return f2(temp(param));
    };

    return f1;
}

template <typename Out1, typename Out2, typename ...Args>
std::function<Out2(Args... args)> operator |(const std::function<Out1(Args...)>& f1, const std::function<Out2(Out1)>& f2)
{
    return [f1, f2](Args... args) {
        return f2(f1(args...));
    };
}

template<typename Out, typename ...Args>
std::function<std::variant<bool, Out>(Args...)> operator &&(std::function<bool(Args...)> const& f1, const std::function<Out(Args...)> const& f2)
{
    return [f1, f2](Args... args) {
        std::variant<bool, Out> ret;
        if (f1(args...))
            ret = std::move(f2(args...));
        else
            ret = false;
        return ret;
    };
}

template<typename ...Args>
std::function<bool(Args...)> operator &&(std::function<bool(Args...)> const& f1, std::function<bool(Args...)> const& f2)
{
    return [f1, f2](Args... args) {
        return f1(args...) && f2(args...);
    };
}

template<typename ...Args>
bool operator &&(std::function<bool(Args...)> const& f1, std::function<void(Args...)> const& f2)
{
    return [f1, f2](Args... args) {
        if (f1(args...)) {
            f2(args...);
            return true;
        }
        return false;
    };
}

bool even(int n) { return !(n % 2); }
bool isDivBy10(int n) { return !(n % 10); }

int add(int n1, int n2)
{
    return n1 + n2;
}

int sub(int n1, int n2)
{
    return n1 - n2;
}

int mul(int n1, int n2)
{
    return n1 * n2;
}

int divide(int n1, int n2)
{
    return n1 / n2;
}

int sqr(int n)
{
    return n * n;
}

int half(int n)
{
    return n / 2;
}

bool isProductEven(int n1, int n2)
{
    return !((n1 * n2) % 2);
}

int isEven(int n)
{
    return !(n % 2);
}

struct GenericVisitor
{
    template<typename T>
    void operator()(T var)
    {
        std::cout << var << std::endl;
    }
};

typedef std::function<int(int, int)> BinaryFunc;
typedef std::function<int(int)> UnaryFunc;
typedef std::function<bool(int)> BoolFunc;
typedef std::function<std::string(int)> StringFunc;

int main()
{

    std::visit(GenericVisitor(), std::function<bool(int)>([](int n){ return !(n%2);}) && BoolFunc(isDivBy10) && UnaryFunc(sqr))(52));


    std::cout << (BinaryFunc(mul) | UnaryFunc(sqr) | UnaryFunc(half))(5, 6) << std::endl;
//
    //auto res = ((f1 + f2) + (f3 + f4))(20, 10);
    //for (const auto& elem : res)
    //    std::cout << elem << std::endl;

    return 0;
}

