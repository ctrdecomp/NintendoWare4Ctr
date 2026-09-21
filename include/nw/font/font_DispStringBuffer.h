#pragma once

#include <nw/font/font_Types.h>
#include <nw/ut/ut_Color.h>
#include <nn/math/math_Vec4.h>

namespace nw {
namespace font {
namespace internal{
    class TextureObject;

struct CharAttribute
{
    nn::math::VEC4 pos;
    ut::Color8 color[internal::TEXTCOLOR_MAX];
    nn::math::VEC4 tex;
    const internal::TextureObject* pTexObj;
};

}

struct DispStringBuffer
{
    const u32 charCountMax;
    u16 charCount;
    u16 drawCharCount;
    u8 generatedCommand;
    u8 padding[3];
    u8* drawFlags;
    u32* commandBuffer;
    u32 commandBufferSize;
    u32 commandBufferCapacity;
    u32 textColorCommandOffset;

    static u32          CalcCommandBufferCapacity(u32 charNum);
    DispStringBuffer(u32 charNum);
    void ClearCommand()
    {
        generatedCommand = false;
    }

    bool IsGeneratedCommand()
    {
        return 0 != generatedCommand;
    }

    bool IsCommandEmpty()
    {
        return 0 == commandBufferSize;
    }

    u16 GetDrawCharCount() const { return this->drawCharCount; }

    void SetDrawCharCount(u16 count)
    {
        drawCharCount = count;
    }
    void ResetDrawCharCount()
    {
        SetDrawCharCount(0xFFFF);
    }
    
    internal::CharAttribute* GetCharAttrs() const
    {
        return reinterpret_cast<internal::CharAttribute*>(
            reinterpret_cast<uptr>(this) + sizeof(*this) );
    }
};

}
}