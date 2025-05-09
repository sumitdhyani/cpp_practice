#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <vector>
#include <tuple>
#include <stdint.h>

using time_point = std::chrono::system_clock::time_point;

std::mutex mutex;
std::condition_variable cond;
bool processorWaiting = false;
std::map<time_point, std::tuple<time_point, std::string>, std::less_equal<time_point>> taskMap;

void push(const time_point& readTime,
          const time_point& targetTime,
          const std::string& str)
{
  std::unique_lock<std::mutex> lock(mutex);
  taskMap.emplace(targetTime, std::make_tuple(readTime, str));
  while (!processorWaiting)
  {
    lock.unlock();
    lock.lock();
  }
}

void process()
{
  std::unique_lock<std::mutex> lock(mutex);
  for (auto it = taskMap.begin();
       !taskMap.empty() && it->first >= std::chrono::system_clock::now();
       taskMap.erase(it), it = taskMap.begin())
  {
    auto const &[time, timedString] = *it;
    auto const &[readTime, str] = timedString;
    std::cout << readTime.time_since_epoch().count() << " " << time.time_since_epoch().count() << " " << str << std::endl;
  }

  processorWaiting = true;
  if (!taskMap.empty())
  {
    cond.wait_until(lock, taskMap.begin()->first);
  }
  else
  {
    cond.wait(lock);
  }
  processorWaiting = false;
}

int main()
{
  int numTestCases;
  std::cin >> numTestCases;

  for (int i = 0; i < numTestCases; ++i)
  {
    uint16_t delay;
    char buffer[1024];
    std::cin >> delay >> buffer;

    auto now = std::chrono::system_clock::now();
    taskMap.emplace(now + std::chrono::seconds(delay), std::make_tuple(now, buffer));
  }

  for (auto it = taskMap.begin(); it != taskMap.end(); it = taskMap.begin())
  {
    auto const &[time, timedString] = *it;
    auto now = std::chrono::system_clock::now();
    if (time > now)
    {
      std::this_thread::sleep_until(time);
    }

    auto const &[readTime, str] = timedString;
    std::cout << readTime.time_since_epoch().count() << " " << time.time_since_epoch().count() << " " << str << std::endl;
    taskMap.erase(it);
  }
}
