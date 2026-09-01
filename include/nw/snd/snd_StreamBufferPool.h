#ifndef NW_SND_STREAM_BUFFER_POOL_H_
#define NW_SND_STREAM_BUFFER_POOL_H_

#include <nw/snd/snd_Global.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

class StreamBufferPool
{
public:
    void Initialize(void* pBuffer, size_t size, int blockCount);
    void Finalize();

    void* Alloc();
    void Free(void* pPtr);

    size_t GetBlockSize() const { return m_BlockSize; }

private:
    static const int BLOCK_MAX = 32;
    static const int BIT_PER_BYTE = 8;

    void*   m_pBuffer;
    size_t  m_BufferSize;
    size_t  m_BlockSize;
    int     m_BlockCount;
    int     m_AllocCount;
    u8      m_AllocFlags[BLOCK_MAX / BIT_PER_BYTE];
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_STREAM_BUFFER_POOL_H_
