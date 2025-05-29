#include <memory>
#include <thread>
#include <functional>
#include <semaphore>
#include <iostream>

std::vector<std::unique_ptr<std::binary_semaphore>> semaphores;

void func(int start, int stepSize, int numSteps)
{
  for (int i = 0; i < numSteps; ++i)
  {
    semaphores[start - 1]->acquire();
    std::cout << start + i * stepSize << std::endl;
    semaphores[start % semaphores.size()]->release();
  }
}

int main()
{
  const uint32_t numParallelFunctions = 5;
  const uint16_t numSteps = 60;

  for (uint32_t i = 0; i < numParallelFunctions; ++i)
  {
    semaphores.push_back(std::make_unique<std::binary_semaphore>(0));
  }
  semaphores[0]->release();

  std::thread threads[numParallelFunctions];
  for (uint32_t i = 0; i < numParallelFunctions; ++i)
  {
    threads[i] = std::thread (std::bind(func, i+1, numParallelFunctions, numSteps));
  }

  for (uint32_t i = 0; i < numParallelFunctions; ++i)
  {
    threads[i].join();
  }

  return 0;
}