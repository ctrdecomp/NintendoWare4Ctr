#pragma once

#include <GLES2/gl2.h>
#include <nw/types.h>

namespace nw {
namespace ut {
namespace internal {

class CmdCache{
public:
    CmdCache();
    void Init(void* buffer,u32 size,bool isCopy  = true);
    void Use() const;
    void Add(const u32*  command,u32 size);
    void RoundUp(u8 align);
    void Clear() { this->mCmdBufferSize = 0; }
    void* GetAddr() const { return this->mCmdBuffer; }
    GLsizei GetSize() const { return this->mCmdBufferSize; }
    void Dump(bool asF32 = false,bool showMnemonic = false) const;
    static void Dump(const void* from,const void* to,bool asF32 = false,bool showMnemonic = false);
private:
    static void DumpCommon(const void* from,const void* to,bool asF32,bool showMnemonic);
    u8* mCmdBuffer;
    GLsizei mCmdBufferSize;
    GLsizei mCmdMaxBufferSize;
    bool mIsCopy;
};

}   // namespace internal
}   // namespace ut
}   // namespace nw