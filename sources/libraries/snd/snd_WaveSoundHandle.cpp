// Filename: snd_WaveSoundHandle.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_WaveSoundHandle.h>

namespace nw {
namespace snd {

WaveSoundHandle::WaveSoundHandle(SoundHandle* pHandle): 
    m_pSound(NULL)
{
    if (pHandle == NULL)
    {
        return;
    }

    if (!pHandle->IsAttachedSound())
    {
        return;
    }

    internal::WaveSound* sound = ut::DynamicCast<internal::WaveSound*>(pHandle->detail_GetAttachedSound());

    if (sound != NULL)
    {
        detail_AttachSoundAsTempHandle(sound);
    }
}

void WaveSoundHandle::detail_AttachSoundAsTempHandle(internal::WaveSound* pSound)
{
    m_pSound = pSound;

    if (m_pSound->IsAttachedTempSpecialHandle())
    {
        m_pSound->DetachTempSpecialHandle();
    }
    m_pSound->m_pTempSpecialHandle = this;
}

void WaveSoundHandle::DetachSound()
{
    if (IsAttachedSound())
    {
        if (m_pSound->m_pTempSpecialHandle == this)
        {
            m_pSound->m_pTempSpecialHandle = NULL;
        }
    }
    if (m_pSound != NULL)
    {
        m_pSound = NULL;
    }
}

} // namespace snd
} // namespace nw