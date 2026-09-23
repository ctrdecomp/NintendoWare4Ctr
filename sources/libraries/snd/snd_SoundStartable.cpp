// Filename: snd_SoundStartable.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundStartable.h>
#include <nw/snd/snd_SoundHandle.h>

namespace nw {
namespace snd {

SoundStartable::StartResult SoundStartable::StartSound(SoundHandle* pHandle, SoundArchive::ItemId soundId, const StartInfo* pStartInfo)
{
    StartResult res = detail_SetupSound(pHandle, soundId, false, pStartInfo);
    if (!res.IsSuccess())
    {
        return res;
    }
    pHandle->IsPrepared();

    return StartResult::START_SUCCESS;
}

SoundStartable::StartResult SoundStartable::StartSound(SoundHandle* pHandle, const char* soundName, const StartInfo* pStartInfo)
{
    SoundArchive::ItemId id = detail_GetItemId(soundName);
    if (id == -1) // invalid id
    {
        return StartResult::START_ERR_INVALID_LABEL_STRING;
    }

    StartSound(pHandle, id, pStartInfo);
}

SoundStartable::StartResult SoundStartable::PrepareSound(SoundHandle* pHandle, const char* pSoundName, const StartInfo* pStartInfo)
{
    SoundArchive::ItemId id = detail_GetItemId(pSoundName);
    if (id == -1) // invalid id
    {
        return StartResult::START_ERR_INVALID_LABEL_STRING;
    }
    else
    {
        detail_SetupSound(pHandle, id, false, pStartInfo);
    }
}
} // namespace snd
} // namespace nw