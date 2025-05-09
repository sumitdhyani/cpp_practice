#include <iostream>

void func1(int& x)
{
	x = 2;
}

void func2(int* x)
{
	*x = 2;
}

struct SampleClass
{
	int n;

	void print()
	{
		std::cout << n << std::endl;
	}

	SampleClass()
	{
		n = 5;
		std::cout << "SampleClass object getting created..." << std::endl;
	}

	~SampleClass()
	{
		n = 0;
		std::cout << "SampleClass object getting destroyed..." << std::endl;
	}
};


int main()
{
	SampleClass s = SampleClass();
	std::cout << "line 39" << std::endl;
	return 0;
}
