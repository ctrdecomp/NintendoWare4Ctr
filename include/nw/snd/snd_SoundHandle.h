#ifndef NW_SND_SOUND_HANDLE_H_
#define NW_SND_SOUND_HANDLE_H_

#include <nw/snd/snd_BasicSound.h>
#include <nw/snd/snd_Global.h>

namespace nw { 
namespace snd {

class SoundHandle
{
public:
    enum PauseState
    {
        PAUSE_STATE_NORMAL,
        PAUSE_STATE_PAUSING,
        PAUSE_STATE_PAUSED,
        PAUSE_STATE_UNPAUSING,
        PAUSE_STATE_INVALID
    };

    SoundHandle()
        : m_pSound(NULL)
    {
    }

    ~SoundHandle()
    {
        DetachSound();
    }

    void StartPrepared()
    {
        if (IsAttachedSound())
        {
            m_pSound->StartPrepared();
        }
    }

    void Stop(int fadeFrames)
    {
        if (IsAttachedSound())
        {
            m_pSound->Stop(fadeFrames);
        }
    }

    void Pause(bool flag, int fadeFrames)
    {
        if (IsAttachedSound())
        {
            m_pSound->Pause(flag, fadeFrames);
        }
    }

    bool IsPrepared() const { return IsAttachedSound() && m_pSound->IsPrepared(); }
    bool IsPause() const { return IsAttachedSound() && m_pSound->IsPause(); }

    PauseState GetPauseState() const
    {
        if (!IsAttachedSound())
            return PAUSE_STATE_INVALID;

        internal::BasicSound::PauseState state = m_pSound->GetPauseState();
        return static_cast<PauseState>(state);
    }

    void FadeIn(int frames)
    {
        if (IsAttachedSound())
        {
            m_pSound->FadeIn(frames);
        }
    }

    int GetRemainingFadeFrames() const
    {
        if (IsAttachedSound())
        {
            return m_pSound->GetRemainingFadeFrames();
        }
        else
        {
            return 0;
        }
    }

    int GetRemainingPauseFadeFrames() const
    {
        if (IsAttachedSound())
        {
            return m_pSound->GetRemainingPauseFadeFrames();
        }
        else
        {
            return 0;
        }
    }

    void SetVolume(f32 volume, int frames = 0)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetVolume(volume, frames);
        }
    }

    void SetPan(f32 pan)
    {
        if (IsAttachedSound())
            m_pSound->SetPan(pan);
    }

    void SetSurroundPan(f32 surroundPan)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetSurroundPan(surroundPan);
        }
    }

    void SetMainSend(f32 send)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetMainSend(send);
        }
    }

    void SetFxSend(AuxBus bus, f32 send)
    {
        if (IsAttachedSound()) 
        {
            m_pSound->SetFxSend(bus, send);
        }
    }

    void SetPitch(f32 pitch)
    {
        if (IsAttachedSound())
        { 
            m_pSound->SetPitch(pitch);
        }
    }

    void SetLpfFreq(f32 lpfFreq)
    {
        if (IsAttachedSound())
        { 
            m_pSound->SetLpfFreq(lpfFreq);
        }
    }

    void SetBiquadFilter(int type, f32 value)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetBiquadFilter(type, value);
        }
    }

    void SetPlayerPriority(int priority)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetPlayerPriority(priority);
        }
    }

    void SetOutputLine(u32 lineFlag)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetOutputLine(lineFlag);
        }
    }

    void ResetOutputLine()
    {
        if (IsAttachedSound())
        {
            m_pSound->ResetOutputLine();
        }
    }

    void SetRemoteOutVolume(u32 remoteIndex, f32 volume)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetRemoteOutVolume(remoteIndex, volume);
        }
    }

    bool IsAttachedSound() const { return m_pSound != NULL; }
    void DetachSound();

    void SetId(u32 id)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetId(id);
        }
    }

    u32 GetId() const
    {
        if (IsAttachedSound())
        {
            return m_pSound->GetId();
        }

        return internal::BasicSound::INVALID_ID;
    }

    const SoundParam* GetAmbientParam() const
    {
        if (!IsAttachedSound())
        {
            return NULL;
        }

        return &m_pSound->GetAmbientParam();
    }

    void detail_AttachSound(internal::BasicSound* sound);
    void detail_AttachSoundAsTempHandle(internal::BasicSound* sound);

    internal::BasicSound* detail_GetAttachedSound()
    {
        return m_pSound;
    }

    const internal::BasicSound* detail_GetAttachedSound() const
    {
        return m_pSound;
    }

private:
    NW_DISALLOW_COPY_AND_ASSIGN(SoundHandle);

    internal::BasicSound* m_pSound;
};

} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_HANDLE_H_
