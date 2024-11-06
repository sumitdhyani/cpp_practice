#include <cstdlib>
#include <cstdint>
#include <optional>
#include <functional>

typedef uint32_t Size;
const Size Size_max = (Size)-1;
constexpr const Size SectionStartMarker = Size(1) << (sizeof(Size) * 8 - 1);
constexpr const Size SectionInfoRemover = ~SectionStartMarker;

template<class T, Size size>
struct SectionArray
{
  SectionArray(T* const arr) : m_arr(arr) {}

  T operator[](const Size idx) const
  {
    const Size val = m_arr[idx];
    return  val & SectionStartMarker?
              val & SectionInfoRemover:
              0;
  }

  private:
  T* m_arr;
};

template <class T, Size size>
class SegmentTree
{
private:
    Size  m_tree[2*size - 1];
    typedef std::function<T(const Size)> ArrayValueFetcher;
    std::function<T(const Size)> m_fetchValueAtIdx;//Fetch value at an idx in the array

    void build(Size node, Size start, Size end, const ArrayValueFetcher& m_fetchValueAtIdx)
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

        build(leftChild, start, mid, m_fetchValueAtIdx);
        build(rightChild, mid + 1, end, m_fetchValueAtIdx);

        m_tree[node] = m_fetchValueAtIdx(m_tree[leftChild]) > m_fetchValueAtIdx(m_tree[rightChild]) ?
                        m_tree[leftChild]:
                        m_tree[rightChild];
      }
    }

    Size maxInRange(Size node, Size start, Size end, Size l, Size r, const ArrayValueFetcher& m_fetchValueAtIdx)
    {
      if (r < start || l > end) {
        return Size_max;
      }
      if (l <= start && end <= r) {
        return m_tree[node];
      }

      Size mid = (start + end) / 2;
      const Size leftChild = node * 2 + 1;
      const Size rightChild = leftChild + 1;
      Size leftIndex = maxInRange(leftChild, start, mid, l, r, m_fetchValueAtIdx);
      Size rightIndex = maxInRange(rightChild, mid + 1, end, l, r, m_fetchValueAtIdx);

      if (Size_max == leftIndex)
      {
        return m_fetchValueAtIdx(rightIndex);
      }
      else if (Size_max == rightIndex)
      {
        return m_fetchValueAtIdx(leftIndex);
      }
      else
      {
        Size leftMax = m_fetchValueAtIdx(leftIndex);
        Size rightMax = m_fetchValueAtIdx(rightIndex);
        return leftMax > rightMax ? leftIndex : rightIndex;
      }
    }

    void updateTree(Size node, Size start, Size end, Size idx, Size value, const ArrayValueFetcher& m_fetchValueAtIdx)
    {
      if (start == end)
      {
        m_tree[node] = idx;
      }
      else
      {
        Size mid = (start + end) / 2;
        Size leftChild = 2 * node + 1;
        Size rightChild = leftChild + 1;
        if (idx <= mid)
        {
            updateTree(leftChild, start, mid, idx, value);
        }
        else
        {
            updateTree(rightChild, mid + 1, end, idx, value);
        }

        Size leftMaxIdx = m_tree[leftChild];
        Size rightMaxIdx = m_tree[rightChild];
        m_tree[node] = (m_fetchValueAtIdx(leftMaxIdx) > m_fetchValueAtIdx(rightMaxIdx)) ?
                        leftMaxIdx :
                        rightMaxIdx;
      }
    }

public:
    SegmentTree(const SectionArray<Size, size> arr, const ArrayValueFetcher& m_fetchValueAtIdx)
    {
      build(0, 0, size - 1, m_fetchValueAtIdx);
    }

    Size maxInrange(Size l, Size r, const ArrayValueFetcher& m_fetchValueAtIdx)
    {
      return maxInRange(0, 0, size - 1, l, r, m_fetchValueAtIdx);
    }

    void update(Size idx, Size value, const ArrayValueFetcher& m_fetchValueAtIdx)
    {
      updateTree(0, 0, size - 1, idx, value, m_fetchValueAtIdx);
    }
};

template <Size size>
struct FreeResourcetable
{
  FreeResourcetable() :
    m_freeSegments({size & SectionStartMarker}),// 1st elements as 'size' and rest as 0
    m_OccupiedSegments({0}),
    m_segmentTree(SectionArray<Size, size>(m_freeSegments))
  {
    m_freeSegments[size-1] = size;
  }

  std::optional<Size> getFreeIdx(Size len)
  {
    Size idx = m_segmentTree.maxInrange(0, size - 1);
    Size longestSectionLen = m_freeSegments[idx];
    if (longestSectionLen >= len)
    {
      const Size newLen = longestSectionLen - len;
      m_freeSegments[idx + len] =
      m_freeSegments[idx + longestSectionLen - 1] = newLen;
      m_freeSegments[idx] = 0;

      m_segmentTree.update(idx, 0);
      if (newLen > 0)
      {
        m_freeSegments[idx + len] |= SectionStartMarker;
        m_segmentTree.update(idx + len, newLen);
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

    if (size > sectionEnd + 1 && m_freeSegments[sectionEnd + 1])
    {
      adjustFreeSection(sectionEnd + 1);
    }
  }

  private:
  Size m_freeSegments[size];
  Size m_OccupiedSegments[size];
  SegmentTree<Size, size> m_segmentTree;
};

int main()
{
  return 0;
}