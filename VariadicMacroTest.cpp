#include <iostream>
template <class T>
void func(const T& val) {
    std:::cout << val << std::endl;
}

template <class T, class... Args>
void func(const T& val, Args... args) {
    std::cout << val << std::endl;
    func(args...);
}

#define PRINT(arg1, ...) printf(arg1, __VA_ARGS__);

int main() {
    PRINT("%d, %f, %d", 2, 6.5, 4);
    return 0;
}