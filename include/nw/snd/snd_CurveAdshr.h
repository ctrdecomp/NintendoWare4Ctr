#ifndef NW_SND_CURVE_ADSHR_H_
#define NW_SND_CURVE_ADSHR_H_

#include <nn/types.h>

namespace nw {
namespace snd {
namespace internal {

class CurveAdshr
{
public:
    enum Status
    {
        STATUS_ATTACK = 0,
        STATUS_HOLD,
        STATUS_DECAY,
        STATUS_SUSTAIN,
        STATUS_RELEASE
    };

private:
    static const float VOLUME_INIT;
    static const int ATTACK_INIT    = 127;
    static const int HOLD_INIT      = 0;
    static const int DECAY_INIT     = 127;
    static const int SUSTAIN_INIT   = 127;
    static const int RELEASE_INIT   = 127;

    static const int DECIBEL_SQUARE_TABLE_SIZE  = 128;
    static const int CALC_DECIBEL_SCALE_MAX     = 127;
    static const s16 DecibelSquareTable[DECIBEL_SQUARE_TABLE_SIZE];

public:
    CurveAdshr();
    void Initialize(float initDecibel = VOLUME_INIT);
    void Reset(float initDecibel = VOLUME_INIT);

    void Update(int msec);
    f32  GetValue() const;

    Status GetStatus() const { return m_Status; }
    void SetStatus(Status status) { m_Status = status; }

    void SetAttack(int attack);
    void SetHold(int hold);
    void SetDecay(int decay);
    void SetSustain(int sustain);
    void SetRelease(int release);

private:
    static f32 CalcRelease(int release);
    static s16 CalcDecibelSquare(int scale);

    Status m_Status;
    f32 m_Value;
    f32 m_Decay;
    f32 m_Release;
    f32 m_Attack;
    u16 m_Hold;
    u16 m_HoldCounter;
    u8  m_Sustain;
    u8  padding[3];
};


} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_CURVE_ADSHR_H_