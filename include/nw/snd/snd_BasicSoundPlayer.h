#ifndef NW_SND_BASIC_SOUND_PLAYER_H_
#define NW_SND_BASIC_SOUND_PLAYER_H_

#include <nn/types.h>
#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_PlayerHeapDataManager.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

struct PlayerParamSet
{
    f32 volume;
    f32 pitch;
    f32 pan;
    f32 surroundPan;
    f32 lpfFreq;
    f32 biquadValue;
    u8 biquadType;
    s8 __PADDING__;
    bool isFrontBypass;
    f32 mainSend;
    PanMode panMode;
    PanCurve panCurve;
    f32 fxSend[AUX_BUS_NUM];

    PlayerParamSet() { Initialize(); }
    void Initialize();
};

class BasicSoundPlayer
{
public:
    BasicSoundPlayer();
    virtual ~BasicSoundPlayer() {};

    virtual void Initialize();
    virtual void Finalize();

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Pause(bool flag) = 0;

    bool IsActive() const { return m_ActiveFlag; }
    bool IsStarted() const { return m_StartedFlag; }
    bool IsPause() const { return m_PauseFlag; }
    bool IsPlayFinished() const { return m_FinishFlag; }
    bool IsFinalizedForCannotAllocateResource() const
    {
        return m_IsFinalizedForCannotAllocateResource;
    }

    void SetVolume(f32 volume) { m_PlayerParamSet.volume = volume; }
    void SetPitch(f32 pitch) { m_PlayerParamSet.pitch = pitch; }
    void SetLpfFreq(f32 lpfFreq) { m_PlayerParamSet.lpfFreq = lpfFreq; }
    void SetBiquadFilter(int type, f32 value);
    void SetPanMode(PanMode mode) { m_PlayerParamSet.panMode = mode; }
    void SetPanCurve(PanCurve curve)  { m_PlayerParamSet.panCurve = curve; }
    void SetFrontBypass(bool frontBypass) { m_PlayerParamSet.isFrontBypass = frontBypass; }

    float GetVolume() const { return m_PlayerParamSet.volume; }
    float GetPitch() const { return m_PlayerParamSet.pitch; }
    float GetPan() const { return m_PlayerParamSet.pan; }
    float GetLpfFreq() const { return m_PlayerParamSet.lpfFreq; }
    int GetBiquadFilterType() const { return m_PlayerParamSet.biquadType; }
    float GetBiquadFilterValue() const { return m_PlayerParamSet.biquadValue; }
    PanMode GetPanMode() const { return m_PlayerParamSet.panMode; }
    PanCurve GetPanCurve() const { return m_PlayerParamSet.panCurve; }
    float GetSurroundPan() const { return m_PlayerParamSet.surroundPan; }
    bool IsFrontBypass() const { return m_PlayerParamSet.isFrontBypass; }

    bool TryWaitInstanceFree()
    {
        return m_Event.TryWait();
    }

    void WaitInstanceFree()
    {
        m_Event.Wait();
    }

    const PlayerHeapDataManager* GetPlayerHeapDataManager() const
    {
        return m_pPlayerHeapDataManager;
    }

    void ClearIsFinalizedForCannotAllocatedResourceFlag()
    {
        m_IsFinalizedForCannotAllocateResource = false;
    }

protected:
    void SetPlayerHeapDataManagerImpl(const PlayerHeapDataManager* mgr)
    {
        m_pPlayerHeapDataManager = mgr;
    }

    nn::os::LightEvent m_Event;
    bool m_ActiveFlag;
    bool m_StartedFlag;
    bool m_PauseFlag;
    bool m_FinishFlag;

    bool m_IsFinalizedForCannotAllocateResource;

private:
    PlayerParamSet m_PlayerParamSet;
    const PlayerHeapDataManager* m_pPlayerHeapDataManager;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_BASIC_SOUND_PLAYER_H_
