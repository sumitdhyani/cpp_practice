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

      // If start == 0, then '(node.m_linkIdx == start - 1)' evaluates to true
      // as m_linkIdxis set to Size_max in case there is no trailing section
      // even though this node is not joined with the any section, so an
      // extra check '(0 != start)' is added to prevent this
      const bool isJoinedWithPrev = (0 != start) && (prev == start - 1);

      // If size == end + 1, there is no trailing section,
      // let alone a touching trailing section
      const bool isJoinedWithNext = (bookSize > end + 1) && (end + 1 == next);

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

        m_freeNodes[a].m_size =
        m_freeNodes[C].m_size = C + 1 - a;

        // memset A,n,N and c to 0 as they are no longer
        // extremities of any section
        m_freeNodes[A] = 
        m_freeNodes[n] =
        m_freeNodes[N] =
        m_freeNodes[c] = {0,0};
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
        m_freeNodes[A] = 
        m_freeNodes[n] = {0,0};
        
        if (next != Size_max)
        {
            m_freeNodes[next].m_linkIdx = N;
        }
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
        m_freeNodes[N] =
        m_freeNodes[a] = {0,0};
        
        if (prev != Size_max)
        {
            m_freeNodes[prev].m_linkIdx = n;
        }
      }
      else
      {
        // Case 3
        // ...n......N.....
        // [n,N] is the freed section, it has no touching section
        // either from the rear or the front
        if (prev != Size_max)
        {
            m_freeNodes[prev].m_linkIdx = start;
        }
        
        if (next != Size_max)
        {
            m_freeNodes[next].m_linkIdx = end;
        }
      }

      // Possible only in cases 2 & 3
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

struct FreeResourcetable_V2
{
  struct Node
  {
    Size m_idx;
    Size m_size;
    Node* m_next;

    Node(Size linkIdx, Size size)
        : m_idx(linkIdx), m_size(size), m_next(nullptr) {}

    // Constructor for the last node in the list
    Node(Size linkIdx, Size size, Node* next)
        : m_idx(linkIdx), m_size(size), m_next(next) {}
  };
private:
  Node* m_freeNodes;
  Node* m_occupiedNodes;

public:
  FreeResourcetable_V2(const Size& size) : m_freeNodes(new Node(0, size, nullptr)),
                           m_occupiedNodes(nullptr)
  {}

  std::optional<Size> getNextFreeIdx(Size len)
  {
    if (!m_freeNodes)
    {
      return std::nullopt;
    }

    Node *prevFreeNode = nullptr;
    Node *currFreeNode = m_freeNodes;
    while (currFreeNode)
    {
      if (currFreeNode->m_size < len)
      {
        prevFreeNode = currFreeNode;
        currFreeNode = currFreeNode->m_next;
        continue;
      }

      Node* occupiedNode = new Node(currFreeNode->m_idx, len, m_occupiedNodes);
      m_occupiedNodes = occupiedNode;

      currFreeNode->m_size -= len;
      currFreeNode->m_idx += len;
      // If the size of the current free node is exactly equal to the requested size
      if (!currFreeNode->m_size)
      {

        if (prevFreeNode)
        {
          // Link the previous free node to the next one
          prevFreeNode->m_next = currFreeNode->m_next;
        }
        else
        {
          // If this is the first free node, update m_freeNodes
          m_freeNodes = currFreeNode->m_next;
        }

        delete currFreeNode;
      }
      // Return the index of the newly occupied node
      return occupiedNode->m_idx;
    }
    
    return std::nullopt;
  }

  bool freeIdx(Size idx)
  {

    Node *prevOccupiedNode = nullptr;
    Node *currOccupiedNode = m_occupiedNodes;
    while (currOccupiedNode)
    {
      if (currOccupiedNode->m_idx > idx)
      {
        prevOccupiedNode = currOccupiedNode;
        currOccupiedNode = currOccupiedNode->m_next;
        continue;
      }

      if (currOccupiedNode->m_idx != idx)
      {
        return false;
      }

      // If the node is found, remove it from the occupied list
      if (prevOccupiedNode)
      {
        prevOccupiedNode->m_next = currOccupiedNode->m_next;
      }
      else
      {
        m_occupiedNodes = currOccupiedNode->m_next;
      }

      Node* newFreeNode = new Node(currOccupiedNode->m_idx, currOccupiedNode->m_size);
      delete currOccupiedNode;
      onFreeNodeReclaimed(newFreeNode);
      return true;
    }

    return false;
  }

private:
  void onFreeNodeReclaimed(Node* reclaimedNode)
  {
    // If the reclaimed node is the first free node, update m_freeNodes
    if (!m_freeNodes || reclaimedNode->m_idx > m_freeNodes->m_idx)
    {
      reclaimedNode->m_next = m_freeNodes;
      m_freeNodes = reclaimedNode;
      return;
    }

    // Otherwise, find the correct position to insert the reclaimed node
    Node* curr = m_freeNodes;
    Node* prev = nullptr;
    while (curr && curr->m_idx > reclaimedNode->m_idx)
    {
      prev = curr;
      curr = curr->m_next;
    }

    // Insert the reclaimed node in the sorted order
    if (prev)
    {
      if (prev->m_idx + prev->m_size == reclaimedNode->m_idx)
      {
        // If the reclaimed node is adjacent to the previous node, merge them
        prev->m_size += reclaimedNode->m_size;
        delete reclaimedNode;
        reclaimedNode = prev;
      }
      else
      {
        // Otherwise, link the previous node to the reclaimed node
        prev->m_next = reclaimedNode;
      }
    }
    // If the reclaimed node is adjacent to the current node, merge them
    if (curr && reclaimedNode->m_idx + reclaimedNode->m_size == curr->m_idx)
    {
      reclaimedNode->m_size += curr->m_size;
      reclaimedNode->m_next = curr->m_next;
      delete curr;
    }
    else
    {
      reclaimedNode->m_next = curr;
    }
  }
  
  ~FreeResourcetable_V2()
  {
    Node* curr = m_freeNodes;
    while (curr)
    {
      Node* next = curr->m_next;
      delete curr;
      curr = next;
    }

    curr = m_occupiedNodes;
    while (curr)
    {
      Node* next = curr->m_next;
      delete curr;
      curr = next;
    }
  }

};
