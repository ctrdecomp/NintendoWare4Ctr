// Filename: snd_CurveLfo.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_CurveLfo.h>
#include <nw/assert.h>

namespace nw {
namespace snd {
namespace internal {

void CurveLfoParam::Initialize()
{
    depth  = 0.0f;
    range  = 1;
    speed  = 6.25f;
    delay  = 0;
}

void CurveLfo::Reset()
{
    m_Counter = 0.0f;
    m_DelayCounter = 0;
}

void CurveLfo::Update(int msec)
{
    if (m_DelayCounter < m_Param.delay)
    {
        if (m_DelayCounter + msec <= m_Param.delay)
        {
            m_DelayCounter += msec;
            return;
        }
        else
        {
            msec -= m_Param.delay - m_DelayCounter;
            m_DelayCounter = m_Param.delay;
        }
    }

    m_Counter += m_Param.speed * msec / 1000.0f;
    m_Counter -= static_cast<int>(m_Counter);
}

f32 CurveLfo::GetValue() const
{
    if (m_Param.depth == 0.0f) 
        return 0.0f;
    if (m_DelayCounter < m_Param.delay) 
        return 0.0f;

    f32 value = GetSinIdx(static_cast<int>(m_Counter * TABLE_SIZE * 4)) / 127.0f;
    value *= m_Param.depth;
    value *= m_Param.range ;

    return value;
}

s8 CurveLfo::GetSinIdx(int index)
{
    static const s8 sinTable[TABLE_SIZE + 1] =
    {
        0,    6,   12,   19,   25,   31,   37,   43,
        49,   54,   60,   65,   71,   76,   81,   85,
        90,   94,   98,  102,  106,  109,  112,  115,
        117,  120,  122,  123,  125,  126,  126,  127,
        127
    };

    NW_MINMAXLT_ASSERT(index, 0, PERIOD);

    if (index < TABLE_SIZE)
    {
        return sinTable[index];
    }
    if (index < TABLE_SIZE * 2)
    {
        return sinTable[TABLE_SIZE - (index - TABLE_SIZE)];
    }
    if (index < TABLE_SIZE * 3)
    {
        return static_cast<s8>(- sinTable[index - TABLE_SIZE * 2]);
    }
    return static_cast<s8>(- sinTable[TABLE_SIZE - (index - TABLE_SIZE * 3)]);
}

} // namespace internal
} // namespace snd
} // namespace nw

