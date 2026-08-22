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
    mCmdBuffer(NULL),
    mCmdBufferSize(0),
    mCmdMaxBufferSize(0),
    mIsCopy(false)
{}

void CmdCache::Init(void* buffer,u32 size,bool isCopy){
    mCmdBuffer = static_cast<u8*>(ut::RoundUp(buffer, 4));
    void *const bufferEnd = AddOffsetToPtr(buffer, size);
    mCmdMaxBufferSize = RoundDown(GetOffsetFromPtr(this->mCmdBuffer, bufferEnd), 8);
    mIsCopy = isCopy;

    Clear();
}

void CmdCache::Add(const u32* command,u32 size){
    std::memcpy(mCmdBuffer + mCmdBufferSize,command,size);
    mCmdBufferSize += size;
}

void CmdCache::RoundUp(u8 align){
    const GLsizei alignedBufferSize = ut::RoundUp(this->mCmdBufferSize, align);

    std::memset(this->mCmdBuffer + this->mCmdBufferSize, 0, alignedBufferSize - this->mCmdBufferSize);
    mCmdBufferSize = alignedBufferSize;
}

}
}
}