#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <iostream>

std::condition_variable cond;
std::mutex mutex;

int lastPrinted = 0;
int numWaitingThreads = 0;

void func(int start, int stepSize, int numSteps)
{
  for (int i = 0; i < numSteps; ++i)
  {
    int toPrint = start + i * stepSize;
    std::unique_lock<std::mutex> lock(mutex);
    while (toPrint != lastPrinted + 1)
    {
      ++numWaitingThreads;
      cond.wait(lock);
      --numWaitingThreads;
    }

    std::cout << toPrint << std::endl;
    lastPrinted = toPrint;

    if (!numWaitingThreads)
      continue;

    do
    {
      if (numWaitingThreads > 1)
      {
        lock.unlock();
        cond.notify_all();
      }
      else
      {
        lock.unlock();
        cond.notify_one();
      }
      
      lock.lock();
    }
    while (lastPrinted == toPrint);
  }
}

int main()
{
  std::thread t1([]() { func(1, 5, 20); });
  std::thread t2([]() { func(2, 5, 20); });
  std::thread t3([]() { func(3, 5, 20); });
  std::thread t4([]() { func(4, 5, 20); });
  std::thread t5([]() { func(5, 5, 20); });
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  return 0;
}