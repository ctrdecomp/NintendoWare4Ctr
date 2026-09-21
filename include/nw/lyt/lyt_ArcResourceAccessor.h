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
class ArcResourceAccessor : public ResourceAccessor
{
public:
    static const int ROOTPATH_MAX = 64;

    ArcResourceAccessor();
    bool Attach(void* archiveStart,const char* resourceRootDirectory);
    void* Detach();

    virtual void* GetResource(ResType resType,const char* name,u32* pSize = 0);
    virtual const TextureInfo GetTexture(const char* name);
    virtual font::Font* GetFont(const char* name);

    bool IsAttached() const {return m_ArcBuf != 0;}
    const void* GetArchiveDataStart() const {return m_ArcBuf;}
protected:
    ARCHandle m_ArcHandle;
    void* m_ArcBuf;
    FontContainer m_FontList;
    TextureContainer m_TextureList;
    wchar_t m_ResRootDir[ROOTPATH_MAX];
    wchar_t m_ResNameWork[ROOTPATH_MAX];
};
}
}