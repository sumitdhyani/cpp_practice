#include <iostream>
#include <functional>

template <typename ...Args>
struct NullableFunction
{
    NullableFunction(const std::function<void(Args...)>& func) : m_func(func){}
    NullableFunction(const std::function<void(Args...)>&& func) : m_func(func){}

    const NullableFunction& operator =(const std::function<void(Args...)>& func)
    {
        m_func = func;
    }

    const NullableFunction& operator =(const std::function<void(Args...)>&& func)
    {
        m_func = func;
    }

    NullableFunction(){}

    void operator()(Args... args){
        try{
            m_func(args...);
        }catch(const std::bad_function_call&){}
    }

private:
    std::function<void(Args...)> m_func;
};

int main()
{
    NullableFunction<int> f;
    f(1);
    return 0;
}