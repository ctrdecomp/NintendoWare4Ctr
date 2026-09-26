// Filename: snd_SequenceSoundFileHandle.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SequenceSoundHandle.h>

#include <nw/snd/snd_SoundHandle.h>

namespace nw {
namespace snd {

SequenceSoundHandle::SequenceSoundHandle(SoundHandle* handle): 
    m_pSound(NULL)
{
    if (handle == NULL) 
    {
        return;
    }

    if (!handle->IsAttachedSound()) 
    {
        return;
    }

    internal::SequenceSound* sound =
        ut::DynamicCast<internal::SequenceSound*>(handle->detail_GetAttachedSound());

    if (sound != NULL)
    {
        detail_AttachSoundAsTempHandle(sound);
    }
}

void SequenceSoundHandle::detail_AttachSoundAsTempHandle(internal::SequenceSound* sound)
{
    NW_NULL_ASSERT(sound);

    m_pSound = sound;

    if (m_pSound->IsAttachedTempSpecialHandle())
    {
        m_pSound->DetachTempSpecialHandle();
    }
    m_pSound->m_pTempSpecialHandle = this;
}

void SequenceSoundHandle::DetachSound()
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

