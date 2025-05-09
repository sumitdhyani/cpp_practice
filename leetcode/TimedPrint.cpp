#include <iostream>
#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <tuple>
#include <stdint.h>

int main()
{
  int numTestCases;
  std::cin >> numTestCases;
  using time_point = std::chrono::system_clock::time_point;
  std::map<time_point, std::tuple<time_point,std::string>, std::less_equal<time_point>> taskMap;

  for(int i = 0; i < numTestCases; ++i)
  {
    uint16_t delay;
    char buffer[1024];
    std::cin >> delay >> buffer;

    auto now = std::chrono::system_clock::now();
    taskMap.emplace(now + std::chrono::seconds(delay), std::make_tuple(now, buffer));
  }

  for (auto it = taskMap.begin(); it != taskMap.end(); it = taskMap.begin())
  {
    auto const& [time, timedString] = *it;
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
