#include <cstdlib> 
#include <cstdint> 
#include <iostream> 
#include <vector> 
#include <cstring> 
#include <functional>
#include <optional>
 
template<uint32_t size> 
struct FreeResourcetable 
{ 
  private:
  struct Node
  {
      //Idx of next or prev Node
      uint32_t m_linkIdx;
      //Size of this section
      uint32_t m_size; 
  }; 

  public:
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
    
  
  std::optional<uint32_t> getNextFreeIdx(uint32_t len) 
  { 
      // No free section available
      if (uint32_t_max == m_firstFreeIdx)
      { 
          return std::nullopt;
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
              shrink(m_freeNodes,
                     currNode.m_linkIdx,
                     pairedNode.m_linkIdx,
                     currFreeIndex,
                     pairedIdx,
                     len,
                     m_firstFreeIdx);

              addOccupiedSection(currFreeIndex,
                                 len);

              return currFreeIndex;
          }
      }

      return std::nullopt;
  }

  void freeIdx(const uint32_t idx)
  {
    const uint32_t sectionSize  = m_occupiedNodes[idx].m_size;
    const uint32_t prevIdx      = m_occupiedNodes[idx].m_linkId;
    const uint32_t nextIdx      = m_occupiedNodes[idx + sectionSize - 1].m_linkId;

    shrink(m_occupiedNodes,
            prevIdx,
            nextIdx,
            idx,
            idx + sectionSize - 1,
            sectionSize,
            m_firstOccupiedIdx);

    addFreeSection(idx,
                   sectionSize);
  }

  private:

  // Returns the prev section end idx and next section start idx
  // if we were to inser a section starting at 'start' length 'len' 
  std::tuple<uint32_t, uint32_t> getPrevAndNext(Node* nodeArray,
                                                uint32_t start,
                                                uint32_t end,
                                                uint32_t firstIdx)
  {
    // Find the prev and next for this section if it is to be
    // added in the metaArray
    // Following cases are anticipated:
    // ==================================================
    // Case 0(no section is there yet):
    // Before:
    // arary begin |.........................| array end
    //
    // After:
    // arary begin |.........................| array end
    //                 ↑       ↑
    //               start    end
    // So that prev = next = uint32_t_max
    //
    // ==================================================
    // Case 1:
    // Before:
    // arary begin |...........a........A.....| array end
    //
    // After:
    // arary begin |...........a........A.....| array end
    //               ↑     ↑
    //             start  end
    // So that prev = uint32_t_max, next = a
    //
    // ==================================================
    // Case 2:
    // Before:
    // ...a.....A...............| array end
    //
    // After:
    // ...a.....A...............| array end
    //             ↑       ↑
    //           start    end
    // So that prev = A, next = uint32_t_max 
    //
    // ==================================================
    // Case 3:
    // Before:
    // ...a.....A...............b.....B...
    //
    // After:
    // ...a.....A...............b.....B...
    //             ↑       ↑
    //           start    end
    // So that prev = A, next = b


    // Case 0
    if (uint32_t_max == firstIdx)
    {
      return {uint32_t_max, uint32_t_max};
    }
    // Case 1 
    else if (start < firstIdx)
    {
      return {uint32_t_max, firstIdx};
    }

    uint32_t prev = uint32_t_max;
    uint32_t next = uint32_t_max;
    
    bool foundPrevAndNext = false;
    for (uint32_t currSectionStart = firstIdx;
         !foundPrevAndNext && currSectionStart != uint32_t_max;
         // Set it to the start of nextSection 
         currSectionStart = nodeArray[currSectionStart + nodeArray[currSectionStart].m_size - 1].m_linkIdx)
    {
      const uint32_t pairedIdx = currSectionStart + nodeArray[currSectionStart].m_size - 1;
      const Node& pairedNode = nodeArray[pairedIdx];
      const uint32_t nextSectionStart = pairedNode.m_linkIdx;

      // Case 2
      if(uint32_t_max == nextSectionStart)
      {
        prev = currSectionStart;
        foundPrevAndNext = true;
      }
      // Case 3
      else if(nextSectionStart > start)
      {
        prev = pairedIdx;
        next = nextSectionStart;
        foundPrevAndNext = true;
      }
    }

    return {prev, next};
  }

  void addOccupiedSection(const uint32_t start,
                          const uint32_t len)
  {
      const uint32_t end = start + len - 1;
      const auto [prev, next] = getPrevAndNext(m_occupiedNodes,
                                                  start,
                                                  end,
                                                  m_firstOccupiedIdx);

      m_occupiedNodes[start] = {prev, len};
      m_occupiedNodes[end] = {next, len};

      if (uint32_t_max != prev)
      {
        Node& prevNode = m_occupiedNodes[prev];
        prevNode.m_linkIdx = start;
      }

      if (uint32_t_max != next)
      {
        Node& nextNode = m_occupiedNodes[next];
        nextNode.m_linkIdx = start;
      }


      if (start < m_firstOccupiedIdx)
      {
        m_firstOccupiedIdx = start;
      }
  }

  void addFreeSection(const uint32_t start,
                      const uint32_t len)
  {
      const uint32_t end = start + len - 1;
      const auto [prev, next] = getPrevAndNext(m_freeNodes,
                                                  start,
                                                  end,
                                                  m_firstFreeIdx);
      
      // If start == 0, then '(node.m_linkIdx == start - 1)' will evaluate to true
      // as m_linkIdxis set to uint32_t_max in case there is no trailing section
      // even though this node is not joined with the any section, so an
      // extra check '(0 != start)' is added to prevent this
      const bool isJoinedWithPrev = (0 != start) && (prev == start - 1);

      // If size == end + 1, there is no trailing section, let alone a touching trailing section
      const bool isJoinedWithNext = (size > end + 1) && (end + 1 == next);
      
      if (isJoinedWithPrev && isJoinedWithNext)
      { 
        // Case 0
        // a......An.....Nc....C
        // [a,A] is the trailing touching section, [c,C] is the leading 
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
        // Case 1
        // a......An.....N...
        // [a,A] is the trailing touching section, [n,N] is the freed
        // section, either N is the last start or there is a section after
        // N, not touching n,N or there isn't a section at all after [n,N]
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
        // Case 2
        // ...n......Na.....A
        // [a,A] is the leading touching section, [n,N] is the freed
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
        // Case 3
        // ...n......N.....
        // [n,N] is the freed section, it has no touching section either from the 
        // back or the front
        m_freeNodes[start] = {prev, len};
        m_freeNodes[end] = {next, len};
      }

      // Possible only in caes 2 & 3
      if (start < m_firstFreeIdx)
      {
          m_firstFreeIdx = start;
      }
  }

  void shrink(Node* nodeArr,
              uint32_t prevIdx,
              uint32_t nextIdx,
              uint32_t startIdx,
              uint32_t endIdx,
              uint32_t len,
              uint32_t& firstIdx)
  { 
      Node& startNode = nodeArr[startIdx];
      Node& endNode = nodeArr[endIdx];
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

        if (startIdx == firstIdx)
        {
          firstIdx = nextIdx;
        }
      }
      else
      {
        startNode.m_size =
        endNode.m_size = sectionSize - len;

        //The currNode is moved forward by 'len'
        const uint32_t newPos = startIdx + len;
        Node& newNode = nodeArr[newPos];
        newNode = startNode;
        memset(&startNode, 0, sizeof(startNode));
        
        if(startIdx == firstIdx)
        { 
            firstIdx = newPos;
        } 
        else if (newNode.m_linkIdx != uint32_t_max)
        { 
            nodeArr[newNode.m_linkIdx].m_linkIdx = newPos;
        }
      }
  }

  Node m_freeNodes[size]; 
  Node m_occupiedNodes[size]; 
  uint32_t m_firstFreeIdx;
  uint32_t m_firstOccupiedIdx;
};
 

 
int main() 
{ 
  return 0; 
}