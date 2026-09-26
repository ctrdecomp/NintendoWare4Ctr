#ifndef NW_SND_FX_DELAY_H_
#define NW_SND_FX_DELAY_H_

#include <nw/snd/snd_FxBase.h>

namespace nw { 
namespace snd {

class FxDelay : public FxBase
{
public:
    struct Param
    {
        u32 m_DelayTime;
        f32 m_FeedbackGain;
        f32 m_Damping;
        bool m_IsEnableSurround;

        Param(): 
            m_DelayTime(250), 
            m_FeedbackGain(0.4f), 
            m_Damping(1.0f), 
            m_IsEnableSurround(false)
        {
        }
    };

    FxDelay();
    virtual ~FxDelay();
    virtual bool Initialize();
    virtual void Finalize();
    virtual void UpdateBuffer(int numChannels, nn::snd::CTR::AuxBusData* data, s32 sampleLength, nw::snd::SampleFormat format, f32 sampleRate, nw::snd::OutputMode mode);

    bool SetParam(const FxDelay::Param& param);

    bool AssignWorkBuffer(uptr buffer, size_t size);
    void ReleaseWorkBuffer();

    size_t GetRequiredMemSize();

    const Param& GetParam() const
    {
        return m_Param;
    }

private:
    void AllocBuffer();
    void FreeBuffer();
    void InitializeParam();

    struct WorkBuffer
    {
        s32* m_Delay[4];
        s32  m_Lpf[4];
    };

    Param m_Param;
    uptr m_pBuffer;
    size_t m_BufferSize;
    WorkBuffer m_WorkBuffer;
    u32 m_DelayFrames;
    u32 m_CurFrame;
    s32 m_FeedbackGain;
    s32 m_LpfCoef1;
    s32 m_LpfCoef2;
    u32 m_DelayTimeAtInitialize;
    bool m_IsEnableSurroundAtInitialize;
    u8 m_ProcessChannelCount;
    bool m_IsActive;
};

} // namespace nn
} // namespace snd

#endif // NW_SND_FX_DELAY_H_
