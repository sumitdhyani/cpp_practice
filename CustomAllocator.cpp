#include <cstdlib> 
#include <cstdint> 
#include <tuple>
#include <optional>

typedef uint32_t Size;

template<Size size>
struct FreeResourcetable
{ 
  private:
  struct Node
  {
      //Idx of next or prev Node
      Size m_linkIdx;
      //Size of this section
      Size m_size;
  }; 

  public:
  constexpr static const Size Size_max = Size(-1); 
    
  FreeResourcetable() :
    m_freeNodes({0,0}),
    m_occupiedNodes({0,0}),
    m_firstFreeIdx(0),
    m_firstOccupiedIdx(Size_max)
  { 
      Node& firstFreeNode = m_freeNodes[0]; 
      Node& lastFreeNode = m_freeNodes[size-1]; 

      firstFreeNode.m_linkIdx =
      lastFreeNode.m_linkIdx = Size_max; 
      
      firstFreeNode.m_size =
      lastFreeNode.m_size = size;
  }
    
  
  std::optional<Size> getNextFreeIdx(Size len) 
  { 
      // No free section available
      if (Size_max == m_firstFreeIdx)
      { 
          return std::nullopt;
      }
      
      for(Size currFreeIndex = m_firstFreeIdx;
          currFreeIndex != Size_max;
          currFreeIndex = m_freeNodes[currFreeIndex + m_freeNodes[currFreeIndex].m_size -1].m_linkIdx)
      {
          const Node& currNode = m_freeNodes[currFreeIndex];
          const Size pairedIdx = currFreeIndex + currNode.m_size - 1;
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

  void freeIdx(const Size idx)
  {
    const Size sectionSize  = m_occupiedNodes[idx].m_size;
    const Size prevIdx      = m_occupiedNodes[idx].m_linkId;
    const Size nextIdx      = m_occupiedNodes[idx + sectionSize - 1].m_linkId;

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
  std::tuple<Size, Size> getPrevAndNext(Node* nodeArray,
                                                Size start,
                                                Size end,
                                                Size firstIdx)
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
    // So that prev = next = Size_max
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
    // So that prev = Size_max, next = a
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
    // So that prev = A, next = Size_max 
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
    if (Size_max == firstIdx)
    {
      return {Size_max, Size_max};
    }
    // Case 1 
    else if (start < firstIdx)
    {
      return {Size_max, firstIdx};
    }

    Size prev = Size_max;
    Size next = Size_max;
    
    bool foundPrevAndNext = false;
    for (Size a = firstIdx;
         !foundPrevAndNext && a != Size_max;
         // Set it to the start of nextSection
         a = nodeArray[a + nodeArray[a].m_size - 1].m_linkIdx)
    {
      const Size A = a + nodeArray[a].m_size - 1;
      const Size b = nodeArray[A].m_linkIdx;

      // Common stuff for case 2 & 3
      if (b > start)
      {
        prev = A;
        foundPrevAndNext = true;
      }

      if(Size_max != b)// Case 3 only
      {
        next = b;
        foundPrevAndNext = true;
      }
    }

    return {prev, next};
  }

  void addOccupiedSection(const Size start,
                          const Size len)
  {
      const Size end = start + len - 1;
      const auto [prev, next] = getPrevAndNext(m_occupiedNodes,
                                                  start,
                                                  end,
                                                  m_firstOccupiedIdx);

      m_occupiedNodes[start] = {prev, len};
      m_occupiedNodes[end] = {next, len};

      if (Size_max != prev)
      {
        Node& prevNode = m_occupiedNodes[prev];
        prevNode.m_linkIdx = start;
      }

      if (Size_max != next)
      {
        Node& nextNode = m_occupiedNodes[next];
        nextNode.m_linkIdx = start;
      }


      if (start < m_firstOccupiedIdx)
      {
        m_firstOccupiedIdx = start;
      }
  }

  void addFreeSection(const Size start,
                      const Size len)
  {
      const Size end = start + len - 1;
      const auto [prev, next] = getPrevAndNext(m_freeNodes,
                                                  start,
                                                  end,
                                                  m_firstFreeIdx);
      
      // If start == 0, then '(node.m_linkIdx == start - 1)' will evaluate to true
      // as m_linkIdxis set to Size_max in case there is no trailing section
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
        const Size A = prev;
        const Size n = start;
        const Size N = end;
        const Size a = n - m_freeNodes[A].m_size;
        const Size c = next;
        const Size C = N + m_freeNodes[c].m_size;
          
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
        // N, not touching [n,N] or there isn't a section at all after [n,N]
        // i need to merge a to N
        const Size A = prev;
        const Size a = A + 1 - m_freeNodes[A].m_size;
        const Size n = start;
        const Size N = end;
        
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
        // before n, not touching [n,N] or there isn't a section at all
        // before [n,N]
        // i need to merge n to a
        const Size n = start;
        const Size N = end;
        const Size a = next;
        const Size A = a + m_freeNodes[a].m_size - 1;
        
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
              Size prevIdx,
              Size nextIdx,
              Size startIdx,
              Size endIdx,
              Size len,
              Size& firstIdx)
  { 
      Node& startNode = nodeArr[startIdx];
      Node& endNode = nodeArr[endIdx];
      Size sectionSize = startNode.m_size;

      if (sectionSize == len)
      {
        if (prevIdx != Size_max)
        {
          nodeArr[prevIdx].m_linkId = nextIdx;
        }

        if (nextIdx != Size_max)
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
        const Size newPos = startIdx + len;
        Node& newNode = nodeArr[newPos];
        newNode = startNode;
        memset(&startNode, 0, sizeof(startNode));
        
        if(startIdx == firstIdx)
        { 
            firstIdx = newPos;
        } 
        else if (newNode.m_linkIdx != Size_max)
        { 
            nodeArr[newNode.m_linkIdx].m_linkIdx = newPos;
        }
      }
  }

  Node m_freeNodes[size]; 
  Node m_occupiedNodes[size]; 
  Size m_firstFreeIdx;
  Size m_firstOccupiedIdx;
};
 

 
int main() 
{ 
  return 0; 
}