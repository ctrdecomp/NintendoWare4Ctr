// Filename: snd_StreamBufferPool.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_StreamBufferPool.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

void StreamBufferPool::Initialize(void* pBuffer, size_t size, int blockCount)
{
    if (blockCount == 0)
    {
        return;
    }

    m_pBuffer = pBuffer;
    m_BufferSize = size;    
    m_BlockCount = blockCount;
    m_BlockSize = ut::RoundDown(size / blockCount, 32);   
    m_AllocCount = 0;

    std::memset(m_AllocFlags, 0, sizeof(m_AllocFlags));

    NW_ASSERTMSG(m_BlockCount <= BLOCK_MAX, "Too large stream buffer size.");
}

void StreamBufferPool::Finalize()
{
    m_pBuffer = NULL;
    m_BufferSize = 0;
    m_BlockSize = 0;
    m_BlockCount = 0;
}

void* StreamBufferPool::Alloc()
{
    if (m_AllocCount >= m_BlockCount) 
    {
        return NULL;
    }

    const int availableByte = ut::RoundUp(m_BlockCount, BIT_PER_BYTE) / BIT_PER_BYTE;

    for(int byteIndex = 0 ; byteIndex < availableByte ; byteIndex++)
    {
        const u8 byte = m_AllocFlags[byteIndex];
        if (byte == 0xff) continue;

        u8 mask = (1 << 0);
        for(int bitIndex = 0 ; bitIndex < BIT_PER_BYTE ; bitIndex++, mask<<=1 )
        {
            if ((byte & mask) == 0)
            {
                m_AllocFlags[byteIndex] |= mask;
                m_AllocCount++;

                const int totalIndex = byteIndex * BIT_PER_BYTE + bitIndex;
                return ut::AddOffsetToPtr( m_pBuffer, m_BlockSize * totalIndex );
            }
        }
    }

    return NULL;
}

void StreamBufferPool::Free(void* pPtr)
{
    PtrDiff offset = ut::GetOffsetFromPtr(m_pBuffer, pPtr);

    const unsigned long totalIndex = static_cast<unsigned long>(offset / m_BlockSize);
    NW_ASSERT(totalIndex < BLOCK_MAX);
    const unsigned long byteIndex = (totalIndex / BIT_PER_BYTE);
    const unsigned long bitIndex  = (totalIndex % BIT_PER_BYTE);

    const int mask = (1 << bitIndex);
    NW_ASSERT((m_AllocFlags[byteIndex] & mask) != 0);
    m_AllocFlags[byteIndex] &= ~mask;
    m_AllocCount--;
    NW_ASSERT(m_AllocCount >= 0);
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw