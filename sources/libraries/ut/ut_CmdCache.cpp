// Filename: ut_CmdCache.cpp
//
// Project: NintendoWare4Ctr

#include <nn/assert.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_CmdCache.h>

namespace nw {
namespace ut {
namespace internal {

CmdCache::CmdCache():   
    m_CmdBuffer(NULL),
    m_CmdBufferSize(0),
    m_CmdMaxBufferSize(0),
    m_IsCopy(false) {}

void CmdCache::Init(void* buffer,u32 size,bool isCopy)
    {
    m_CmdBuffer = static_cast<u8*>(ut::RoundUp(buffer, 4));
    void *const bufferEnd = AddOffsetToPtr(buffer, size);
    m_CmdMaxBufferSize = RoundDown(GetOffsetFromPtr(this->m_CmdBuffer, bufferEnd), 8);
    m_IsCopy = isCopy;

    Clear();
}

void CmdCache::Add(const u32* command,u32 size)
{
    std::memcpy(m_CmdBuffer + m_CmdBufferSize,command,size);
    m_CmdBufferSize += size;
}

void CmdCache::RoundUp(u8 align)
{
    const GLsizei alignedBufferSize = ut::RoundUp(this->m_CmdBufferSize, align);

    std::memset(this->m_CmdBuffer + this->m_CmdBufferSize, 0, alignedBufferSize - this->m_CmdBufferSize);
    m_CmdBufferSize = alignedBufferSize;
}

}
}
}