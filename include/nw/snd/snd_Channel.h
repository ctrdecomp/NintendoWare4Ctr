#ifndef NW_SND_CHANNEL_H_
#define NW_SND_CHANNEL_H_

#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_Voice.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

class Channel
{
public:
    enum LfoTarget
    {
        LFO_TARGET_PITCH,
        LFO_TARGET_VOLUME,
        LFO_TARGET_PAN
    };

    enum ChannelCallbackStatus
    {
        CALLBACK_STATUS_STOPPED,
        CALLBACK_STATUS_DROP,
        CALLBACK_STATUS_FINISH,
        CALLBACK_STATUS_CANCEL
    };

    typedef void (*ChannelCallback)(Channel* channel,ChannelCallbackStatus status,void* userData);

private:
    static void VoiceCallbackFunc(driver::Voice* voice,Voice::VoiceCallbackStatus status,void* arg);
    void AppendWaveBuffer(const WaveInfo& waveInfo);

    static const int KEY_INIT = 60;
    static const int ORIGINAL_KEY_INIT = 60;
    static const u8  SILENCE_VOLUME_MAX = 255;
    static const u8  SILENCE_VOLUME_MIN = 0;
    static const int WAVE_BUFFER_MAX = 2;
private:
    class Disposer : public DisposeCallback
    {
    public:
        Disposer(Channel* channel) : m_pChannel(channel) {}
        virtual ~Disposer() {}
        virtual void InvalidateData( const void* start, const void* end );
    private:
        Channel* m_pChannel;
    };

    friend class Disposer;

    f32 GetSweepValue() const;
    void InitParam(ChannelCallback callback, u32 callbackData);

    nn::snd::CTR::WaveBuffer m_WaveBuffer[Voice::CHANNEL_MAX][WAVE_BUFFER_MAX];
    nn::snd::CTR::AdpcmContext m_AdpcmContext[Voice::CHANNEL_MAX];
    nn::snd::CTR::AdpcmContext m_AdpcmLoopContext[Voice::CHANNEL_MAX];
    u32 m_LoopStartFrame;
    bool m_LoopFlag;
    Disposer m_Disposer;
    CurveAdshr m_CurveAdshr;
    CurveLfo m_Lfo;
    u8 m_LfoTarget;

    u8 m_PauseFlag;
    u8 m_ActiveFlag;
    u8 m_AllocFlag;
    u8 m_AutoSweep;
    u8 m_ReleasePriorityFixFlag;
    u8 m_IsIgnoreNoteOff;
    u8 m_BiquadType;
    
    f32 m_UserVolume;
    f32 m_UserPitchRatio;
    f32 m_UserPan;
    f32 m_UserSurroundPan;
    f32 m_UserLpfFreq;
    f32 m_BiquadValue;
    f32 m_MainSend;
    f32 m_FxSend[AUX_BUS_NUM];

    f32 m_UserPitch;
    f32 m_SweepPitch;
    s32 m_SweepCounter;
    s32 m_SweepLength;

    f32 m_InitVolume;
    f32 m_InitPan;
    f32 m_InitSurroundPan;
    f32 m_Tune;
    MoveValue<u8, u16> m_SilenceVolume;

    f32 m_Cent;
    f32 m_CentPitch;
    
    s32 m_Length;

    PanMode m_PanMode;
    PanCurve m_PanCurve;

    u8 m_Key;
    u8 m_OriginalKey;
    u8 m_KeyGroupId;
    u8 m_InterpolationType;

    ChannelCallback m_Callback;
    u32 m_CallbackData;

    Voice* m_pVoice;

    Channel* m_pNextLink;

public:
    ut::LinkListNode m_Link;
    
    static Channel* AllocChannel(int voiceChannelCount, int priority, ChannelCallback callback, u32 callbackData);
    static void FreeChannel(Channel* channel);
public:
    Channel();
    ~Channel();

    void Update(bool doPeriodicProc);

    void Start(const WaveInfo& waveParam, int length, u32 startOffset);
    void NoteOff();
    void Release();
    void Stop();
    void Pause( bool flag ) { m_PauseFlag = flag; m_pVoice->Pause( flag ); }
    bool IsActive() const { return m_ActiveFlag != 0; }
    bool IsPause() const { return m_PauseFlag != 0; }

    /* Set Inines */
    void SetKey(u8 key) { m_Key = key; }
    void SetKey(u8 key, u8 originalKey) { m_Key = key; m_OriginalKey = originalKey; }
    void SetInitVolume(f32 volume) { m_InitVolume = volume; }
    void SetInitPan(f32 pan) { m_InitPan = pan; }
    void SetInitSurroundPan(f32 surroundPan) { m_InitSurroundPan = surroundPan; }
    void SetTune(f32 tune) { m_Tune = tune; }
    void SetAttack(int attack) { m_CurveAdshr.SetAttack(attack); }
    void SetHold(int hold) { m_CurveAdshr.SetHold(hold); }
    void SetDecay(int decay) { m_CurveAdshr.SetDecay(decay); }
    void SetSustain(int sustain) { m_CurveAdshr.SetSustain(sustain); }
    void SetRelease(int release) { m_CurveAdshr.SetRelease(release); }
    void SetSilence(bool silenceFlag, int fadeTimes) { m_SilenceVolume.SetTarget(silenceFlag ? SILENCE_VOLUME_MIN : SILENCE_VOLUME_MAX, static_cast<u16>(fadeTimes)); }

    s32 GetLength() const { return m_Length; }
    void SetLength( s32 length ) { m_Length = length; }
    bool IsRelease() const { return m_CurveAdshr.GetStatus() == CurveAdshr::STATUS_RELEASE; }

    void SetUserVolume(f32 volume) { m_UserVolume = volume; }
    void SetUserPitch(f32 pitch) { m_UserPitch = pitch; }
    void SetUserPitchRatio(f32 pitchRatio) { m_UserPitchRatio = pitchRatio; }
    void SetUserPan(f32 pan) { m_UserPan = pan; }
    void SetUserSurroundPan(f32 surroundPan) { m_UserSurroundPan = surroundPan; }
    void SetUserLpfFreq(f32 lpfFreq) { m_UserLpfFreq = lpfFreq; }
    void SetBiquadFilter(int type, f32 value);
    void SetLfoParam(const CurveLfoParam& param) { m_Lfo.SetParam(param); }
    void SetLfoTarget(LfoTarget type) { m_LfoTarget = type; }
    void SetPriority(int priority) { m_pVoice->SetPriority(priority); }
    void SetReleasePriorityFix(bool fix) { m_ReleasePriorityFixFlag = fix; }
    void SetIsIgnoreNoteOff(bool flag) { m_IsIgnoreNoteOff = flag; }
    void SetFrontBypass(bool flag) { m_pVoice->SetFrontBypass(flag); }

    void SetSweepParam(f32 sweepPitch, int sweepTime, bool autoUpdate);
    bool IsAutoUpdateSweep() const { return m_AutoSweep != 0 ; }
    void UpdateSweep(int count);

    void SetPanMode(PanMode panMode) { m_PanMode = panMode; }
    void SetPanCurve(PanCurve panCurve) { m_PanCurve = panCurve; }

    void SetMainSend(f32 send) { m_MainSend = send; }
    void SetFxSend(AuxBus bus, f32 send) { m_FxSend[ bus ] = send; }

    Channel* GetNextTrackChannel() const { return m_pNextLink; }
    void SetNextTrackChannel(Channel* channel) { m_pNextLink = channel; }

    u32 GetCurrentPlayingSample() const;

    void SetKeyGroupId(u8 id) { m_KeyGroupId = id; }
    u8 GetKeyGroupId() const { return m_KeyGroupId; }

    void SetInterpolationType(u8 type) { m_InterpolationType = type; }
    u8 GetInterpolationType() const { return m_InterpolationType; }
};

} // namespace nw
} // namespace snd
} // namespace internal
} // namespace driver

#endif // NW_SND_CHANNEL_H_
