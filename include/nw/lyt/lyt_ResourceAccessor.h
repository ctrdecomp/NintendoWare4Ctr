#pragma once

#include <nw/lyt/lyt_Types.h>

namespace nw{
namespace font{
    class Font;
}
namespace lyt{
typedef u32 ResType;

class ResourceAccessor{
protected:
    ResourceAccessor();
public:
    virtual ~ResourceAccessor();
    virtual void* GetResource(ResType resType,const char* name,u32* pSize = 0) = 0;
    virtual font::Font* GetFont(const char* name) = 0;
    virtual const TextureInfo GetTexture(const char* name) = 0;
protected:
    virtual const TextureInfo LoadTexture(const char* name);
    virtual font::Font* LoadFont(const char* name);
};
}
}