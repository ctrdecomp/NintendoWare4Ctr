#ifndef NW_SND_BASIC_SOUND_H_
#define NW_SND_BASIC_SOUND_H_

#include <nn/types.h>
#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_MoveValue.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/ut/ut_LinkList.h>

namespace nw { 
namespace snd {

class SoundHandle;
class SoundPlayer;
class SoundActor;

struct SoundParam
{
    f32 volume;
    f32 pitch;
    f32 pan;
    f32 span;
    f32 fxSend;
    f32 lpf;
    f32 biquadFilterValue;
    int biquadFilterType;
    int priority;
    u32 userData;

    SoundParam(): 
        volume(1.0f),
        pitch(1.0f),
        pan(0.0f),
        span(0.0f),
        fxSend(0.0f),
        lpf(0.0f),
        biquadFilterValue(0.0f),
        biquadFilterType(0),
        priority(0),
        userData(0)
    {
    }
};

struct SoundAmbientParam
{
    f32 volume;
    f32 pitch;
    f32 pan;
    f32 span;
    f32 fxSend;
    f32 lpf;
    f32 biquadFilterValue;
    int biquadFilterType;
    int priority;
    u32 userData;

    SoundAmbientParam(): 
        volume(1.0f),
        pitch(1.0f),
        pan(0.0f),
        span(0.0f),
        fxSend(0.0f),
        lpf(0.0f),
        biquadFilterValue(0.0f),
        biquadFilterType(0),
        priority(0),
        userData(0)
    {
    }
};

namespace internal {

struct SoundActorParam
{
    f32 volume;
    f32 pitch;
    f32 pan;

    SoundActorParam(): 
        volume(1.0f),
        pitch(1.0f),
        pan(0.0f) 
    {
    }

    void Reset()
    {
        volume = pitch = 1.0f;
        pan = 0.0f;
    }
};

namespace driver {

class BasicSoundPlayer;

} // namespace internal::driver

class PlayerHeap;
class ExternalSoundPlayer;

class BasicSound
{
    friend class nw::snd::SoundHandle;

public:
    NW_UT_RUNTIME_TYPEINFO;

public:
    static const int PRIORITY_MIN = 0;
    static const int PRIORITY_MAX = 127;
    static const u32 INVALID_ID = 0xffffffff;

public:
    class AmbientParamUpdateCallback;
    class AmbientArgUpdateCallback;
    class AmbientArgAllocatorCallback;

    struct AmbientInfo
    {
        AmbientParamUpdateCallback* paramUpdateCallback;
        AmbientArgUpdateCallback* argUpdateCallback;
        AmbientArgAllocatorCallback* argAllocatorCallback;
        void* arg;
        unsigned long argSize;
    };

    enum PlayerState
    {
        PLAYER_STATE_INIT,
        PLAYER_STATE_PLAY,
        PLAYER_STATE_STOP
    };

public:
    BasicSound();
    virtual ~BasicSound() {}
    void Update();
    void StartPrepared();
    void Stop(int fadeFrames);
    void Pause(bool flag, int fadeFrames);
    void SetAutoStopCounter(int frames);
    void FadeIn(int frames);

    virtual void Initialize();
    virtual void Finalize();
    virtual bool IsPrepared() const = 0;

    bool IsPause() const;
    bool IsStarted() const { return m_StartedFlag; }

    void SetPriority(int priority, int ambientPriority);
    void GetPriority(int* priority, int* ambientPriority) const;

    void SetInitialVolume(f32 volume);
    f32  GetInitialVolume() const;

    void SetVolume(f32 volume, int frames = 0);
    f32  GetVolume() const;

    void SetPitch(f32 pitch);
    f32  GetPitch() const;

    void SetLpfFreq(f32 lpfFreq);
    f32  GetLpfFreq() const

    void SetOutputLine(u32 lineFlag);
    u32 GetOutputLine() const;
    void ResetOutputLine();

    void SetPlayerPriority(int priority);

    void SetPan(f32 pan);
    f32  GetPan() const;

    void SetFxSend(AuxBus bus, f32 send);
    f32 GetFxSend(AuxBus bus) const;

    void SetPanMode(PanMode mode);

    void SetPanCurve(PanCurve curve);

    void SetSurroundPan(f32 pan);
    f32 GetSurroundPan() const;

    void SetMainSend(f32 send);
    f32  GetMainSend() const;

    void SetFrontBypass(bool isFrontBypass);

    void SetRemoteFilter(u8 filter);

    void SetBiquadFilter(int type, f32 value);
    void GetBiquadFilter(int* type, f32* value) const;

    void SetRemoteOutVolume(u32 remoteIndex, f32 volume);
    f32  GetRemoteOutVolume(u32 remoteIndex) const;

    int GetRemainingFadeFrames() const;
    int GetRemainingPauseFadeFrames() const;

    int GetPlayerPriority() const { return m_Priority; }

    void SetId(u32 id);
    u32 GetId() const { return m_Id; }

    int CalcCurrentPlayerPriority() const
    {
        return ut::Clamp(
             static_cast<int>(m_Priority) + static_cast<int>(m_AmbientParam.priority),
             PRIORITY_MIN,
             PRIORITY_MAX
        );
    }

    SoundPlayer* GetSoundPlayer() { return m_pSoundPlayer; }
    const SoundPlayer* GetSoundPlayer() const { return m_pSoundPlayer; }

    void AttachSoundPlayer(SoundPlayer* player);
    void DetachSoundPlayer(SoundPlayer* player);

    void AttachSoundActor(SoundActor* actor);
    void DetachSoundActor(SoundActor* actor);

    void AttachExternalSoundPlayer(ExternalSoundPlayer* extPlayer);
    void DetachExternalSoundPlayer(ExternalSoundPlayer* extPlayer);

    void AttachPlayerHeap(PlayerHeap* pHeap);
    void DetachPlayerHeap(PlayerHeap* pHeap);
    PlayerHeap* GetPlayerHeap() { return m_pPlayerHeap; }

    void SetAmbientInfo(const AmbientInfo& info);
    void ClearAmbientArgUpdateCallback() { m_AmbientInfo.argUpdateCallback = NULL; }
    void ClearAmbientParamUpdateCallback() { m_AmbientInfo.paramUpdateCallback = NULL; }
    void ClearAmbientArgAllocatorCallback() { m_AmbientInfo.argAllocatorCallback = NULL; }
    const SoundParam& GetAmbientParam() const { return m_AmbientParam; }
    static int GetAmbientPriority(const AmbientInfo& ambientInfo, u32 soundId);

    bool IsAttachedGeneralHandle();
    bool IsAttachedTempGeneralHandle();
    virtual bool IsAttachedTempSpecialHandle() = 0;
    void DetachGeneralHandle();
    void DetachTempGeneralHandle();
    virtual void DetachTempSpecialHandle() = 0;

    enum PauseState
    {
        PAUSE_STATE_NORMAL,
        PAUSE_STATE_PAUSING,
        PAUSE_STATE_PAUSED,
        PAUSE_STATE_UNPAUSING
    };
    PauseState GetPauseState() const { return m_PauseState; }

protected:
    virtual driver::BasicSoundPlayer* GetBasicSoundPlayerHandle() = 0;

    virtual void OnUpdatePlayerPriority() {}

    virtual void UpdateMoveValue();
    virtual void UpdateParam();

    bool IsPlayerAvailable() const { return m_PlayerAvailableFlag; }

private:
    void ClearIsFinalizedForCannotAllocatedResourceFlag();

    PlayerHeap* m_pPlayerHeap;
    SoundHandle* m_pGeneralHandle;
    SoundHandle* m_pTempGeneralHandle;
    SoundPlayer* m_pSoundPlayer;
    SoundActor* m_pSoundActor;
    ExternalSoundPlayer* m_pExtSoundPlayer;

    AmbientInfo m_AmbientInfo;
    SoundParam m_AmbientParam;
    SoundActorParam m_ActorParam;

    MoveValue<f32, int> m_FadeVolume;
    MoveValue<f32, int> m_PauseFadeVolume;

    bool m_InitializeFlag;
    bool m_StartFlag;
    bool m_StartedFlag;
    bool m_AutoStopFlag;
    bool m_FadeOutFlag;
    bool m_PlayerAvailableFlag;

    PlayerState m_PlayerState;
    PauseState m_PauseState;
    bool m_UnPauseFlag;

    s32 m_AutoStopCounter;
    u32 m_UpdateCounter;

    u8 m_Priority;
    s8 m_BiquadFilterType;

    u32 m_Id;

    MoveValue<f32, int> m_ExtMoveVolume;
    f32 m_InitVolume;
    f32 m_ExtPan;
    f32 m_ExtPitch;
    f32 m_LpfFreq;
    f32 m_BiquadFilterValue;
    f32 m_MainSend;
    f32 m_FxSend[AUX_BUS_NUM];
    f32 m_ExtSurroundPan;

public:
    ut::LinkListNode m_PriorityLink;
    ut::LinkListNode m_SoundPlayerPlayLink;
    ut::LinkListNode m_SoundPlayerPriorityLink;
    ut::LinkListNode m_ExtSoundPlayerPlayLink;
};

class BasicSound::AmbientParamUpdateCallback
{
public:
    virtual ~AmbientParamUpdateCallback() {}
    virtual void detail_UpdateAmbientParam(const void* arg, u32 soundId, SoundAmbientParam* param) = 0;
    virtual int detail_GetAmbientPriority(const void* arg, u32 soundId) = 0;
};

class BasicSound::AmbientArgUpdateCallback
{
public:
    virtual ~AmbientArgUpdateCallback() {}
    virtual void detail_UpdateAmbientArg(void* arg, const internal::BasicSound* sound) = 0;
};

class BasicSound::AmbientArgAllocatorCallback
{
public:
    virtual ~AmbientArgAllocatorCallback() {}
    virtual void* detail_AllocAmbientArg(size_t argSize ) = 0;
    virtual void detail_FreeAmbientArg(void* arg, const internal::BasicSound* sound) = 0;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_BASIC_SOUND_H_
