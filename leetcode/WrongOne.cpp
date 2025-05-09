#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

bool lastPrintedWasEven = true;
bool someThreadaiting = false;
std::mutex mutex;
std::condition_variable cond;

void funcPrintOdd()
{
  for(int i = 1; i < 100; i+=2)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (!lastPrintedWasEven)
    {
      someThreadaiting = true;
      // 1. lock.unlock()
      // 2. Puase until notified from other thread
      // 3. lock.lock()
      cond.wait(lock);
      someThreadaiting = false;
    }

    std::cout << i << std::endl;
    lastPrintedWasEven = !lastPrintedWasEven;
    if(someThreadaiting)
    {
      lock.unlock();
      cond.notify_one();
      lock.lock();

      while (!lastPrintedWasEven)
      {
        lock.unlock();
        lock.lock();
      }
    }
  }
}

void funcPrintEven()
{
  for (int i = 2; i <= 100; i += 2)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (lastPrintedWasEven)
    {
      someThreadaiting = true;
      // 1. lock.unlock()
      // 2. Puase until notified from other thread
      // 3. lock.lock()
      cond.wait(lock);
      someThreadaiting = false;
    }

    std::cout << i << std::endl;
    lastPrintedWasEven = !lastPrintedWasEven;
    if (someThreadaiting)
    {
      lock.unlock();
      cond.notify_one();
      lock.lock();

      while (lastPrintedWasEven)
      {
        lock.unlock();
        lock.lock();
      }
    }
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