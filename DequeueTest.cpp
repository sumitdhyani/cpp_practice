#include <iostream>
#include <deque>

int main()
{
    std::deque<int> deque;
    deque.push_back(1);
    deque.push_back(2);
    deque.push_front(3);
    deque.push_back(4);

    while(!deque.empty())
    {
        std::cout << deque.front() << std::endl;
        deque.pop_front();
    }
    return 0;
}