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
  constexpr static const Size bookSize = size * 2;

  FreeResourcetable() :
    m_freeNodes({0,0}),
    m_occupiedNodes(0),
    m_firstFreeIdx(0)
  {
      Node& firstFreeNode = m_freeNodes[0];
      Node& lastFreeNode = m_freeNodes[bookSize-1];

      firstFreeNode.m_linkIdx =
      lastFreeNode.m_linkIdx = Size_max;

      firstFreeNode.m_size =
      lastFreeNode.m_size = bookSize;
  }


  std::optional<Size> getNextFreeIdx(Size len)
  {
    // multiply by 2
    Size notionalLen = len << 1;
    // No free section available
    if (Size_max == m_firstFreeIdx)
    {
        return std::nullopt;
    }

    for(Size currFreeIndex = m_firstFreeIdx;
        currFreeIndex != Size_max;
        currFreeIndex = m_freeNodes[currFreeIndex +
                        m_freeNodes[currFreeIndex].m_size -1].m_linkIdx)
    {
        const Node& currNode = m_freeNodes[currFreeIndex];
        const Size pairedIdx = currFreeIndex + currNode.m_size - 1;
        const Node& pairedNode = m_freeNodes[pairedIdx];

        if (currNode.m_size >= notionalLen)
        {
            shrink(m_freeNodes,
                    currNode.m_linkIdx,
                    pairedNode.m_linkIdx,
                    currFreeIndex,
                    pairedIdx,
                    notionalLen,
                    m_firstFreeIdx);

            Size origIdx = currFreeIndex >> 1;
            m_occupiedNodes[origIdx] = len;
            return origIdx;
        }
    }

    return std::nullopt;
  }

  void freeIdx(Size idx)
  {
    addFreeSection(idx << 1,
                   m_occupiedNodes[idx] << 1);
    m_occupiedNodes[idx] = 0;
  }

  private:

  // Returns the prev section end idx and next section start idx
  // if we were to inser a section starting at 'start' length 'len'
  std::tuple<Size, Size> getPrevAndNext(Size start,
                                        Size end)
  {
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
    if (Size_max == m_firstFreeIdx)
    {
      return {Size_max, Size_max};
    }
    // Case 1:
    // Before:
    // arary begin |...........a........A.....
    //
    // After:
    // arary begin |...........a........A.....
    //               ↑     ↑
    //             start  end
    // So that prev = Size_max, next = a
    else if (start < m_firstFreeIdx)
    {
      return {Size_max, m_firstFreeIdx};
    }


    // Following cases may occur in the loop below:
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
    Size prev = Size_max;
    Size next = Size_max;

    bool foundPrevAndNext = false;
    for (Size a = m_firstFreeIdx;
         !foundPrevAndNext;
         // Set it to the start of nextSection
         a = m_freeNodes[a + m_freeNodes[a].m_size - 1].m_linkIdx)
    {
      const Size A = a + m_freeNodes[a].m_size - 1;
      const Size b = m_freeNodes[A].m_linkIdx;

      
      // for cases 2 & 3
      if (b > start)
      {
        // This is Common stuff to be done for cases 2 and 3
        prev = A;
        
        // Case 3 only
        if(Size_max != b)
        {
          next = b;
        }
        
        foundPrevAndNext = true;
      }
    }

    return {prev, next};
  }

  void addFreeSection(const Size start,
                      const Size len)
  {
      const Size end = start + len - 1;
      const auto [prev, next] = getPrevAndNext(start,
                                               end);

      m_freeNodes[start] = {prev, len};
      m_freeNodes[end] = {next, len};
      mergeFreeSection(start, end, prev, next);
      if (start < m_firstFreeIdx)
      {
          m_firstFreeIdx = start;
      }
  }

  void mergeFreeSection(const Size start,
                           const Size end,
                           const Size prev,
                           const Size next)
  {
    if (start > 0 && prev+1 == start)
    {
      // s......es.....e...
      // a      An     N   
      // a,A is the trailing touching section, n,N is the freed
      // section, either N is the last start or there is a section after
      // N, not touching n,N or there isn't a section at all after n,N
      // i need to merge a to N
      const Size A = start - 1;
      const Size a = A + 1 - m_freeNodes[A].m_size;
      const Size n = start;
      const Size N = n + m_freeNodes[n].m_size - 1;

      m_freeNodes[a].m_size =
      m_freeNodes[N] = N - a + 1;

      m_freeNodes[A] =
      m_freeNodes[n] = {0,0};
    }

    // If [n,N] had a neighouring section, let's name it [b,B], and [b,B] started immediately after N,
    // then repeat the step above for [b,B], this will result in max 1 recursive call
    if (size > end + 1  && next == end + 1)
    {
      const Size nextSectionEnd = next + m_freeNodes[next] - 1;
      mergeFreeSection(next, nextSectionEnd, end, m_freeNodes[nextSectionEnd].m_linkId);
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

      // Case 0
      //       |------- len = sectionSize ----|
      // ......n..............................N....
      //       ↑                              ↑
      //    startIdx                        endIdx
      if (sectionSize == len)
      {
        if (prevIdx != Size_max)
        {
          nodeArr[prevIdx].m_linkIdx = nextIdx;
        }

        if (nextIdx != Size_max)
        {
          nodeArr[nextIdx].m_linkIdx = prevIdx;
        }

        startNode = 
        endNode = {0,0};

        if (startIdx == firstIdx)
        {
          firstIdx = nextIdx;
        }
      }
      // Case 1
      //       |------- len < sectionSize ----|
      // ......n....................................N....
      //       ↑                                    ↑
      //    startIdx                             endIdx
      else
      {
        startNode.m_size =
        endNode.m_size = sectionSize - len;

        //The currNode is moved forward by 'len'
        const Size newPos = startIdx + len;
        Node& newNode = nodeArr[newPos];
        newNode = startNode;
        
        startNode = {0,0};

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

  Node m_freeNodes[bookSize];
  Size m_occupiedNodes[size];
  Size m_firstFreeIdx;
};
