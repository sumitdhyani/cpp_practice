#include "CustomAllocator.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <cstddef>
#include <memory>
#include <iostream>

int totalDefaultConstructions = 0;
int totalCopyConstructions = 0;
int totalAssignments = 0;
int totalMoves = 0;

class Sample
{
  char m_buff[1024];
  public:
  Sample()
  {
    std::cout << "Sample()" << std::endl;
    ++totalDefaultConstructions;
  }

  Sample(const Sample& other)
  {
    std::cout << "Sample(const Sample&)" << std::endl;
    ++totalCopyConstructions;
  }

  Sample(Sample&& other)
  {
    std::cout << "Sample(Sample&&)" << std::endl;
    ++totalMoves;
  }

  const Sample& operator=(const Sample& other)
  {
    std::cout << "Sample& operator=(const Sample&)" << std::endl;
    ++totalAssignments;
    return *this;
  }

  const Sample& operator=(Sample&& other)
  {
    std::cout << "Sample& operator=(Sample&&)" << std::endl;
    ++totalMoves;
    return *this;
  }
};

template <class T, std::size_t size>
class FreeListAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <class U>
    struct rebind {
        using other = FreeListAllocator<U, size>;
    };

    FreeListAllocator()
    {
      m_freePtrList= new FreeResourcetable<size>();
    }

    ~FreeListAllocator() = default;

    T* allocate(std::size_t n) {
      //std::cout << "Requested "<< n << " objects" << std::endl;
      if (auto idx = m_freePtrList->getNextFreeIdx(n); idx.has_value())
      {
        T* ret = reinterpret_cast<T*>(m_pool + idx.value() * sizeof(T));
        //std::cout << "Returned " << n << " objects starting at idx: " << (ret - reinterpret_cast<T*>(m_pool))  << std::endl;
        return ret;
      }
      else
      {
        return nullptr;
      }
    }

    void deallocate(T* p, std::size_t n) noexcept {
      const std::size_t idx = p - reinterpret_cast<T*>(m_pool);
      //std::cout << "Freed memory of size: " << n << " at idx: " << idx << std::endl;
      m_freePtrList->freeIdx(idx);
    }

    bool operator==(const FreeListAllocator& other) const noexcept {
        return this == &other;
    }

    bool operator!=(const FreeListAllocator& other) const noexcept {
        return !(*this == other);
    }

private:
    char m_pool[size*sizeof(T)];
    FreeResourcetable<size>* m_freePtrList;
};

class Temple
{
  char str[256];
};


constexpr Size getPoolSize(const Size size)
{
  Size currMask = Size(1) >> (sizeof(Size) - 3);
  while(currMask && !(currMask & size))
  {
    currMask = currMask >> 1;
  }

  return currMask << 2;
}

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#define TWO_POWER 13
#define POOL_SIZE Size(1)<< (TWO_POWER + 1)

#define NUM_ELEMENTS (Size(1) << (TWO_POWER - 1))


int main()
{
  {

    auto start_time = Clock::now();
    std::vector<Temple, FreeListAllocator<Temple, POOL_SIZE >> vec;
    
    for(int i = 0; i < NUM_ELEMENTS; i++)
    {
      //std::cout << "Pushing " << i << "th element to the vector" << std::endl;
      vec.push_back(Temple());
    }
    
    auto end_time = Clock::now();

    std::cout << "For objects of size " << sizeof(Temple) <<", in a vector, cost to insert " << NUM_ELEMENTS <<" elements with Custom allocator:    "<< std::chrono::duration_cast<std::chrono::nanoseconds>(end_time    - start_time).count() << " nanoseconds" << std::endl;
  }

  {

    auto start_time = Clock::now();
    std::vector<Temple> vec;
    vec.reserve(NUM_ELEMENTS);
    
    for(int i = 0; i < NUM_ELEMENTS; i++)
    {
      //std::cout << "Pushing " << i << "th element to the vector" << std::endl;
      vec.push_back(Temple());
    }

    auto end_time = Clock::now();

    std::cout << "For objects of size " << sizeof(Temple) <<", in a vector, cost to insert " << NUM_ELEMENTS <<" elements with Default allocator:   "<< std::chrono::duration_cast<std::chrono::nanoseconds>(end_time    - start_time).count() << " nanoseconds" << std::endl;
  }

  std::cout << std::endl << "Programm exiting..." << std::endl << std::endl << std::endl;

  return 0;
}
