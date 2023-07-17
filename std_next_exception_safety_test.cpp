#include <vector>
#include <iostream>
#include <exception>
#include <functional>
#include <algorithm>

int main() {
    std::vector<int> arr;
    arr.push_back(1);
    arr.push_back(2);

    std::vector<int> arr2;

    try {
        std::for_each(arr.begin(), std::next(arr.begin(), std::min((size_t)10, arr.size())), [&arr2](const int& elem) {
            arr2.push_back(elem);
        });

        for (const auto& elem : arr2) {
            std::cout << elem << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}