#include <cstdlib> 
#include <cstdint> 
#include <iostream> 
#include <vector> 
#include <cstring> 
#include <functional> 
 
template<uint32_t size> 
struct FreeResourcetable 
{ 
  constexpr static const uint32_t uint32_t_max = uint32_t(-1); 
    
  FreeResourcetable() :
    m_freeNodes({0,0}),
    m_occupiedNodes({0,0}),
    m_firstFreeIdx(0),
    m_firstOccupiedIdx(uint32_t_max)
  { 
      Node& first = m_freeNodes[0]; 
      Node& last = m_freeNodes[size-1]; 

      first.m_linkIdx = uint32_t_max; 
      first.m_size = size; 
        
      last.m_linkIdx = uint32_t_max; 
      last.m_size = size;
  }
    
  //Expected: currNode[start].m_size >= len 
  void shrink(Node* nodeArr,
              uint32_t prevIdx,
              uint32_t nextIdx,
              uint32_t start,
              uint32_t end,
              uint32_t len,
              uint32_t& firstIdx)
  { 
      Node& startNode = nodeArr[start];
      Node& endNode = nodeArr[end];
      uint32_t sectionSize = startNode.m_size;

      if (sectionSize == len)
      {
        if (prevIdx != uint32_t_max)
        {
          nodeArr[prevIdx].m_linkId = nextIdx;
        }

        if (nextIdx != uint32_t_max)
        {
          nodeArr[nextIdx].m_linkId = prevIdx;
        }

        memset(&startNode, 0, sizeof(startNode));
        memset(&endNode, 0, sizeof(endNode));

        if (start == firstIdx)
        {
          firstIdx = uint32_t_max;
        }
      }
      else
      {
        startNode.m_size =
        endNode.m_size = sectionSize - len;


        //The currNode is moved forward by 'len'
        const uint32_t newPos = start + len;
        Node& newNode = nodeArr[newPos];
        newNode = startNode;
        memset(&startNode, 0, sizeof(startNode));
        
        if(start == firstIdx)
        { 
            firstIdx = newPos;
        } 
        else if (newNode.m_linkIdx != uint32_t_max)
        { 
            nodeArr[newNode.m_linkIdx].m_linkIdx = newPos;
        }
      }
  }

  uint32_t getNextFreeIdx(uint32_t len) 
  { 
      // No free section available, return uint32_t_max, should be considered as
      // an error case by the caller
      if (uint32_t_max == m_firstFreeIdx)
      { 
          return uint32_t_max;
      }
      
      for(uint32_t currFreeIndex = m_firstFreeIdx;
          currFreeIndex != uint32_t_max;
          currFreeIndex = m_freeNodes[currFreeIndex + m_freeNodes[currFreeIndex].m_size -1].m_linkIdx)
      {
          const Node& currNode = m_freeNodes[currFreeIndex];
          const uint32_t pairedIdx = currFreeIndex + currNode.m_size - 1;
          const Node& pairedNode = m_freeNodes[pairedIdx];

          if (currNode.m_size >= len)
          { 
              Node
              shrink(m_freeNodes,
                     currNode.m_linkIdx,
                     pairedNode.m_linkIdx,
                     currFreeIndex,
                     currFreeIndex,
                     pairedIdx,
                     m_firstFreeIdx);

              addSection(currFreeIndex, len, newNode.m_linkIdx, m_freeNodes[newPos + len -1] );
              return currFreeIndex;
          }
      }

      return uint32_t_max;

  }

  bool freeIdx(const uint32_t idx)
  {
// To be implemented
    
  }

  private:

  std::tuple<uint32_t, uint32_t> getPrevAndNext(Node* nodeArray,
                                                uint32_t start,
                                                uint32_t len,
                                                uint32_t firstIdx)
  {
    uint32_t end = start + len - 1;
    
    if (uint32_t_max == firstIdx)
    {
      return {uint32_t_max, uint32_t_max};
    }
    else if (start < firstIdx)
    {
      return {uint32_t_max, firstIdx};
    }

    uint32_t prev = uint32_t_max;
    uint32_t next = uint32_t_max;


    bool exit = false;
    for (uint32_t currIdx = firstIdx;
         !exit;
         currIdx = nodeArray[currIdx + nodeArray[currIdx].m_size -1].m_linkIdx)
    {
      const uint32_t pairedIdx = currIdx + nodeArray[currIdx].m_size -1;
      const Node& pairedNode = nodeArray[pairedIdx];
      const uint32_t nextSectionStart = pairedNode.m_linkIdx;

      if(uint32_t_max == nextSectionStart)
      {
        prev = currIdx;
        exit = true;
      }
      else if(nextSectionStart > start)
      {
        prev = pairedIdx;
        next = nextSectionStart;
        exit = true;
      }
    }

    return {prev, next};
  }

  void addSection(const uint32_t start,
                const uint32_t len,
                const uint32_t prev,
                const uint32_t next,
                Node* nodeArray)
  {
      const uint32_t end = start + len -1;
      
      // If start == 0, then '(node.m_linkIdx == start - 1)' will evaluate to true
      // as m_linkIdxis set to uint32_t_max in case there is no trailing section
      // even though this node is not joined with the any section, so an
      // extra check '(0 != start)' is added to prevent this
      const bool isJoinedWithPrev = (0 != start) && (prev == start - 1);

      // If size == end + 1, there is no trailing section, let alone a touuching trailing section
      const bool isJoinedWithNext = (size > end + 1) && (end + 1 == next);
      
      if (isJoinedWithPrev && isJoinedWithNext)
      { 
        // s......es.....es....e
        // a      An     Nc    C     
        // a,A is the trailing touching section, c,C is the leading 
        // touching section n,N is the freed section, i need to merge
        // a to C
        const uint32_t A = prev;
        const uint32_t n = start;
        const uint32_t N = end;
        const uint32_t a = n - nodeArray[A].m_size;
        const uint32_t c = next;
        const uint32_t C = N + nodeArray[c].m_size; 
          
        nodeArray[a].size = 
        nodeArray[C].size = C + 1 - a;
        
        // memset A,n,N and c to 0 as they are no longer
        // extremities of any section
        memset(&nodeArray[A], 0, sizeof(nodeArray[0]));
        memset(&nodeArray[n], 0, sizeof(nodeArray[0]));
        memset(&nodeArray[N], 0, sizeof(nodeArray[0]));
        memset(&nodeArray[c], 0, sizeof(nodeArray[0]));
      } 
      else if (isJoinedWithPrev)
      {
        // s......es.....e...
        // a      An     N   
        // a,A is the trailing touching section, n,N is the freed
        // section, either N is the last start or there is a section after
        // N, not touching n,N or there isn't a section at all after n,N
        // i need to merge a to N
        const uint32_t A = prev;
        const uint32_t a = A + 1 - nodeArray[A].m_size;
        const uint32_t n = start;
        const uint32_t N = end;
        
        nodeArray[a].m_size =
        nodeArray[N].m_size = N + 1 - a;
        
        // memset A and n to 0 as they are no longer extremities
        // of any section
        memset(&nodeArray[A], 0, sizeof(nodeArray[0]));
        memset(&nodeArray[n], 0, sizeof(nodeArray[0]));
      } 
      else if (isJoinedWithNext) 
      {
        // ...s......es.....e
        //    n      Na     A
        // a,A is the leading touching section, n,N is the freed
        // section, either n is the first start or there is a section
        // before n, not touching n,N or there isn't a section at all
        // before n,N
        // i need to merge n to a
        const uint32_t n = start;
        const uint32_t N = end;
        const uint32_t a = next;
        const uint32_t A = a + nodeArray[a].m_size - 1;
        
        nodeArray[n].m_size =
        nodeArray[A].m_size = A + 1 - n;
        
        // memset N and a to 0 as they are no longer extremities
        // of any section
        memset(&nodeArray[N], 0, sizeof(nodeArray[0]));
        memset(&nodeArray[a], 0, sizeof(nodeArray[0]));
      } 
      else 
      { 
        nodeArray[start] = {prev, len};
        nodeArray[end] = {next, len};
      }
  }

  void addOccupiedNode(const uint32_t start, const uint32_t len)
  {
      const uint32_t end = start + len -1;
      
      // If start == 0, then '(node.m_linkIdx == start - 1)' will evaluate to true
      // as m_linkIdxis set to uint32_t_max in case there is no trailing section
      // even though this node is not joined with the any section, so an
      // extra check '(0 != start)' is added to prevent this
      const bool isJoinedWithPrev = (0 != start) && (prev == start - 1);

      // If size == end + 1, there is no trailing section, let alone a touuching trailing section
      const bool isJoinedWithNext = (size > end + 1) && (end + 1 == next);
      
      if (isJoinedWithPrev && isJoinedWithNext)
      { 
        // s......es.....es....e
        // a      An     Nc    C     
        // a,A is the trailing touching section, c,C is the leading 
        // touching section n,N is the freed section, i need to merge
        // a to C
        const uint32_t A = prev;
        const uint32_t n = start;
        const uint32_t N = end;
        const uint32_t a = n - m_freeNodes[A].m_size;
        const uint32_t c = next;
        const uint32_t C = N + m_freeNodes[c].m_size; 
          
        m_freeNodes[a].size = 
        m_freeNodes[C].size = C + 1 - a;
        
        // memset A,n,N and c to 0 as they are no longer
        // extremities of any section
        memset(&m_freeNodes[A], 0, sizeof(m_freeNodes[0]));
        memset(&m_freeNodes[n], 0, sizeof(m_freeNodes[0]));
        memset(&m_freeNodes[N], 0, sizeof(m_freeNodes[0]));
        memset(&m_freeNodes[c], 0, sizeof(m_freeNodes[0]));
      } 
      else if (isJoinedWithPrev)
      {
        // s......es.....e...
        // a      An     N   
        // a,A is the trailing touching section, n,N is the freed
        // section, either N is the last start or there is a section after
        // N, not touching n,N or there isn't a section at all after n,N
        // i need to merge a to N
        const uint32_t A = prev;
        const uint32_t a = A + 1 - m_freeNodes[A].m_size;
        const uint32_t n = start;
        const uint32_t N = end;
        
        m_freeNodes[a].m_size =
        m_freeNodes[N].m_size = N + 1 - a;
        
        // memset A and n to 0 as they are no longer extremities
        // of any section
        memset(&m_freeNodes[A], 0, sizeof(m_freeNodes[0]));
        memset(&m_freeNodes[n], 0, sizeof(m_freeNodes[0]));
      } 
      else if (isJoinedWithNext) 
      {
        // ...s......es.....e
        //    n      Na     A
        // a,A is the leading touching section, n,N is the freed
        // section, either n is the first start or there is a section
        // before n, not touching n,N or there isn't a section at all
        // before n,N
        // i need to merge n to a
        const uint32_t n = start;
        const uint32_t N = end;
        const uint32_t a = next;
        const uint32_t A = a + m_freeNodes[a].m_size - 1;
        
        m_freeNodes[n].m_size =
        m_freeNodes[A].m_size = A + 1 - n;
        
        // memset N and a to 0 as they are no longer extremities
        // of any section
        memset(&m_freeNodes[N], 0, sizeof(m_freeNodes[0]));
        memset(&m_freeNodes[a], 0, sizeof(m_freeNodes[0]));
      } 
      else 
      { 
        m_freeNodes[start] = {prev, len};
        m_freeNodes[end] = {next, len};
      }
  }

  struct Node
  {
      //Idx of next or prev Node
      uint32_t m_linkIdx;
      //Size of this section
      uint32_t m_size; 
  }; 
    
  Node m_freeNodes[size]; 
  Node m_occupiedNodes[size]; 
  uint32_t m_firstFreeIdx;
  uint32_t m_firstOccupiedIdx;
}; 
 
template <class T, uint32_t Size> 
class FreeListAllocator { 
public: 
    using value_type = T; 
    using pointer = T*; 
    using const_pointer = const T*; 
    using size_type = uint32_t; 
    using difference_type = std::ptrdiff_t;
 
    template <class U> 
    struct rebind { 
        using other = FreeListAllocator<U, Size>; 
    }; 
 
    FreeListAllocator() noexcept { 
        m_pool = reinterpret_cast<T*>(m_rawPool); 
        m_freePtrList = new FreePtrNode(m_pool, Size, nullptr, nullptr); 
    } 
 
    ~FreeListAllocator() noexcept { 
        
        for(auto curr = m_freePtrList; 
            curr;) 
        { 
            auto next = curr->m_next; 
            delete curr; 
            curr = next; 
        } 
    } 
 
    // Allocate memory for n objects of type T 
    T* allocate(std::size_t n) 
    { 
        return static_cast<T*>( 
            ::operator new(n * sizeof(T))); 
    } 
    // Deallocate memory 
    void deallocate(T* p, std::size_t n) noexcept 
    { 
        ::operator delete(p);
    } 
 
    bool operator==(const FreeListAllocator& other) const noexcept {
        return this == &other; 
    } 
 
    bool operator!=(const FreeListAllocator& other) const noexcept { 
        return !(*this == other); 
    } 
 
private: 
    struct FreePtrNode { 
        FreePtrNode(T* ptr, std::size_t len, FreePtrNode* prev, FreePtrNode* next) 
            : m_ptr(ptr), m_len(len), m_prev(prev), m_next(next) {} 
 
        T* m_ptr; 
        uint32_t m_len; 
        FreePtrNode* m_next; 
        FreePtrNode* m_prev; 
    }; 
 
    alignas(T) char m_rawPool[Size * sizeof(T)]; // Aligned storage for pool 
    T*              m_pool; 
    FreePtrNode     m_freePtrList[Size]; 
}; 
 
int main() 
{ 
    std::vector<int, FreeListAllocator<int, 32> > vec; 
    //std::vector<int> vec; 
    FreeListAllocator<int, 32> f; 
    for (uint32_t i = 0; i < 1000000; ++i) 
    { 
        vec.push_back(i);     
    } 
     
    std::cout<<"Hello World"; 
 
    return 0; 
}