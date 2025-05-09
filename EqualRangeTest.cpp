#include <iostream> 
#include <algorithm> 
#include <string> 
#include <vector> 
#include <functional>

struct Employee {
    size_t id;
    std::string name;
    size_t age;
};

void print(const Employee& emp) {
    std::cout << emp.id << " : " << emp.name.c_str() << " : " << emp.age << std::endl; 
}

int main() {
    std::vector<Employee> employees{{1, std::string("Ram"), 25}, {2, std::string("Syham"), 25}, {3, std::string("Gopal"), 30}, {4, "Ravan", 30}};
    auto const& range = std::equal_range(employees.begin(),
                                  employees.end(),
                                  Employee{1, "1", 25},
                                  [](const Employee& e1, const Employee& e2) {
                                    return e1.age < e2.age;
                                  });

    auto const& uniqueRange = std::unique(employees.begin(), employees.end(), [](const Employee& e1, const Employee& e2) {
        return e1.age == e2.age? 1 : 0;
    });

    for (auto it = range.first; it != range.second; ++it) {
        print(*it);
    }

    std::cout << "=============================================================";
    for (auto it = uniqueRange.first; it != uniqueRange.second; ++it) {
        print(*it);
    }

            
    return 0;
}