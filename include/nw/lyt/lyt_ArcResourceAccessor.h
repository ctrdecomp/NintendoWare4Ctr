#pragma once

#include <nw/types.h>

#include <nw/ut/ut_LinkList.h>
#include <nw/lyt/lyt_FontContainer.h>
#include <nw/lyt/lyt_ResourceAccessor.h>
#include <nw/lyt/lyt_TextureContainer.h>
#include <nw/lyt/lyt_Arc.h>
#include <cstddef>

namespace nw{
namespace lyt{
class ArcResourceAccessor : public ResourceAccessor{
public:
    static const int ROOTPATH_MAX = 64;

    ArcResourceAccessor();
    bool Attach(void* archiveStart,const char* resourceRootDirectory);
    void* Detach();

    virtual void* GetResource(ResType resType,const char* name,u32* pSize = 0);
    virtual const TextureInfo GetTexture(const char* name);
    virtual font::Font* GetFont(const char* name);

    bool IsAttached() const{return mArcBuf != 0;}
    const void* GetArchiveDataStart() const{return mArcBuf;}
protected:
    ARCHandle mArcHandle;
    void* mArcBuf;
    FontContainer mFontList;
    TextureContainer mTextureList;
    wchar_t mResRootDir[ROOTPATH_MAX];
    wchar_t mResNameWork[ROOTPATH_MAX];
};
}
}