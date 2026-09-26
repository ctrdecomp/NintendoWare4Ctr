#ifndef NW_SND_FX_REVERB_HI_H_
#define NW_SND_FX_REVERB_HI_H_

#include <nw/snd/snd_FxBase.h>

namespace nw { 
namespace snd {

/* Hey mommy, can we have nn::snd::FxReverb? 
   No son, we have FxReverb at home.

   Fx Reverb at home..
*/

class FxReverb : public FxBase
{
public:
    struct FilterSize
    {
        u32  m_Comb0;
        u32  m_Comb1;
        u32  m_AllPass;
        FilterSize(): 
            m_Comb0(19 * NN_SND_SAMPLES_PER_FRAME), 
            m_Comb1(23 * NN_SND_SAMPLES_PER_FRAME), 
            m_AllPass(13 * NN_SND_SAMPLES_PER_FRAME)
        {
        }
    };

    static FilterSize s_DefaultFilterSize;

    struct Param
    {
        u32 m_EarlyReflectionTime;
        u32 m_FusedTime;
        u32 m_PreDelayTime;
        f32 m_Coloration;
        f32 m_Damping;
        FilterSize* m_pFilterSize;
        f32 m_EarlyGain;
        f32 m_FusedGain;
        bool m_UseHpfDamping;

        Param(): 
            m_EarlyReflectionTime(60),
            m_FusedTime(4000),
            m_PreDelayTime(100),
            m_Damping(0.4f),
            m_pFilterSize(&s_DefaultFilterSize),
            m_EarlyGain(0.6f),
            m_FusedGain(0.4f),
            m_UseHpfDamping(false)
        {
        }
    };

    FxReverb();
    virtual ~FxReverb();
    virtual bool Initialize();
    virtual void Finalize();
    virtual void UpdateBuffer(int numChannels, nn::snd::CTR::AuxBusData* data, s32 sampleLength, nw::snd::SampleFormat format, f32 sampleRate, nw::snd::OutputMode mode);

    bool SetParam(const FxReverb::Param& param);
    size_t GetRequiredMemSize();
    bool AssignWorkBuffer(uptr buffer, size_t size);
    void ReleaseWorkBuffer();
    
private:
    struct WorkBuffer
    {
        s32* m_EarlyReflection[4];
        s32* m_PreDelay[4];
        s32* m_CombFilter[4][2];
        s32* m_AllPassFilter[4];
        s32  m_Lpf[4];
    };

    void AllocBuffer();
    void FreeBuffer();
    void InitializeParam();

    Param m_Param;
    uptr m_pBuffer;
    size_t m_BufferSize;
    FilterSize m_FilterSize;
    WorkBuffer m_WorkBuffer;
    
    s32 m_EarlyLength;
    s32 m_EarlyPos;
    
    s32 m_PreDelayLength;
    s32 m_PreDelayPos;
    
    s32 m_CombFilterLength[2];
    s32 m_CombFilterPos[2];
    s32 m_CombFilterCoef[2];
    
    s32 m_AllPassFilterLength;
    s32 m_AllPassFilterPos;
    s32 m_AllPassFilterCoef;
    
    s32 m_LastLpfOut[4];

    s32 m_EarlyGain;
    s32 m_FusedGain;
    s32 m_LpfCoef1;
    s32 m_LpfCoef2;

    u32 m_EarlyReflectionTimeAtInitialize;
    u32 m_PreDelayTimeAtInitialize;
    FilterSize m_FilterSizeAtInitialize;
    bool m_IsEnableSurroundAtInitialize;

    u8 m_ProcessChannelCount;
    bool m_IsActive;
};

} // namespace nw
} // namespace snd

#endif // NW_SND_FX_REVERB_HI_H_
