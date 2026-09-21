#pragma once

#include <nw/types.h>
#include <nw/Assert.h>

namespace nw {
namespace font {

class CharStrmReader
{
    friend class Font;
public:
    CharStrmReader(const CharStrmReader& rhs):
        m_CharStrm(rhs.m_CharStrm),
        m_ReadFunc(rhs.m_ReadFunc) {}

    ~CharStrmReader() {}

    void Set(const char* stream)
    {
        this->m_CharStrm = stream;
    }

    void Set(const wchar_t* stream)
    {
        this->m_CharStrm = stream;
    }
    const void* GetCurrentPos() const { return m_CharStrm; }
    u16 Next() { return (this->*m_ReadFunc)(); }
private:
    typedef u16 (CharStrmReader::*ReadNextCharFunc)();

    const void* m_CharStrm;
    const ReadNextCharFunc m_ReadFunc;

    explicit CharStrmReader(ReadNextCharFunc func):
        m_CharStrm(NULL),
        m_ReadFunc(func) {}

    template<typename CharType>
    CharType GetChar(int offset = 0) const
    {
        const CharType* charStrm = reinterpret_cast<const CharType*>(m_CharStrm);
        return *(charStrm + offset);
    }

    template<typename CharType>
    void StepStrm(int step = 1)
    {
        const CharType*& charStrm = reinterpret_cast<const CharType*&>(m_CharStrm);
        charStrm += step;
    }

    u16 ReadNextCharUTF8();
    u16 ReadNextCharUTF16();
    u16 ReadNextCharCP1252();
    u16 ReadNextCharSJIS();
};

}
}