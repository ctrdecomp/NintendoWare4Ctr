// Filename: snd_SoundHandle.cpp
//
// Project: NintendoWare4Ctr


#include <nw/snd/snd_SoundHandle.h>

namespace nw {
namespace snd {

void SoundHandle::detail_DuplicateHandle(SoundHandle* handle)
{
    DetachSound();

    if (handle == NULL)
    {
        return;
    }

    if (handle->IsAttachedSound() == false)
    {
        return;
    }

    internal::BasicSound* sound = handle->detail_GetAttachedSound();

    if (sound != NULL)
    {
        detail_AttachSoundAsTempHandle(sound);
    }
}

void SoundHandle::detail_AttachSoundAsTempHandle(internal::BasicSound* sound)
{
    NW_NULL_ASSERT(sound);

    m_pSound = sound;

    if (m_pSound->IsAttachedTempGeneralHandle())
    {
        m_pSound->DetachTempGeneralHandle();
    }
    m_pSound->m_pTempGeneralHandle = this;
}

void SoundHandle::detail_AttachSound(internal::BasicSound* sound)
{
    NW_NULL_ASSERT(sound);

    m_pSound = sound;

    if (m_pSound->IsAttachedGeneralHandle())
    {
        m_pSound->DetachGeneralHandle();
    }
    m_pSound->m_pGeneralHandle = this;
}

void SoundHandle::DetachSound()
{
    if (IsAttachedSound())
    {
        if (m_pSound->m_pGeneralHandle == this)
        {
            m_pSound->m_pGeneralHandle = NULL;
        }
        if (m_pSound->m_pTempGeneralHandle == this)
        {
            m_pSound->m_pTempGeneralHandle = NULL;
        }
    }
    if (m_pSound != NULL)
    {
        m_pSound = NULL;
    }
}

} // namespace snd
} // namespace nw

