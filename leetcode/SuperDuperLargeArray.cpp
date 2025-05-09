#include <iostream>
#include <math.h>
#include <stdint.h>
#include <concepts>
#include <stdexcept>
#include <functional>
#include <optional>
#include <string.h>

// SizeType should be an unsigned integral type
template <class SizeType>
  requires std::unsigned_integral<SizeType>
struct SyncIOReadBuffer
{
  typedef std::function<SizeType(char *, const SizeType &)> IOInterface;
  enum class LastOperation
  {
    COPY,
    PASTE,
    NONE
  };

  /**
   *  Constructor
   *  @param size Size of the Buffer
   *              throws if size is 0
   **/
  SyncIOReadBuffer(const SizeType &size) : m_readBuff(reinterpret_cast<char *>(malloc(size))),
                                           m_tail(0),
                                           m_head(0),
                                           m_size(size),
                                           m_lastOperation(LastOperation::NONE)
  {
    if (!size)
    {
      throw std::invalid_argument("size should  be passed as a positive integer");
    }
  }

  /**
   * Read some bytes from the provided IOInterface
   * @param out         The memory to read the bytes into
   * @param len         The max no. of b ytes to read
   * @param ioInterface The sysnchronous IOInterface to read bytes from,
   *                    it's an std::function<SizeType(char *, const SizeType &)>
   *
   * @return            No. of bytes actually read from the IOInterface
   **/
  SizeType read(char *const &out,
                const SizeType &len,
                const IOInterface &ioInterface)
  {
    SizeType ret = 0;
    if (occupiedBytes() >= len)
    {
      copy(out, len);
      ret = len;
    }
    else if (paste(ioInterface))
    {
      SizeType occBytes = occupiedBytes();
      if (occBytes >= len)
      {
        copy(out, len);
        ret = len;
      }
      else
      {
        copy(out, occBytes);
        ret = occBytes;

        while (ret < len && paste(ioInterface))
        {
          // if remaining length to copy, i.e, len - ret <= occupiedBytes(),
          // then copy only the remaining Len, otherwise copy all the occupied
          // bytes and continue
          uint32_t toCopy = std::min(occupiedBytes(), len - ret);
          copy(out + ret, toCopy);
          ret += toCopy;
        }
      }
    }

    return ret;
  }

  /**
   * Read bytes from the provided IOInterface until the character provided
   * as 'ender' is met, or the ioInterface reads 0 bytes.
   * It assumes that if the ioInterface reads 0 bytes, then it won't read
   * anymore bytes thereafter
   *
   * @param out         The memory to read the bytes into
   * @param ioInterface The sysnchronous IOInterface to read bytes from,
   *                    it's an std::function<SizeType(char *, const SizeType &)>
   * @param ender       The character marking the end of this read
   *
   * @return            No. of bytes actually read from the IOInterface
   **/
  SizeType readUntil(char *const &out,
                     const IOInterface &ioInterface,
                     const char &ender)
  {
    SizeType ret = 0;
    SizeType occBytes = occupiedBytes();
    if (!occBytes)
    {
      occBytes = paste(ioInterface);
    }

    if (occBytes)
    {
      auto len = findLengthTill(ender);
      // Found ender
      if (len)
      {
        copy(out, *len);
        ret = *len;
      }
      // Didn't find the ender
      else
      {
        // Source the data from IO Interface
        do
        {
          occBytes = occupiedBytes();
          copy(out + ret, occBytes);
          ret += occBytes;
        } while (paste(ioInterface) && !(len = findLengthTill(ender)));

        if (len)
        {
          copy(out + ret, *len);
          ret += *len;
        }
      }
    }

    return ret;
  }

  /**
   * Read bytes from the provided IOInterface until the character satisfying
   * the 'ender' predicate is read, or the ioInterface reads 0 bytes.
   * It assumes that if the ioInterface reads 0 bytes, then it won't read
   * anymore bytes thereafter
   *
   * @param out         The memory to read the bytes into
   * @param ioInterface The sysnchronous IOInterface to read bytes from,
   *                    it's an std::function<SizeType(char *, const SizeType &)>
   * @param ender       The predicate detrmining whether a character qualifies
   *                    as end of the read
   * @return            No. of bytes actually read from the IOInterface
   **/
  SizeType readUntil(char *const &out,
                     const IOInterface &ioInterface,
                     const std::function<bool(const char &)> &ender)
  {
    SizeType ret = 0;
    SizeType occBytes = occupiedBytes();
    if (!occBytes)
    {
      occBytes = paste(ioInterface);
    }

    if (occBytes)
    {
      auto len = findLengthTill(ender);
      // Found ender
      if (len)
      {
        copy(out, *len);
        ret = *len;
      }
      // Didn't find the ender
      else
      {
        // Source the data from IO Interface
        do
        {
          occBytes = occupiedBytes();
          copy(out + ret, occBytes);
          ret += occBytes;
        } while (paste(ioInterface) && !(len = findLengthTill(ender)));

        if (len)
        {
          copy(out + ret, *len);
          ret += *len;
        }
      }
    }

    return ret;
  }

  std::optional<SizeType> findLengthTill(const char &ender)
  {
    std::optional<SizeType> ret;
    SizeType occBytes = occupiedBytes();

    SizeType offset = 0;
    for (;
         offset < occBytes && ender != m_readBuff[(m_tail + offset) % m_size];
         ++offset)
      ;

    if (offset < occBytes)
    {
      ret = offset + 1;
    }

    return ret;
  }

  std::optional<SizeType> findLengthTill(const std::function<bool(const char &)> &ender)
  {
    std::optional<SizeType> ret;
    SizeType occBytes = occupiedBytes();

    SizeType offset = 0;
    for (;
         offset < occBytes && !ender(m_readBuff[(m_tail + offset) % m_size]);
         ++offset)
      ;

    if (offset < occBytes)
    {
      ret = offset + 1;
    }

    return ret;
  }

  bool empty()
  {
    return occupiedBytes() == 0;
  }

  bool full()
  {
    return freeBytes() == 0;
  }

  SizeType size()
  {
    return occupiedBytes();
  }

  SizeType capacity()
  {
    return m_size;
  }

  SizeType vacancy()
  {
    return freeBytes();
  }

  ~SyncIOReadBuffer()
  {
    free(m_readBuff);
  }

  // Non copyable-assignable, Non moveable-move assinable for the reasons of
  // Simplicity
  SyncIOReadBuffer(const SyncIOReadBuffer &) = delete;
  SyncIOReadBuffer &operator=(const SyncIOReadBuffer &) = delete;
  SyncIOReadBuffer(SyncIOReadBuffer &&) = delete;
  SyncIOReadBuffer &operator=(SyncIOReadBuffer &&) = delete;

private:
  /**
   * Copy some bytes into the provided outBuffer
   * Assumes that len <= occupiedBytes, so the caller of this function has to
   * take care of that. Handles the scerio when the memory
   * to be copied is fragmented
   *
   * @param out         The memory to copy the bytes into
   * @param len         No. of bytes to read into the target
   *
   **/
  void copy(char *const &out, const SizeType &len)
  {
    if (!len)
    {
      return;
    }

    // Case 1: m_tail < m_head:
    // Before:
    //                    len
    //                 <-------->
    // m_readBuff |.........................................|
    //                 ↑                  ↑
    //                 m_tail             m_head
    //
    // After:
    //                    len
    //                 <-------->
    // m_readBuff |.........................................|
    //                           ↑        ↑
    //                           m_tail   m_head
    //
    // Case 2: m_tail > m_head but len <= m_size - m_tail:
    // Before:
    //                                        len
    //                                    <-------->
    // m_readBuff |....................................|
    //                 ↑                  ↑
    //                 m_head             m_tail
    //
    // After:
    //                                      len
    //                                    <-------->
    // m_readBuff |....................................|
    //                           ↑                 ↑
    //                           m_head            m_tail
    //
    // Case 3: m_tail > m_head and len > m_size - m_tail:
    // Before:
    //                                             len
    //             ---->                          <----
    // m_readBuff |....................................|
    //                            ↑               ↑
    //                         m_head             m_tail
    //
    // After:
    //                                             len
    //             ---->                          <----
    // m_readBuff |....................................|
    //                  ↑         ↑
    //                  m_tail    m_head

    if (m_tail < m_head ||        //  Case 1
        len <= (m_size - m_tail)) //  Case 2
    {
      memcpy(out, m_readBuff + m_tail, len);
      m_tail = (m_tail + len) % m_size;
    }
    else // case 3
    {
      const SizeType l1 = m_size - m_tail;
      const SizeType l2 = len - l1;
      memcpy(out, m_readBuff + m_tail, l1);
      memcpy(out + l1, m_readBuff, l2);
      m_tail = l2;
    }

    m_lastOperation = LastOperation::COPY;
    if (!occupiedBytes())
    {
      m_head = m_tail = 0;
    }
  }

  // Read from IOInterface, the max amount of bytes
  // possible(i.e. freeBytes())
  // Takes care of the scenario when the available memory is fragmented,
  // by making separate io call for each fragment
  SizeType paste(const IOInterface &ioInterface)
  {
    // Case 1: m_head = 0:
    // Before:
    //
    // m_readBuff |..................................................|
    //             ↑                                       ↑
    //             m_head                                  m_tail
    //
    // After(read some bytes from the interface):
    //
    //                 bytesReadFromIOInterface
    //             <----------------------------->
    // m_readBuff |..................................................|
    //                                            ↑        ↑
    //                                            m_head   m_tail
    //
    // Case 2: m_tail < m_head:
    // Before:
    //                                                   <-------------lengthTillEnd----------->
    // m_readBuff |.............................................................................|
    //                                ↑                  ↑
    //                                m_tail             m_head
    //
    // After(bytesReadFromIOInterface = lengthTillEnd): turns into case 1
    //                                                   <------bytesReadFromIOInterface------->
    // m_readBuff |.............................................................................|
    //             ↑                  ↑
    //             m_head             m_tail
    //
    //                                  OR
    //
    // After(bytesReadFromIOInterface < lengthTillEnd): turns into case 1
    //                                                   <------bytesReadFromIOInterface----->
    // m_readBuff |.............................................................................|
    //                                ↑                                                       ↑
    //                                m_tail                                                  m_head

    SizeType bytesReadFromIOInterface = 0;
    if (auto free = freeBytes(); free)
    {
      SizeType lengthTillEnd = m_size - m_head;

      // if freeBytes() < lengthTillEnd, then free memory contiguous ans a single read
      // should be done, toRead should be freeBytes()
      // if freeBytes() > lengthTillEnd, then free memory is fragmanted,
      // 2 reads will be requires, 1st read should be "lengthTillEnd" bytes
      // after that if, the ioInterface reads "lengthTillEnd" bytes, then it will
      // turn into case 1, otherwise the algorithm stops
      SizeType toRead = std::min(lengthTillEnd, free);

      bytesReadFromIOInterface = pasteFromInterface(ioInterface, toRead);
      free -= bytesReadFromIOInterface;
      if (bytesReadFromIOInterface == toRead && free) // Case 1
      {
        bytesReadFromIOInterface += pasteFromInterface(ioInterface, free);
      }
    }

    return bytesReadFromIOInterface;
  }

  // Read from IOInterface, assumes that the provided memory is
  // contiguous
  SizeType pasteFromInterface(const IOInterface &ioInterface, const SizeType &len)
  {
    SizeType ret = 0;
    if (len &&
        (ret = ioInterface(m_readBuff + m_head, len)))
    {
      m_head = (m_head + ret) % m_size;
      m_lastOperation = LastOperation::PASTE;
    }

    return ret;
  }

  SizeType occupiedBytes()
  {
    if (m_tail == m_head)
    {
      // In this case m_lastOperation == LastOperation::PASTE means that the
      // buffer is completely onoccupied, otherwise it's completely free
      return m_lastOperation == LastOperation::PASTE ? m_size : 0;
    }
    else if (m_tail < m_head)
    {
      return m_head - m_tail;
    }
    else
    {
      return m_size - (m_tail - m_head);
    }
  }

  SizeType freeBytes()
  {
    return m_size - occupiedBytes();
  }

  LastOperation m_lastOperation;
  SizeType m_tail;
  SizeType m_head;
  const SizeType m_size;
  char *const m_readBuff;
};

template <class SizeType>
  requires std::unsigned_integral<SizeType>
struct SyncIOLazyWriteBuffer
{
  typedef std::function<SizeType(const char *, const SizeType &)> IOInterface;
  enum class LastOperation
  {
    FLUSH,
    PUT,
    NONE
  };

  /**
   *  Constructor
   *  @param size         Size of the Buffer
   *                      throws if size is 0
   *  @param ioInterface  The synchronous IOInterface to write bytes to,
   *                      it's an std::function<SizeType(const char*, const SizeType&)>
   **/
  SyncIOLazyWriteBuffer(const SizeType &size, const IOInterface &ioInterface) : m_outBuff(reinterpret_cast<char *>(malloc(size))),
                                                                                m_tail(0),
                                                                                m_head(0),
                                                                                m_size(size),
                                                                                m_ioInterface(ioInterface),
                                                                                m_lastOperation(LastOperation::NONE)
  {
    if (!size)
    {
      throw std::invalid_argument("size should  be passed as a positive integer");
    }
  }

  /**
   *  Write data to interface
   *  It puts the data in the buffer and delays the IO call for as long as
   *  it can. If the buffer is already full or there is insufficint space in
   *  the buffer to hold entire data, it will need to flush the buffered data
   *  to the ioInterface
   *
   *  @param size         Size of the Buffer
   *                      throws if size is 0
   *  @param ioInterface  The synchronous IOInterface to write bytes to,
   *                      it's an std::function<SizeType(const char*, const SizeType&)>
   **/
  SizeType write(const char *out, const SizeType &len)
  {
    SizeType remainingLen = len;
    SizeType ret = 0;
    bool flushfailed = false;
    for (SizeType freeBytesBeforePut = freeBytes();
         freeBytesBeforePut < remainingLen && !flushfailed;
         flushfailed = !flush(), freeBytesBeforePut = freeBytes())
    {
      put(out, freeBytesBeforePut);
      remainingLen -= freeBytesBeforePut;
      out += freeBytesBeforePut;
      ret += freeBytesBeforePut;
    }

    SizeType bytestoPut = std::min(remainingLen, freeBytes());
    put(out, bytestoPut);
    ret += bytestoPut;
    return ret;
  }

  /*
   * Put all of the buffered data to the ioInterface
   */
  SizeType flush()
  {
    if (!occupiedBytes())
    {
      return 0;
    }

    SizeType ret = 0;
    if (m_tail <= m_head)
    {
      ret = m_ioInterface(m_outBuff + m_tail, occupiedBytes());
      m_tail += (m_tail + ret) % m_size;
    }
    else
    {
      SizeType lengthTillEnd = m_size - m_tail;
      if (ret = m_ioInterface(m_outBuff + m_tail, lengthTillEnd) == lengthTillEnd)
      {
        m_tail = m_ioInterface(m_outBuff, m_head);
        ret += m_tail;
      }
    }

    if (ret)
    {
      if (m_tail == m_head)
      {
        m_tail = m_head = 0;
      }
      m_lastOperation = LastOperation::FLUSH;
    }

    return ret;
  }

  ~SyncIOLazyWriteBuffer()
  {
    flush();
    free(m_outBuff);
  }

  SyncIOLazyWriteBuffer(const SyncIOLazyWriteBuffer &) = delete;
  SyncIOLazyWriteBuffer &operator=(const SyncIOLazyWriteBuffer &) = delete;
  SyncIOLazyWriteBuffer(SyncIOLazyWriteBuffer &&) = delete;
  SyncIOLazyWriteBuffer &operator=(SyncIOLazyWriteBuffer &&) = delete;

private:
  /**
   *  Copy some data to the internal buffer
   *
   *  @param outData  The address from which the bytes are to be copied
   *                  into the internal buffer
   *  @param len      No. of bytes to be copied
   *
   *  @remark          Assumes that len is <= the available memory,
   *                  So the caller has to take care of that
   **/
  void put(const char *outData, const SizeType &len)
  {
    if (!len)
    {
      return;
    }

    if (m_head < m_tail ||
        len <= m_size - m_head)
    {
      memcpy(m_outBuff + m_head, outData, len);
      m_head = (m_head + len) % m_size;
    }
    else
    {
      const SizeType l1 = m_size - m_head;
      const SizeType l2 = len - l1;
      memcpy(m_outBuff + m_head, outData, l1);
      memcpy(m_outBuff, outData + l1, l2);
      m_head = l2;
    }

    m_lastOperation = LastOperation::PUT;
  }

  SizeType occupiedBytes()
  {
    if (m_tail == m_head)
    {
      return m_lastOperation == LastOperation::PUT ? m_size : 0;
    }
    else if (m_tail < m_head)
    {
      return m_head - m_tail;
    }
    else
    {
      return m_size - (m_tail - m_head);
    }
  }

  SizeType freeBytes()
  {
    return m_size - occupiedBytes();
  }

  LastOperation m_lastOperation;
  const IOInterface m_ioInterface;
  SizeType m_tail;
  SizeType m_head;
  const SizeType m_size;
  char *const m_outBuff;
};

uint64_t sigma(const uint64_t& n)
{
  return n % 2? n * ((n+1)/2) : (n/2) * (n+1); 
}
uint64_t partialSigma(const uint64_t& k, const uint64_t& n)
{
  return sigma(k + n - 1) - sigma(k - 1);
}

uint64_t partialSigmaDiffSplit(const uint64_t& k, const uint64_t& n, const uint64_t& i)
{
  const uint64_t leftSum = partialSigma(k, i);
  const uint64_t rightSum = partialSigma(k + i, n - i);
  return  leftSum > rightSum?
          leftSum - rightSum:
          rightSum - leftSum;

}

uint64_t logic(const uint64_t& n, const uint64_t& k)
{

  if(2 == n) return 1;

  double nd = n;
  double kd = k;

  double a = -1;
  double b = 1 - (2*kd);
  double c = ((kd + nd) * (kd + nd - 1) - kd * (kd - 1)) / 2;
  //double c = (pow(kd + nd, 2) - kd) / 2;
  double d = pow(b, 2) - (4*a*c);
  double root =(-b - sqrt(d)) /(2*a);
  uint64_t floor = (uint64_t)root;
  uint64_t ceil = floor + 1;
  uint64_t res1 = partialSigmaDiffSplit(kd, nd, floor);
  uint64_t res2 = partialSigmaDiffSplit(kd, nd, ceil);
  
  return std::min(res1, res2);
}

int main()
{

  auto io_console_reader =
  [](char *out, const uint32_t len)
  {
    std::cin.read(out, len);
    return std::cin.gcount();
  };
  auto io_console_writer =
  [](const char *out, const uint32_t len)
  {
    std::cout.write(out, len);
    return len;
  };

  uint64_t buffSize = 1024;
  SyncIOReadBuffer<uint64_t> smartReadBuffer(buffSize);
  SyncIOLazyWriteBuffer<uint64_t> smartWriteBuffer(buffSize, io_console_writer);

  char input[128];
  smartReadBuffer.readUntil(input, io_console_reader, '\n');
  uint32_t numTestCases;
  sscanf(input, "%llu", &numTestCases);

  for (uint64_t i = 0; i < numTestCases; ++i)
  {
    char in[128];
    char out[128];
    int n, k;
    smartReadBuffer.readUntil(in, io_console_reader, '\n');
    sscanf(in, "%u %u", &n, &k);
    uint64_t answer =  logic(n, k);
    smartWriteBuffer.write(out, sprintf(out, "%llu\n", answer));
  }

  return 0;
}