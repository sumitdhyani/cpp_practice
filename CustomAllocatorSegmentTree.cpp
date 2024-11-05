#include <cstdlib>
#include <cstdint>
#include <tuple>
#include <optional>

typedef uint32_t Size;
const Size Size_max = (Size)-1;
constexpr const Size SectionStartMarker = Size(1) << (sizeof(Size) * 8 - 1);
constexpr const Size SectionInfoRemover = ~SectionStartMarker;

template<class T, Size size>
struct SectionArray
{
  SectionArray(T* arr) : m_arr(arr) {}

  T operator[](const Size idx) const
  {
    const Size val = m_arr[idx];
    return !val?
              0:
              val & SectionStartMarker?
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
    Size  m_tree[4*size];
    const SectionArray<T, size> m_arr;  // original array, memory to be owned by the creator of the object

    // Helper function to build the tree
    void buildTree(Size node, Size start, Size end)
    {
      if (start == end)
      {
        m_tree[node] = start;
      }
      else
      {
        Size mid = (start + end) / 2;
        buildTree(2 * node + 1, start, mid);
        buildTree(2 * node + 2, mid + 1, end);
        // Store the index with the maximum value between the left and right subtrees
        Size leftIndex = m_tree[2 * node + 1];
        Size rightIndex = m_tree[2 * node + 2];
        m_tree[node] = (m_arr[leftIndex] > m_arr[rightIndex]) ? leftIndex : rightIndex;
      }
    }

    // Helper function for range maximum query (returns the index)
    Size rangeMaxQuery(Size node, Size start, Size end, Size l, Size r)
    {
      if (r < start || l > end) {
        // Range is completely outside the current segment
        return Size_max; // Return an invalid index
      }
      if (l <= start && end <= r) {
        // Range is completely inside the current segment
        return m_tree[node];
      }
      // Partial overlap: check both left and right subtrees
      Size mid = (start + end) / 2;
      Size leftIndex = rangeMaxQuery(2 * node + 1, start, mid, l, r);
      Size rightIndex = rangeMaxQuery(2 * node + 2, mid + 1, end, l, r);

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
        return (m_arr[leftIndex] > m_arr[rightIndex]) ? leftIndex : rightIndex;
      }
    }

    // Helper function for point update
    void updateTree(Size node, Size start, Size end, Size idx, Size value)
    {
      if (start == end)
      {
        // Leaf node: update the value in the original array
        m_tree[node] = idx;
      }
      else
      {
        Size mid = (start + end) / 2;
        if (idx <= mid)
        {
            updateTree(2 * node + 1, start, mid, idx, value);
        }
        else
        {
            updateTree(2 * node + 2, mid + 1, end, idx, value);
        }
        // Update the current node after updating subtrees
        Size leftIndex = m_tree[2 * node + 1];
        Size rightIndex = m_tree[2 * node + 2];
        m_tree[node] = (m_arr[leftIndex] > m_arr[rightIndex]) ? leftIndex : rightIndex;
      }
    }

public:
    // Constructor to initialize the segment tree
    SegmentTree(Size* arr) : m_arr(arr)
    {
      m_arr = arr;
      buildTree(0, 0, size - 1);
    }

    // Public method to perform range maximum query (returns the index)
    Size rangeMax(Size l, Size r)
    {
      return rangeMaxQuery(0, 0, size - 1, l, r);
    }

    // Public method to update an element at a specific index
    void update(Size idx, Size value)
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
    m_segmentTree(m_freeSegments)
  {
  }

  std::optional<Size> getFreeIdx(Size len)
  {
    Size idx = m_segmentTree.rangeMax(0, size - 1);
    Size longestSectionLen = m_freeSegments[idx];
    if (longestSectionLen >= len)
    {
      m_freeSegments[idx + len] = longestSectionLen - len;
      m_freeSegments[idx + longestSectionLen - 1] = longestSectionLen - len;
      m_freeSegments[idx] = 0;

      if (longestSectionLen > len)
      {
        m_freeSegments[idx + len] &= SectionStartMarker;
      }

      m_segmentTree.update(idx, 0);
      m_segmentTree.update(idx + len, longestSectionLen - len);
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
    // To be implemented
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