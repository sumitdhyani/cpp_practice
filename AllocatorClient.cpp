#include "CustomAllocator.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <cstddef>
#include <memory>
#include <iostream>

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

    FreeListAllocator() = default;

    ~FreeListAllocator() = default;

    T* allocate(std::size_t n) {
      std::cout << "Requested "<< n << " objects" << std::endl;
      if (auto idx = m_freePtrList.getNextFreeIdx(n); idx.has_value())
      {
        T* ret = reinterpret_cast<T*>(m_pool + idx.value() * sizeof(T));
        std::cout << "Returned " << n << " objects starting at idx: " << (ret - reinterpret_cast<T*>(m_pool))  << std::endl;
        return ret;
      }
      else
      {
        return nullptr;
      }
    }

    void deallocate(T* p, std::size_t n) noexcept {
      const std::size_t idx = p - reinterpret_cast<T*>(m_pool);
      std::cout << "Freed memory of size: " << n << " at idx: " << idx << std::endl;
      m_freePtrList.freeIdx(idx);
    }

    bool operator==(const FreeListAllocator& other) const noexcept {
        return this == &other;
    }

    bool operator!=(const FreeListAllocator& other) const noexcept {
        return !(*this == other);
    }

private:
    char m_pool[size*sizeof(T)];
    FreeResourcetable<size> m_freePtrList;
};

int main()
{

  // {
  //   std::vector<int, FreeListAllocator<int, 1024>> vec;

  //   for(int i = 0; i < 100; i++)
  //   {
  //     std::cout << "Pushing " << i << " to the vector" << std::endl;
  //     vec.push_back(i);
  //   }
  // }

  {
    std::set<int, std::less<int>, FreeListAllocator<int, 1024>> set;
    for(int i = 0; i < 100; i++)
    {
      std::cout << "Pushing " << i << " to the set" << std::endl;
      set.insert(i);
    }

  }

  std::cout << "Programm exiting..." << std::endl << std::endl << std::endl;

  return 0;
}
