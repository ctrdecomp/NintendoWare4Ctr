#ifndef NW_SND_SEQUENCE_SOUND_HANDLE_H_
#define NW_SND_SEQUENCE_SOUND_HANDLE_H_

#include <nw/snd/snd_SequenceSound.h>

namespace nw {
namespace snd {

class SequenceSoundHandle
{
public:
    static const int BANK_INDEX_MIN = internal::SequenceSound::BANK_INDEX_MIN;
    static const int BANK_INDEX_MAX = internal::SequenceSound::BANK_INDEX_MAX;
    static const s8 TRANSPOSE_MIN = internal::SequenceSound::TRANSPOSE_MIN;
    static const s8 TRANSPOSE_MAX = internal::SequenceSound::TRANSPOSE_MAX;
    static const u8 VELOCITY_RANGE_MIN = internal::SequenceSound::VELOCITY_RANGE_MIN;
    static const u8 VELOCITY_RANGE_MAX = internal::SequenceSound::VELOCITY_RANGE_MAX;

    SequenceSoundHandle() : m_pSound(NULL) { }
    explicit SequenceSoundHandle(SoundHandle* handle);
    ~SequenceSoundHandle() { DetachSound(); }

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

    void FadeIn(int frames)
    {
        if (IsAttachedSound())
        {
            m_pSound->FadeIn(frames);
        }
    }

    void SetVolume(f32 volume, int frames = 0)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetVolume(volume, frames);
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

    void SetPlayerPriority(int priority)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetPlayerPriority(priority);
        }
    }

    void SetChannelPriority(int priority)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetChannelPriority(priority);
        }
    }

    void SetTempoRatio(f32 tempoRatio)
    {
        if (IsAttachedSound())
        { 
            m_pSound->SetTempoRatio(tempoRatio);
        }
    }

    void SetPan(f32 pan)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetPan(pan);
        }
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

    bool ReadVariable(int varNo, signed short* var) const
    {
        return IsAttachedSound() && m_pSound->ReadVariable(varNo, var);
    }

    static bool ReadGlobalVariable(int varNo, signed short* var)
    {
        return internal::SequenceSound::ReadGlobalVariable(varNo, var);
    }

    bool ReadTrackVariable(int trackNo, int varNo, signed short* var) const
    {
        return IsAttachedSound() && m_pSound->ReadTrackVariable(trackNo, varNo, var);
    }

    bool WriteVariable(int varNo, signed short var)
    {
        if (!IsAttachedSound()) return false;
        m_pSound->WriteVariable(varNo, var);
        return true;
    }

    static bool WriteGlobalVariable(int varNo, signed short var)
    {
        internal::SequenceSound::WriteGlobalVariable(varNo, var);
        return true;
    }

    bool WriteTrackVariable(int trackNo, int varNo, signed short var)
    {
        if (!IsAttachedSound()) return false;
        m_pSound->WriteTrackVariable(trackNo, varNo, var);
        return true;
    }

    void SetTrackMute(u32 trackBitFlag, SeqMute mute)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackMute(trackBitFlag, mute);
        }
    }

    void SetTrackMute(u32 trackBitFlag, bool muteFlag)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackMute(trackBitFlag, muteFlag);
        }
    }

    void SetTrackSilence(u32 trackBitFlag, bool silenceFlag, int fadeTimes)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackSilence(trackBitFlag, silenceFlag, fadeTimes);
        }
    }

    void SetTrackVolume(u32 trackBitFlag, f32 volume)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackVolume(trackBitFlag, volume);
        }
    }

    void SetTrackPitch(u32 trackBitFlag, f32 pitch)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackPitch(trackBitFlag, pitch);
        }
    }

    bool SetTrackBankIndex(u32 trackBitFlag, int bankIndex)
    {
        if (bankIndex < BANK_INDEX_MIN || bankIndex > BANK_INDEX_MAX)
        {
            return false;
        }

        if (IsAttachedSound())
        {
            m_pSound->SetTrackBankIndex(trackBitFlag, bankIndex);
        }
        return true;
    }

    bool SetTrackTranspose(u32 trackBitFlag, s8 transpose)
    {
        if (transpose < TRANSPOSE_MIN || transpose > TRANSPOSE_MAX)
        {
            return false;
        }

        if (IsAttachedSound())
        {
            m_pSound->SetTrackTranspose(trackBitFlag, transpose);
        }
        return true;
    }

    bool SetTrackVelocityRange(u32 trackBitFlag, u8 range)
    {
        if (range > VELOCITY_RANGE_MAX)
        {
            return false;
        }

        if (IsAttachedSound())
        {
            m_pSound->SetTrackVelocityRange(trackBitFlag, range);
        }
        return true;
    }

    void SetTrackOutputLine(u32 trackBitFlag, u32 lineFlag)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackOutputLine(trackBitFlag, lineFlag);
        }
    }

    void ResetTrackOutputLine(u32 trackBitFlag)
    {
        if (IsAttachedSound())
        {
            m_pSound->ResetTrackOutputLine(trackBitFlag);
        }
    }

    void SetTrackMainOutVolume(u32 trackBitFlag, f32 volume)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackMainOutVolume(trackBitFlag, volume);
        }
    }

    void SetTrackPan(u32 trackBitFlag, f32 pan)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackPan(trackBitFlag, pan);
        }
    }

    void SetTrackSurroundPan(u32 trackBitFlag, f32 surroundPan)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackSurroundPan(trackBitFlag, surroundPan);
        }
    }

    void SetTrackMainSend(u32 trackBitFlag, f32 send)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackMainSend(trackBitFlag, send);
        }
    }

    void SetTrackFxSend(u32 trackBitFlag, AuxBus bus, f32 send)
    {
        if (IsAttachedSound())
        {
            m_pSound->SetTrackFxSend(trackBitFlag, bus, send);
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

    unsigned long GetTick() const { return IsAttachedSound() ? m_pSound->GetTick() : 0; }

    void detail_AttachSoundAsTempHandle(internal::SequenceSound* sound);

    internal::SequenceSound* detail_GetAttachedSound() { return m_pSound; }

    const internal::SequenceSound* detail_GetAttachedSound() const { return m_pSound; }

private:
    NW_DISALLOW_COPY_AND_ASSIGN(SequenceSoundHandle);

    internal::SequenceSound* m_pSound;
};

} // namespace snd
} // namespace nw

#endif // NW_SND_SEQUENCE_SOUND_HANDLE_H_
