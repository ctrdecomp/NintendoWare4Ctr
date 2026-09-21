#pragma once

#include <nw/types.h>
#include <nw/ut/ut_Rect.h>

namespace nw {
namespace font {

template <typename CharType>
class TextWriterBase;

template <typename CharType>
struct PrintContext
{
    TextWriterBase<CharType>* writer;
    const CharType* str;
    const f32 xOrigin;
    const f32 yOrigin;
    u32 flags;

    PrintContext(TextWriterBase<CharType>* aWriter, const CharType* aStr, const f32 aXOrigin, const f32 aYOrigin, u32 aFlags):
        writer(aWriter),
        str(aStr),
        xOrigin(aXOrigin),
        yOrigin(aYOrigin),
        flags(aFlags) {}
};

template <typename CharType>
class TagProcessorBase
{
public:
    enum Operation
    {
        OPERATION_DEFAULT,
        OPERATION_NO_CHAR_SPACE,
        OPERATION_CHAR_SPACE,
        OPERATION_NEXT_LINE,
        OPERATION_END_DRAW,
        NUM_OF_OPERATION
    };

    TagProcessorBase();
    virtual ~TagProcessorBase();

    virtual Operation Process(u16 code, PrintContext<CharType>* pContext);
    virtual Operation CalcRect(ut::Rect* pRect, u16 code, PrintContext<CharType>* pContext);

private:
    typedef PrintContext<CharType> ContextType;

    void ProcessLinefeed(ContextType* pContext);
    void ProcessTab(ContextType* pContext);
};

}
}