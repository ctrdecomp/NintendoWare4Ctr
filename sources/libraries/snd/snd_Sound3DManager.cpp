// Filename: snd_Sound3DManager.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Sound3DManager.h>

#include <nw/snd/snd_Sound3DEngine.h>
#include <nw/snd/snd_SoundArchive.h>

namespace nw {
namespace snd {

namespace
{
Sound3DEngine sSound3DEngine;
}

Sound3DManager::Sound3DManager(): 
    m_pSound3DEngine(&sSound3DEngine),
    m_MaxPriorityReduction(32),
    m_PanRange(0.9f),
    m_SonicVelocity(0.0f),
    m_BiquadFilterType(0),
    m_pSoundParamPoolBuffer(NULL),
    m_SoundParamPoolBufferSize(0),
    m_FreeMemSizeAfterCheking(0),
    m_IsInitialized(false)
{
}

size_t Sound3DManager::GetRequiredMemSize(const SoundArchive* arc)
{
    NW_NULL_ASSERT(arc);

    s32 numSounds = 0;

    SoundArchive::SoundArchivePlayerInfo soundArchivePlayerInfo;
    if (arc->ReadSoundArchivePlayerInfo(&soundArchivePlayerInfo))
    {
        numSounds += soundArchivePlayerInfo.sequenceSoundMax;
        numSounds += soundArchivePlayerInfo.streamSoundMax;
        numSounds += soundArchivePlayerInfo.waveSoundMax;
    }

    return static_cast<size_t>(numSounds) * sizeof(Sound3DParam);
}

bool Sound3DManager::Initialize(const SoundArchive* arc, void* buffer, size_t size)
{
    if (m_IsInitialized == true)
    {
        return false;
    }
    NW_UNUSED_VARIABLE(arc);

    NW_NULL_ASSERT(arc);
    NW_NULL_ASSERT(buffer);
    size_t memSize = GetRequiredMemSize(arc);
    NW_ASSERT(size >= memSize);

    m_ParamPool.Create(buffer, size);
    m_pSoundParamPoolBuffer = buffer;
    m_SoundParamPoolBufferSize = size;
    m_FreeMemSizeAfterCheking = size - memSize;
    m_IsInitialized = true;

    return true;
}

bool Sound3DManager::Finalize()
{
    if (m_IsInitialized == false)
    {
        return false;
    }

    m_pSound3DEngine = &sSound3DEngine;
    m_MaxPriorityReduction = 32;
    m_PanRange = 0.9f;
    m_SonicVelocity = 0.0f;
    m_BiquadFilterType = 0;

    while (!m_ListenerList.IsEmpty())
    {
        Sound3DListener& listener = m_ListenerList.GetBack();
        m_ListenerList.Erase(&listener);
    }

    m_ParamPool.Destroy(m_pSoundParamPoolBuffer, m_SoundParamPoolBufferSize);
    m_pSoundParamPoolBuffer = NULL;
    m_SoundParamPoolBufferSize = 0;
    m_FreeMemSizeAfterCheking = 0;

    m_IsInitialized = false;
    return true;
}

void Sound3DManager::SetEngine(Sound3DEngine* engine)
{
    m_pSound3DEngine = engine;
}

void Sound3DManager::detail_UpdateAmbientParam(const void* arg, u32 soundId,
    SoundAmbientParam* param)
{
    const Sound3DParam* actorParam = static_cast<const Sound3DParam*>(arg);

    if (m_pSound3DEngine != NULL)
    {
        m_pSound3DEngine->UpdateAmbientParam(this, actorParam, soundId, param);
    }
}

int Sound3DManager::detail_GetAmbientPriority(const void* arg, u32 soundId)
{
    const Sound3DParam* actorParam = static_cast<const Sound3DParam*>(arg);

    int priority = 0;

    if (m_pSound3DEngine != NULL)
    {
        priority = m_pSound3DEngine->GetAmbientPriority(this, actorParam, soundId);
    }

    return priority;
}

void* Sound3DManager::detail_AllocAmbientArg(size_t argSize)
{
    if (argSize != sizeof(Sound3DParam)) 
    {
        return NULL;
    }
    return m_ParamPool.Alloc();
}

void Sound3DManager::detail_FreeAmbientArg(void* arg, const internal::BasicSound* sound)
{
    NN_UNUSED_VAR(sound);

    Sound3DParam* actorParam = static_cast<Sound3DParam*>(arg);
    m_ParamPool.Free(actorParam);
}

void Sound3DManager::SetBiquadFilterType(int type)
{
    NW_MINMAX_ASSERT(type, 0, BIQUAD_FILTER_TYPE_USER_MAX);
    m_BiquadFilterType = type;
}

/* Sound3DParam */

Sound3DParam::Sound3DParam():
    ctrlFlag(0),
    actorUserParam(0),
    soundUserParam(0),
    decayRatio(0.5f),
    decayCurve(SoundArchive::Sound3DInfo::DECAY_CURVE_LOG),
    dopplerFactor(0)
{
}

} // namespace nw::snd
} // namespace nw

