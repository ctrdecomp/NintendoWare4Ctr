// Filename: snd_InstancePool.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_InstancePool.h>
#include <nw/ut.h>

/*

Some can be copied from NW4R since NW4C and NW4R are somewhat simliar.

[REFERENCE] https://github.com/doldecomp/ogws/blob/master/src/nw4r/snd/snd_InstancePool.cpp

*/

namespace nw {
namespace snd {
namespace internal {

u32 PoolImpl::CreateImpl(void* buffer, size_t size, u32 stride) 
{
    NW_ASSERT(pBuffer);

    u8* pPtr = static_cast<u8*>(ut::RoundUp(pBuffer, 4));
    stride = ut::RoundUp(stride, 4);

    u32 length = (size - ut::GetOffsetFromPtr(pBuffer, pPtr)) / stride;

    for (u32 i = 0; i < length; i++, pPtr += stride) 
    {
        PoolImpl* pHead = reinterpret_cast<PoolImpl*>(pPtr);
        pHead->m_pNext = m_pNext;
        m_pNext = pHead;
    }

    return length;
}

void PoolImpl::DestroyImpl(void* buffer, size_t size) 
{
    void* pBegin = buffer;
    void* pEnd = static_cast<u8*>(pBegin) + size;

    PoolImpl* pIt = m_pNext;
    PoolImpl* pPrev = this;

    for (; pIt != NULL; pIt = pIt->m_pNext) 
    {
        if (pBegin <= pIt && pIt < pEnd) 
        {
            pPrev->m_pNext = pIt->m_pNext;
        } 
        else 
        {
            pPrev = pIt;
        }
    }
}

int PoolImpl::CountImpl() const 
{
    int num = 0;

    for (PoolImpl* pIt = m_pNext; pIt != NULL; pIt = pIt->m_pNext) 
    {
        num++;
    }

    return num;
}

void* PoolImpl::AllocImpl() 
{
    if (m_pNext == NULL) 
    {
        return NULL;
    }

    PoolImpl* pHead = m_pNext;
    m_pNext = pHead->m_pNext;

    return pHead;
}

void PoolImpl::FreeImpl(void* pElem) 
{
    PoolImpl* pHead = static_cast<PoolImpl*>(pElem);
    pHead->m_pNext = m_pNext;
    m_pNext = pHead;
}

} // namespace internal
} // namespace snd
} // namespace nw