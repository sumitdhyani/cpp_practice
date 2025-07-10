#include "CustomAllocator.hpp"
//#include "CustomAllocatorSegmentTree.hpp"
#include <vector>
#include <map>
#include <list>
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


//Everything on stack
template <class T, std::size_t size, class ResourceTable>
class FreeListAllocator_V1 {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <class U>
    struct rebind {
        using other = FreeListAllocator_V1<U, size, ResourceTable>;
    };

    FreeListAllocator_V1() = default;

    ~FreeListAllocator_V1() = default;

    T* allocate(std::size_t n) {
      //std::cout << "Requested "<< n << " objects" << std::endl;
      if (auto idx = m_freePtrList.getNextFreeIdx(n); idx.has_value())
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
      m_freePtrList.freeIdx(idx);
    }

    bool operator==(const FreeListAllocator_V1& other) const noexcept {
        return this == &other;
    }

    bool operator!=(const FreeListAllocator_V1& other) const noexcept {
        return !(*this == other);
    }

private:
    char m_pool[size*sizeof(T)];
    ResourceTable m_freePtrList;
};

template <class T, std::size_t size, class ResourceTable>
class FreeListAllocator_V2
{
public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  template <class U>
  struct rebind
  {
    using other = FreeListAllocator_V2<U, size, ResourceTable>;
  };

  FreeListAllocator_V2() : m_pool((char*)malloc(size * sizeof(T)))
  {}

  ~FreeListAllocator_V2()
  {
    free(m_pool);
  }

  T *allocate(std::size_t n)
  {
    // std::cout << "Requested "<< n << " objects" << std::endl;
    if (auto idx = m_freePtrList.getNextFreeIdx(n); idx)
    {
      T *ret = reinterpret_cast<T *>(m_pool + idx.value() * sizeof(T));
      // std::cout << "Returned " << n << " objects starting at idx: " << (ret - reinterpret_cast<T*>(m_pool))  << std::endl;
      return ret;
    }
    else
    {
      return nullptr;
    }
  }

  void deallocate(T *p, std::size_t n) noexcept
  {
    const std::size_t idx = p - reinterpret_cast<T *>(m_pool);
    // std::cout << "Freed memory of size: " << n << " at idx: " << idx << std::endl;
    m_freePtrList.freeIdx(idx);
  }

  bool operator==(const FreeListAllocator_V2 &other) const noexcept
  {
    return this == &other;
  }

  bool isInRange(char* ptr) const noexcept
  {
    return ptr >= m_pool && ptr < m_pool + size * sizeof(T);
  }

  bool operator!=(const FreeListAllocator_V2 &other) const noexcept
  {
    return !(*this == other);
  }

private:
  char* m_pool;
  ResourceTable m_freePtrList;
};

template <class T, class AllocType>
struct LinkedListAllocator
{
  struct AllocNode
  {
    AllocNode(AllocNode *m_next) : m_next(m_next) {}
    AllocNode() : m_next(nullptr) {}

    T* allocate()
    {
      return m_alloc.allocate();
    }

    void deallocate(T* p, std::size_t n) noexcept
    {
      m_alloc.deallocate(p, n);
    }

    bool isInRange(char* ptr) const noexcept
    {
      return m_alloc.isInRange(ptr);
    }

    AllocType m_alloc;
    AllocNode* m_next;
  };

  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  template <class U>
  struct rebind
  {
    using other = LinkedListAllocator<U, AllocType>;
  };

  LinkedListAllocator() : m_start(new AllocNode())
  {}

  ~LinkedListAllocator()
  {
    AllocNode *current = m_start;
    while (current)
    {
      AllocNode *next = current->m_next;
      delete current;
      current = next;
    }
  }

  T *allocate(std::size_t n)
  {

    // std::cout << "Requested "<< n << " objects" << std::endl;
    T* ret = nullptr;
    AllocNode *current = m_start;
    while (current && !(ret == current->allocate()))
    {
      current = current->m_next;
    }
    
    if (!ret)
    {
      m_start = new AllocNode(m_start);
      ret = m_start->allocate();
    }

    return ret;
  }

  void deallocate(T *p, std::size_t n) noexcept
  {
    AllocNode *current = m_start;
    while (!current->isInRange(reinterpret_cast<char*>(p)))
    {
      current = current->m_next;
    }

    current->deallocate(p, n);
  }

  bool operator==(const LinkedListAllocator &other) const noexcept
  {
    return this == &other;
  }

  bool operator!=(const LinkedListAllocator &other) const noexcept
  {
    return !(*this == other);
  }

private:
  AllocNode *m_start;
};
class Temple
{
  char str[256];
};
#define TWO_POWER 12


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

#define POOL_SIZE (Size(1) << (TWO_POWER + 1))

#define NUM_ELEMENTS (Size(1) << (TWO_POWER - 1))


void demoDefaultAllocatorVector(Size numElements)
{
    std::vector<Temple> vec;
    for (Size i = 0; i < numElements; i++)
    {
        vec.push_back(Temple());
    }
}

void demoCustomAllocatorVector(Size numElements)
{
    std::vector<Temple, FreeListAllocator_V2<Temple, POOL_SIZE, FreeResourcetable_Heap<POOL_SIZE>>> vec;
    for (Size i = 0; i < numElements; i++)
    {
        vec.push_back(Temple());
    }
}

void demoCustomAllocatorMap(Size numElements)
{
  std::map<int, Temple, std::less<int>, FreeListAllocator_V2<std::pair<const int, Temple>, POOL_SIZE, FreeResourcetable_Heap<POOL_SIZE>>> myMap;
  for (Size i = 0; i < numElements; i++)
  {
    myMap[i] = Temple();
  }

  // while(!myMap.empty())
  // {
  //   myMap.erase(rand() % numElements);
  // }

  for (Size i = 0; i < numElements; i++)
  {
    //myMap.erase(rand() % numElements);
    myMap.erase(numElements - i -1);
  }
}

void demoDefaultAllocatorMap(Size numElements)
{
  std::map<int, Temple> myMap;
  for (Size i = 0; i < numElements; i++)
  {
    myMap[i] = Temple();
  }

  while (!myMap.empty())
  {
    myMap.erase(rand() % numElements);
  }

  for (Size i = 0; i < numElements; i++)
  {
    // myMap.erase(rand() % numElements);
    myMap.erase(numElements - i - 1);
  }
}

void demoCustomAllocatorList(Size numElements)
{
  std::list<Temple, FreeListAllocator_V2<Temple, POOL_SIZE, FreeResourcetable_Heap<POOL_SIZE>>> myList;
  for (Size i = 0; i < numElements; i++)
  {
    myList.push_back(Temple());
  }

  for (Size i = 0; i < numElements; i++)
  {
    myList.pop_front();
  }
}

void demoDefaultAllocatorList(Size numElements)
{
  std::list<Temple> myList;
  for (Size i = 0; i < numElements; i++)
  {
    myList.push_back(Temple());
  }

  for (Size i = 0; i < numElements; i++)
  {
    myList.pop_front();
  }
}


int main()
{
  std::cout << "Custom Allocator Performance Test" << std::endl;
  std::cout << "=================================" << std::endl;
  std::cout << "Pool size: " << POOL_SIZE << " bytes" << std::endl;
  std::cout << "Number of elements to insert: " << NUM_ELEMENTS << std::endl;
  std::cout << "Size of Temple object: " << sizeof(Temple) << " bytes" << std::endl;

  auto start = Clock::now();
  //demoCustomAllocatorVector(NUM_ELEMENTS);
  //demoCustomAllocatorMap(NUM_ELEMENTS);
  demoCustomAllocatorList(NUM_ELEMENTS);
  auto end = Clock::now();

  std::cout << "Custom allocator performance test completed." << std::endl;
  std::cout << "Time taken to insert " << NUM_ELEMENTS << " elements: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
            << " nanoseconds" << std::endl;


  std::cout << std::endl << "Now testing with default allocator..." << std::endl;
  std::cout << "====================================" << std::endl;
  std::cout << "Pool size: " << POOL_SIZE << " bytes" << std::endl;
  std::cout << "Number of elements to insert: " << NUM_ELEMENTS << std::endl;
  std::cout << "Size of Temple object: " << sizeof(Temple) << " bytes" << std::endl;
  
  // Measure time for inserting elements using default allocator
  start = Clock::now();
  //demoDefaultAllocatorVector(NUM_ELEMENTS);
  //demoDefaultAllocatorMap(NUM_ELEMENTS);
  demoDefaultAllocatorList(NUM_ELEMENTS);
  end = Clock::now();

  std::cout << "Default allocator performance test completed." << std::endl;
  std::cout << "Time taken to insert " << NUM_ELEMENTS << " elements: "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
            << " nanoseconds" << std::endl;

  return 0;
}
