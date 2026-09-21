// Filename: font_TextWriterBase.cpp
//
// Project: NintendoWare4Ctr

#include <cstdarg>
#include <cstdio>
#include <nn/types.h>
#include <nn/assert.h>
#include <nw/font/font_TextWriterBase.h>
#include <nw/font/font_TagProcessorBase.h>
#include <nw/ut/ut_Inlines.h>

#include <nw/math/math_Types.h>

namespace nw {
namespace font {
namespace{

inline f32 AdjustCenterValue(f32 value)
{
    return math::FCeil(value * 0.5f);
}

}

template <typename CharType>
CharType* TextWriterBase<CharType>::s_FormatBuffer = NULL;

template <typename CharType>
std::size_t TextWriterBase<CharType>::s_FormatBufferSize = DEFAULT_FORMAT_BUFFER_SIZE;

template <typename CharType>
TagProcessorBase<CharType> TextWriterBase<CharType>::s_DefaultTagProcessor;


template <typename CharType>
TextWriterBase<CharType>::TextWriterBase(): CharWriter(),
    m_WidthLimit(FLT_MAX),
    m_CharSpace(0),
    m_LineSpace(0),
    m_TabWidth (4),
    m_DrawFlag(DEFAULT_DRAWFLAG),
    m_TagProcessor(&s_DefaultTagProcessor) {}

template <typename CharType>
TextWriterBase<CharType>::~TextWriterBase() { }

template <typename CharType>
f32 TextWriterBase<CharType>::CalcStringHeight(StreamType str,int length) const
{
    ut::Rect rect;
    CalcStringRect(&rect, str, length);
    return rect.GetHeight();
}

template <typename CharType>
void TextWriterBase<CharType>::CalcStringRect(ut::Rect* pRect,StreamType str,int length) const
{
    TextWriterBase<CharType> self = *this;
    self.CalcStringRectImpl(pRect, str, length);
}

template <typename CharType>
f32 TextWriterBase<CharType>::Print(StreamType  str,int length)
{
    TextWriterBase<CharType> myCopy = *this;

    f32 width = myCopy.PrintImpl(str, length);

    SetCursor(myCopy.GetCursorX(), myCopy.GetCursorY());

    return width;
}

template <typename CharType>
f32 TextWriterBase<CharType>::CalcLineWidth(StreamType str,int length)
{
    ut::Rect rect;
    TextWriterBase<CharType> myCopy = *this;

    myCopy.SetCursor(0, 0);
    myCopy.CalcLineRectImpl(&rect, &str, length);

    return rect.GetWidth();
}

template <typename CharType>
bool TextWriterBase<CharType>::CalcLineRectImpl(ut::Rect*   pRect,StreamType* pStr,int         length)
{
    const StreamType str            = *pStr;
    const StreamType end            = str + length;
    const bool bUseLimit            = m_WidthLimit < FLT_MAX;
    PrintContext<CharType> context(this, str, 0, 0, 0);
    f32 limitLeft                   = 0;
    f32 limitRight                  = 0;
    bool bCharSpace                 = false;
    bool bOverLimit                 = false;
    StreamType prevStreamPos        = NULL;
    ut::Rect prevRect;

    NN_POINTER_ASSERT(GetFont());
    CharStrmReader reader = GetFont()->GetCharStrmReader(CharType(0));

    f32 lineFeed = GetLineHeight() - GetLineSpace();

    pRect->left     = 0;
    pRect->right    = 0;
    pRect->top      = math::Min(0.0f, lineFeed);
    pRect->bottom   = math::Max(0.0f, lineFeed);
    prevRect = *pRect;

    reader.Set(str);
    prevStreamPos = NULL;

    for (CharCode code = reader.Next();reinterpret_cast<StreamType>(reader.GetCurrentPos()) <= end;)
    {
        if (code < ' ')
        {

            typename TagProcessor::Operation operation;
            ut::Rect rect(limitRight, 0, 0, 0);

            context.str = reinterpret_cast<StreamType>(reader.GetCurrentPos());
            context.flags = 0;
            context.flags |= bCharSpace ? 0: CONTEXT_NO_CHAR_SPACE;
            SetCursorX(limitRight);

            if (bUseLimit&& code != '\n' && prevStreamPos != NULL)
            {
                PrintContext<CharType> context2 = context;
                TextWriterBase<CharType> myCopy = *this;
                ut::Rect rect2;

                context2.writer = &myCopy;
                operation = m_TagProcessor->CalcRect(&rect2, code, &context2);

                if ( rect2.GetWidth() > 0.0f&& (myCopy.GetCursorX() - context.xOrigin > m_WidthLimit))
                {
                    bOverLimit = true;
                    code       = '\n';
                    reader.Set(prevStreamPos);
                    continue;
                }
            }
            operation = m_TagProcessor->CalcRect(&rect, code, &context);

            NN_POINTER_ASSERT(context.str);
            reader.Set(context.str);

            pRect->left     = math::Min(pRect->left,      rect.left);
            pRect->top      = math::Min(pRect->top,       rect.top);
            pRect->right    = math::Max(pRect->right,     rect.right);
            pRect->bottom   = math::Max(pRect->bottom,    rect.bottom);
            limitRight = GetCursorX();

            if (operation == TagProcessor::OPERATION_END_DRAW)
            {
                *pStr += length;
                return false;
            }
            else if (operation == TagProcessor::OPERATION_NO_CHAR_SPACE)
            {
                bCharSpace = false;
            }
            else if (operation == TagProcessor::OPERATION_CHAR_SPACE)
            {
                bCharSpace = true;
            }
            else if (operation == TagProcessor::OPERATION_NEXT_LINE)
            {
                break;
            }
        }
        else{
            f32 crntRight = limitRight;

            if (bCharSpace)
            {
                crntRight += GetCharSpace();
            }

            if (IsWidthFixed())
            {
                crntRight += GetFixedWidth();
            }
            else{
                crntRight += GetFont()->GetCharWidth(code) * GetScaleH();
            }

            if (bUseLimit && prevStreamPos != NULL)
            {
                f32 width = crntRight - limitLeft;
                if (width > this->m_WidthLimit)
                {
                    bOverLimit = true;
                    code       = '\n';
                    reader.Set(prevStreamPos);
                    continue;
                }
            }

            limitRight = crntRight;
            pRect->left  = math::Min(pRect->left,  limitRight);
            pRect->right = math::Max(pRect->right, limitRight);
            bCharSpace = true;
        }

        if (bUseLimit)
        {
            prevStreamPos = reinterpret_cast<StreamType>(reader.GetCurrentPos());
        }

        code = reader.Next();
    }

    *pStr = reinterpret_cast<StreamType>(reader.GetCurrentPos());

    return bOverLimit;
}

template <typename CharType>
void TextWriterBase<CharType>::CalcStringRectImpl(ut::Rect* pRect,StreamType str,int length)
{
    const StreamType end = str + length;
    int remain = length;
    StreamType pos = str;

    pRect->left     = 0;
    pRect->right    = 0;
    pRect->top      = 0;
    pRect->bottom   = 0;

    SetCursor(0, 0);

    do{
        ut::Rect rect;
        this->CalcLineRectImpl(&rect, &pos, remain);
        remain   = (end - pos);

        pRect->left     = math::Min(pRect->left,      rect.left);
        pRect->top      = math::Min(pRect->top,       rect.top);
        pRect->right    = math::Max(pRect->right,     rect.right);
        pRect->bottom   = math::Max(pRect->bottom,    rect.bottom);
    } while (remain > 0);
}

}
}