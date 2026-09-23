// Filename: snd_StreamSoundHandle.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_StreamSoundHandle.h>

namespace nw {
namespace snd {

StreamSoundHandle::StreamSoundHandle(SoundHandle* pHandle): 
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

    internal::StreamSound* sound = ut::DynamicCast<internal::StreamSound*>(pHandle->detail_GetAttachedSound());

    if (sound != NULL)
    {
        detail_AttachSoundAsTempHandle(sound);
    }
}

void StreamSoundHandle::detail_AttachSoundAsTempHandle(internal::StreamSound* pSound)
{
    m_pSound = pSound;

    if (m_pSound->IsAttachedTempSpecialHandle())
    {
        m_pSound->DetachTempSpecialHandle();
    }
    m_pSound->m_pTempSpecialHandle = this;
}

void StreamSoundHandle::DetachSound()
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