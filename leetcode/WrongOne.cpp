#include <iostream>
#include <thread>
#include <semaphore>

std::binary_semaphore printOdd(1);
std::binary_semaphore printEven(0);
void funcPrintOdd()
{
  for(int i = 1; i < 100; i+=2)
  {
    printOdd.acquire();
    std::cout << i << std::endl;
    printEven.release();
  }
}

void funcPrintEven()
{
  for (int i = 2; i <= 100; i += 2)
  {
    printEven.acquire();
    std::cout << i << std::endl;
    printOdd.release();
  }
}
int main()
{
  std::thread t1(funcPrintEven);
  std::thread t2(funcPrintOdd);
  
  t1.join();
  t2.join();

  return 0;
}