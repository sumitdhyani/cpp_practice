#include <cstdlib>
#include <cstdint>
#include <optional>
#include <functional>

typedef uint32_t Size;
const Size Size_max = (Size)-1;
constexpr const Size SectionStartMarker = Size(1) << (sizeof(Size) * 8 - 1);
constexpr const Size SectionInfoRemover = ~SectionStartMarker;

template <class T, Size size>
class SegmentTree
{
private:
    Size  m_tree[2*size - 1];
    typedef std::function<T(const Size)> ArrayValueFetcher;
    const ArrayValueFetcher m_arrIdxFetcher;

    void build(const Size node,
                const Size start,
                Size end)
    {
      if(start == end)
      {
        m_tree[node] = start;
      }
      else
      {
        const Size mid = (start + end) / 2;
        const Size leftChild = node * 2 + 1;
        const Size rightChild = leftChild + 1;

        build(leftChild, start, mid);
        build(rightChild, mid + 1, end);

        m_tree[node] = m_arrIdxFetcher(m_tree[leftChild]) > m_arrIdxFetcher(m_tree[rightChild]) ?
                        m_tree[leftChild]:
                        m_tree[rightChild];
      }
    }

    Size maxInRange(Size node,
                    Size start,
                    Size end,
                    Size l,
                    Size r)
    {
      if (r < start || l > end)
      {
        return Size_max;
      }
      else if (l <= start && end <= r)
      {
        return m_tree[node];
      }

      const Size mid = (start + end) / 2;
      const Size leftChild = node * 2 + 1;
      const Size rightChild = leftChild + 1;
      const Size leftIndex = maxInRange(leftChild, start, mid, l, r);
      const Size rightIndex = maxInRange(rightChild, mid + 1, end, l, r);

      if (Size_max == leftIndex)
      {
        return rightIndex;
      }
      else if (Size_max == rightIndex)
      {
        return leftIndex;
      }
      else
      {
        const Size leftMax = m_arrIdxFetcher(leftIndex);
        const Size rightMax = m_arrIdxFetcher(rightIndex);
        return leftMax > rightMax ? leftIndex : rightIndex;
      }
    }

    void updateTree(const Size node,
                    const Size start,
                    const Size end,
                    const Size idx,
                    const Size value)
    {
      if (start == end)
      {
        m_tree[node] = idx;
      }
      else
      {
        const Size mid = (start + end) / 2;
        const Size leftChild = 2 * node + 1;
        const Size rightChild = leftChild + 1;
        if (idx <= mid)
        {
            updateTree(leftChild, start, mid, idx, value);
        }
        else
        {
            updateTree(rightChild, mid + 1, end, idx, value);
        }

        const Size leftMaxIdx = m_tree[leftChild];
        const Size rightMaxIdx = m_tree[rightChild];
        m_tree[node] = (m_arrIdxFetcher(leftMaxIdx) > m_arrIdxFetcher(rightMaxIdx)) ?
                        leftMaxIdx :
                        rightMaxIdx;
      }
    }

public:
    SegmentTree(const ArrayValueFetcher& arrIdxFetcher) : 
      m_arrIdxFetcher(arrIdxFetcher)
    {
      build(0, 0, size - 1);
    }

    Size maxInrange(const Size l,
                    const Size r)
    {
      return maxInRange(0, 0, size - 1, l, r);
    }

    void update(const Size idx,
                const Size value)
    {
      updateTree(0, 0, size - 1, idx, value);
    }
};

template <Size size>
struct FreeResourcetable
{
  FreeResourcetable() :
    m_freeSegments({size & SectionStartMarker}),// 1st elements as 'size' and rest as 0
    m_OccupiedSegments({0}),
    m_segmentTree([this](const Size idx){ 
      return  m_freeSegments[idx] & SectionStartMarker ?
                m_freeSegments[idx] & SectionInfoRemover :
                0;
    })
  {
    m_freeSegments[size-1] = size;
  }

  std::optional<Size> getFreeIdx(Size len)
  {
    const Size idx = m_segmentTree.maxInRange(0, size - 1);
    const Size longestSectionLen = m_freeSegments[idx];
    if (longestSectionLen >= len)
    {
      const Size newLen = longestSectionLen - len;
      m_freeSegments[idx] = 0;
      m_freeSegments[idx + longestSectionLen - 1] = newLen;

      m_segmentTree.update(idx, 0);
      if (newLen > 0)
      {
        const Size newSectionStart = idx + len;
        m_freeSegments[newSectionStart] = newLen | SectionStartMarker;
        m_segmentTree.update(newSectionStart, newLen);
      }

      m_OccupiedSegments[idx] = len;
      return idx;
    }
    else
    {
      return std::nullopt;
    }
  }

  void freeIdx(Size idx)
  {
    const Size sectionLen = m_OccupiedSegments[idx];
    m_OccupiedSegments[idx] = 0;

    m_freeSegments[idx] =
    m_freeSegments[idx + sectionLen - 1] = sectionLen;

    m_freeSegments[idx] |= SectionStartMarker;
    m_segmentTree.update(idx, sectionLen);

    adjustFreeSection(idx);
  }

  void adjustFreeSection(Size idx)
  {
    Size sectionEnd = idx + (m_freeSegments[idx] & SectionInfoRemover) - 1;
    if (idx > 0 && m_freeSegments[idx-1])
    {
      // s......es.....e...
      // a      An     N   
      // a,A is the trailing touching section, n,N is the freed
      // section, either N is the last start or there is a section after
      // N, not touching n,N or there isn't a section at all after n,N
      // i need to merge a to N
      const Size A = idx - 1;
      const Size a = A + 1 - m_freeSegments[A];
      const Size n = idx;
      const Size N = n + (m_freeSegments[n] & SectionInfoRemover) - 1;

      const Size mergedLen = m_freeSegments[N] + m_freeSegments[A];

      m_freeSegments[a] =
      m_freeSegments[N] = mergedLen;
      m_freeSegments[a] |= SectionStartMarker;

      m_freeSegments[A] =
      m_freeSegments[n] = 0;

      m_segmentTree.update(n, 0);
      m_segmentTree.update(a, mergedLen);
    }

    // If [n,N] had a neighouring section, let's name it [b,B], and [b,B] started immediately after N,
    // then repeat the step above for [b,B], this will result in max 1 reursive call
    if (size > sectionEnd + 1 && 0 != m_freeSegments[sectionEnd + 1])
    {
      adjustFreeSection(sectionEnd + 1);
    }
  }

  private:
  Size m_freeSegments[size];
  Size m_OccupiedSegments[size];
  SegmentTree<Size, size> m_segmentTree;
};

//#include <iostream>
//#include <stdlib.h>
//
//int main(int argc, char** argv)
//{
//  const int start = atoi(argv[1]);
//  const int end = atoi(argv[2]);
//  int arr[] = {6, 9, 4, 2, 7};
//  const std::function<Size(const Size)> fetchIndexVal = [&arr](const Size idx){ return arr[idx]; };
//  SegmentTree<Size, 5> st(fetchIndexVal);
//  std::cout << arr[st.maxInrange(start, end)] << std::endl;
//  return 0;
//}
