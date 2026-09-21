#pragma once

#include <nw/types.h>
#include <nw/font/font_ResFontBase.h>

namespace nw{
namespace font{
class ResFont : public ResFontBase
{
public:
    ResFont();
    virtual ~ResFont();

    void RemoveResource();
    void* SetDrawBuffer(void* buffer);
    void* SetResource(void* bfnt);
    static u32 GetDrawBufferSize(const void* bfnt);
    void* GetDrawBuffer() {return GetTextureObjectsBufferPtr();}

    using ResFontBase::IsManaging;
};

}
}