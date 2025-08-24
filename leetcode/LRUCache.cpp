#include <cassert>
#include <iostream>
#include <string.h>
#include <vector>
#include <tuple>
#include <stdexcept>
#include <unordered_map>
#include <list>

// Key value pair
using tuple = std::tuple<int, int>;

using KeyValVector = std::vector<tuple>;

class LRUBook
{

  std::list<int> m_keys;
  std::unordered_map<int, std::list<int>::iterator> m_keyToNode;

  public:
  void used(int key)
  {
    if (auto it = m_keyToNode.find(key); it != m_keyToNode.end())
    {
      m_keys.erase(it->second);
      m_keyToNode.erase(it);
    }

    m_keys.push_back(key);
    m_keyToNode[key] = std::prev(m_keys.end());
  }

  void deleteLRUKey()
  {
    if (m_keys.empty()) throw std::runtime_error("Book is empty");
    
    int lruKey = *m_keys.begin();
    m_keys.erase(m_keys.begin());
    m_keyToNode.erase(lruKey);
  }

  int getLRUKey()
  {
    if (m_keys.empty()) throw std::runtime_error("Book is empty");
    return *m_keys.begin();
  }
};

class LRUCache
{
  
  int m_capacity;
  std::unordered_map<int, int> m_keyToVal;
  LRUBook m_lruBook;

  bool full()
  {
    return m_keyToVal.size() == m_capacity;
  }

public:

  LRUCache(int capacity) : m_capacity(capacity)
  {
    if(0 >= capacity) throw std::runtime_error("Capacity should be a +ve integer");

    m_keyToVal.reserve(capacity);
  }

  int get(int key)
  {
    if (auto it = m_keyToVal.find(key); it != m_keyToVal.end())
    {
      m_lruBook.used(key);
      return it->second;
    } 

    throw std::runtime_error("Invalid key!");
  }

  // Will replace the old value if key is already there
  void put(int key, int value)
  {
    if (full())
    {
      int lruKey = m_lruBook.getLRUKey();
      m_keyToVal.erase(lruKey);
      m_lruBook.deleteLRUKey();
    }

    m_keyToVal[key] = value;
  }
};

void insertValues(LRUCache& cache, const KeyValVector& arr)
{
  for (auto const& [key, val]: arr)
  {
    cache.put(key, val);
  }
}

int main()
{

  LRUCache cache(2);
  insertValues(cache, KeyValVector{{2, 1}, {1, 2}});

  assert(cache.get(1) == 2);
  assert(cache.get(2) == 1);
  insertValues(cache, KeyValVector{{4, 3}});

  assert(cache.get(4) == 3);
  assert(cache.get(2) == 1);

  insertValues(cache, KeyValVector{{3, 4}});

  try
  {
    cache.get(4); // This line should throw
    throw std::runtime_error("Dummy");
  }
  catch(const std::runtime_error& ex)
  {
    // Ensure we got this error by fetching the abscent key 4
    assert(strcmp(ex.what(), "Dummy") != 0);
  }

  assert(cache.get(2) == 1);
  assert(cache.get(3) == 4);

  /////////////////////////////////////////////////////////////////
  std::cout << "All tests passed!" << std::endl;
  return 0;
}