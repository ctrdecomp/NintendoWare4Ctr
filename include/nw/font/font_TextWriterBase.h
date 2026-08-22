#pragma once

#include <cstddef>
#include <cwchar>
#include <cstdio>
#include <cstring>
#include <cfloat>
#include <nn/types.h>

#include <nw/ut/ut_Color.h>
#include <nw/font/font_Font.h>
#include <nw/font/font_CharWriter.h>
#include <nw/font/font_TagProcessorBase.h>

namespace nw {
namespace font {

template <typename CharType>
class TextWriterBase : public CharWriter{
public:
    enum PositionFlag{
        HORIZONTAL_ALIGN_LEFT    = 0x0,
        HORIZONTAL_ALIGN_CENTER  = 0x1,
        HORIZONTAL_ALIGN_RIGHT   = 0x2,
        HORIZONTAL_ALIGN_MASK    = 0x3,

        HORIZONTAL_ORIGIN_LEFT   = 0x00,
        HORIZONTAL_ORIGIN_CENTER = 0x10,
        HORIZONTAL_ORIGIN_RIGHT  = 0x20,
        HORIZONTAL_ORIGIN_MASK   = 0x30,

        VERTICAL_ORIGIN_TOP      = 0x000,
        VERTICAL_ORIGIN_MIDDLE   = 0x100,
        VERTICAL_ORIGIN_BOTTOM   = 0x200,
        VERTICAL_ORIGIN_BASELINE = 0x300,
        VERTICAL_ORIGIN_MASK     = 0x300
    };

    enum ContextFlag{
        CONTEXT_NO_CHAR_SPACE = 0x1
    };

    static const u32 DEFAULT_DRAWFLAG = HORIZONTAL_ALIGN_LEFT | HORIZONTAL_ORIGIN_LEFT | VERTICAL_ORIGIN_TOP;

    TextWriterBase();
    ~TextWriterBase();

    void SetLineSpace(f32 space) { this->mLineSpace = space; }
    f32  GetLineSpace() const { return this->mLineSpace; }

    void SetLineHeight(f32 height);
    f32 GetLineHeight() const;

    void SetCharSpace(f32 space) { this->mCharSpace = space; }
    f32 GetCharSpace() const { return this->mCharSpace; }

    void SetTabWidth(int tabWidth) { this->mTabWidth = tabWidth; }
    int GetTabWidth() const { return this->mTabWidth; }

    void SetWidthLimit(f32 limit) { this->mWidthLimit = limit; }
    f32 GetWidthLimit() const { return this->mWidthLimit; }
    void ResetWidthLimit() { SetWidthLimit(FLT_MAX); }

    void SetDrawFlag(u32 flags) { this->mDrawFlag = flags; }
    u32 GetDrawFlag() const { return this->mDrawFlag; }

    void SetTagProcessor(TagProcessorBase<CharType>* tagProcessor){
        NN_POINTER_ASSERT(tagProcessor);
        this->mTagProcessor = tagProcessor;
    }

    TagProcessorBase<CharType>& GetTagProcessor() const { return *this->mTagProcessor; }
    void ResetTagProcessor() { this->mTagProcessor = &sDefaultTagProcessor; }

    f32 CalcFormatStringWidth(const CharType* format, ...) const;

    f32 CalcStringWidth(const CharType* str) const{
        NN_POINTER_ASSERT(str);
        return CalcStringWidth(str, StrLen(str));
    }

    f32 CalcStringWidth(const CharType* str, int length) const;

    f32 CalcFormatStringHeight(const CharType* format, ...) const;

    f32 CalcStringHeight(const CharType* str) const{
        NN_POINTER_ASSERT(str);
        return CalcStringHeight(str, StrLen(str));
    }

    f32 CalcStringHeight(const CharType* str, int length) const;

    void CalcFormatStringRect(ut::Rect* pRect, const CharType* format, ...) const;

    void CalcVStringRect(ut::Rect* pRect, const CharType* format, std::va_list args) const;

    void CalcStringRect(ut::Rect* pRect, const CharType* str) const{
        NN_POINTER_ASSERT(pRect);
        NN_POINTER_ASSERT(str);
        CalcStringRect(pRect, str, StrLen(str));
    }

    void CalcStringRect(ut::Rect* pRect, const CharType* str, int length) const;

    f32 Printf(const CharType* format, ...);
    f32 VPrintf(const CharType* format, std::va_list args);

    f32 Print(const CharType* str){
        NN_POINTER_ASSERT(str);
        return Print(str, StrLen(str));
    }

    f32 Print(const CharType* str, int length);

    static void* SetBuffer(std::size_t size){
        void* oldBuffer = sFormatBuffer;
        sFormatBuffer = NULL;
        sFormatBufferSize = size;
        return oldBuffer;
    }

    static void* SetBuffer(CharType* buffer, std::size_t size){
        NN_POINTER_ASSERT(buffer);
        void* oldBuffer = sFormatBuffer;
        sFormatBuffer = buffer;
        sFormatBufferSize = size;
        return oldBuffer;
    }

    static const void* GetBuffer() { return sFormatBuffer; }
    static std::size_t GetBufferSize() { return sFormatBufferSize; }

    static int VSNPrintf(char* buffer, std::size_t count, const char* format, std::va_list arg){
        using namespace std;
        return vsnprintf(buffer, count, format, arg);
    }

    static int VSNPrintf(wchar_t* buffer, std::size_t count, const wchar_t* format, std::va_list arg){
        using namespace std;
        return vswprintf(buffer, count, format, arg);
    }

    static int StrLen(const char* str) { return static_cast<int>(std::strlen(str)); }
    static int StrLen(const wchar_t* str) { return static_cast<int>(std::wcslen(str)); }

    using CharWriter::Print;

private:
    typedef TagProcessorBase<CharType> TagProcessor;
    typedef const CharType*            StreamType;

    static const int DEFAULT_FORMAT_BUFFER_SIZE = 256;

    f32 CalcLineWidth(StreamType str, int length);
    bool CalcLineRectImpl(ut::Rect* pRect, StreamType* pStr, int length);
    void CalcStringRectImpl(ut::Rect* pRect, StreamType str, int length);
    f32 PrintImpl(StreamType str, int length);
    f32 AdjustCursor(f32* pXOrigin, f32* pYOrigin, StreamType str, int length);

    bool IsDrawFlagSet(u32 mask, u32 flag) const{
        return (this->mDrawFlag & mask) == flag;
    }

    static CharType*    sFormatBuffer;
    static std::size_t  sFormatBufferSize;
    static TagProcessor sDefaultTagProcessor;

    f32 mWidthLimit;
    f32 mCharSpace;
    f32 mLineSpace;
    int mTabWidth;
    u32 mDrawFlag;
    TagProcessor* mTagProcessor;
};

} // namespace font
} // namespace nw