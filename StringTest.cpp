#include <iostream>
#include <string>
#include <variant>

bool f1()
{
	std::cout << "f1" << std::endl;
	return true;
}

bool f2()
{
	std::cout << "f2" << std::endl;
	return true;
}

bool f3()
{
	std::cout << "f3" << std::endl;
	return true;
}

int main()
{
	f1() && f2() && f3();
	return 0;
}
