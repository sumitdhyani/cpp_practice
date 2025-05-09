#include <memory>
#include <optional>
#include <stack>
#include <unordered_set>
#include <optional>
#include <cstdlib>

template<class T>
struct MemPool
{
  typedef T value_type;
  MemPool(unsigned short blockCount) : 
  _blockCount(blockCount)
  {
    size_t blobSize = sizeof(T) * blockCount;
    _occupied.reserve(blobSize);
    _memBlob = (T*)malloc(blobSize);
    for (unsigned short i = 0; i * sizeof(T) < blobSize; i++) {
      _free.push(i);
    }
  }
  
  MemPool() {
    MemPool(3200);
  }

  T* allocate(unsigned short size) {
    if (_free.empty()) {
      return nullptr;
    }

    unsigned short nextAvailableIndex = _free.top();
    _free.pop();
    _occupied.insert(nextAvailableIndex);
    return _memBlob + nextAvailableIndex; 
  }

  void deallocate(T* ptr, size_t n) {
    auto rawOffset = (char*)ptr - (char*)_memBlob;
    if (0 != rawOffset % _blockSize) {
      return;
    }

    auto indexOffset = ptr - _memBlob;
    if (auto it = _occupied.find(indexOffset); it != _occupied.end()) {
      _occupied.erase(it);
      _free.push(indexOffset);
    } 
  }

  ~MemPool() {
    free(_memBlob);
  }

  //MemPool() = delete;
  MemPool(const MemPool&) = delete;
  const MemPool& operator=(const MemPool&) = delete;

  private:
  std::stack<unsigned short> _free;
  std::unordered_set<unsigned short> _occupied;
  T* _memBlob;
  short _blockCount;
};