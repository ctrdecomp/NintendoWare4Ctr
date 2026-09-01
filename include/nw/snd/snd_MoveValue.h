#ifndef NW_SND_MOVE_VALUE_H_
#define NW_SND_MOVE_VALUE_H_

namespace nw { 
namespace snd { 
namespace internal {

template <typename ValueType, typename CountType>
class MoveValue
{
public:
    MoveValue(): 
        m_Origin(0), 
        m_Target(0), 
        m_Frame(0), 
        m_Counter(0)
    {
    }

    void InitValue(ValueType value)
    {
        m_Origin  = value;
        m_Target  = value;
        m_Frame   = 0;
        m_Counter = 0;
    }

    void SetTarget(ValueType targetValue, CountType frames)
    {
        m_Origin  = GetValue();
        m_Target  = targetValue;
        m_Frame   = frames;
        m_Counter = 0;
    }

    ValueType GetTarget() const
    {
        return m_Target;
    }

    ValueType GetValue() const
    {
        if (IsFinished())
            return m_Target;

        return static_cast<ValueType>(m_Origin + (m_Target - m_Origin) * ValueType(m_Counter) / ValueType(m_Frame));
    }

    void Update()
    {
        if (m_Counter < m_Frame)
            ++m_Counter;
    }

    bool IsFinished() const
    {
        return m_Counter >= m_Frame;
    }

    CountType GetRemainingCount() const
    {
        if (IsFinished())
            return static_cast<CountType>(0);

        return m_Frame - m_Counter;
    }

    ValueType GetTarget() const { return m_Target; }

private:
    ValueType m_Origin;
    ValueType m_Target;
    CountType m_Frame;
    CountType m_Counter;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_MOVE_VALUE_H_
