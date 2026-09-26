// Filename: snd_Sound3DActor.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Sound3DActor.h>

#include <nw/snd/snd_SoundArchivePlayer.h>
#include <nw/snd/snd_Sound3DManager.h>
#include <nw/snd/snd_SoundHandle.h>
#include <nw/snd/snd_Global.h>

namespace nw {
namespace snd {

Sound3DActor::Sound3DActor(SoundArchivePlayer& player, Sound3DManager& manager): 
    m_p3dManager(NULL),
    m_pArchivePlayer(NULL),
    m_UserParam(0),
    m_Position(0.0f, 0.0f, 0.0f),
    m_Velocity(0.0f, 0.0f, 0.0f),
    m_ResetPositionFlag(true),
    m_IsInitialized(false),
    m_IsFinalized(true)
{
    Initialize(player, manager);
}

Sound3DActor::Sound3DActor(): 
    m_p3dManager(NULL),
    m_pArchivePlayer(NULL),
    m_UserParam(0),
    m_Position(0.0f, 0.0f, 0.0f),
    m_Velocity(0.0f, 0.0f, 0.0f),
    m_ResetPositionFlag(true),
    m_IsInitialized(false),
    m_IsFinalized(true)
{
}

Sound3DActor::~Sound3DActor()
{
    Finalize();
}

void Sound3DActor::Initialize(SoundArchivePlayer& player, Sound3DManager& manager)
{
    if (m_IsInitialized) 
    {
        return;
    }

    SoundActor::Initialize(player);
    m_p3dManager = &manager;
    m_pArchivePlayer = &player;

    m_IsInitialized = true;
    m_IsFinalized = false;
}

void Sound3DActor::Finalize()
{
    if (m_IsFinalized) 
    {
        return;
    }

    SoundActor::ForEachSound(ClearUpdateCallback);
    SoundActor::Finalize();

    m_IsFinalized = true;
    m_IsInitialized = false;
    m_p3dManager = NULL;
    m_pArchivePlayer = NULL;
}

SoundStartable::StartResult Sound3DActor::SetupSound(SoundHandle* handle, 
    u32 soundId, const StartInfo* startInfo, void* setupArg)
{
    if (m_IsInitialized == false)
    {
        return StartResult(StartResult::START_ERR_NOT_AVAILABLE);
    }

    Sound3DParam param;
    param.position = m_Position;
    param.velocity = m_Velocity;
    param.actorUserParam = m_UserParam;
    if (m_pArchivePlayer != NULL)
    {
        const SoundArchive& soundArchive = m_pArchivePlayer->GetSoundArchive();

        SoundArchive::Sound3DInfo p;
        if (soundArchive.ReadSound3DInfo(soundId, &p))
        {
            param.ctrlFlag = p.flags;
            param.decayRatio = p.decayRatio;
            param.dopplerFactor = p.dopplerFactor;

            switch (p.decayCurve)
            {
            case SoundArchive::Sound3DInfo::DECAY_CURVE_LOG:
                param.decayCurve = nw::snd::DECAY_CURVE_LOG;
                break;
            case SoundArchive::Sound3DInfo::DECAY_CURVE_LINEAR:
                param.decayCurve = nw::snd::DECAY_CURVE_LINEAR;
                break;
            default:
                param.decayCurve = nw::snd::DECAY_CURVE_LOG;
                break;
            }
        }
        param.soundUserParam = soundArchive.GetSoundUserParam(soundId);
    }

    internal::BasicSound::AmbientInfo argInfo = 
    {
        m_p3dManager,
        this,
        m_p3dManager,
        &param,
        sizeof(param)
    };

    SoundStartable::StartResult result = SoundActor::detail_SetupSoundWithAmbientInfo(handle, soundId, startInfo, &argInfo, setupArg);

    if (handle->IsAttachedSound())
    {
        handle->detail_GetAttachedSound()->SetPanCurve(PAN_CURVE_SINCOS);
    }

    return result;
}

void Sound3DActor::SetPosition(const nw::math::VEC3& position)
{
    if (!m_ResetPositionFlag) 
    {
        nw::math::VEC3Sub(&m_Velocity, &position, &m_Position);
    }
    m_Position = position;
    m_ResetPositionFlag = false;
}

void Sound3DActor::ResetPosition()
{
    m_ResetPositionFlag = true;
    m_Position = m_Velocity = nw::math::VEC3(0.0f, 0.0f, 0.0f);
}

void Sound3DActor::SetVelocity(const nw::math::VEC3& velocity)
{
    m_Velocity = velocity;
}

void Sound3DActor::detail_UpdateAmbientArg(void* arg, const internal::BasicSound* sound)
{
    NW_UNUSED_VARIABLE(sound);

    Sound3DParam* param = static_cast< Sound3DParam* >(arg);
    param->position = m_Position;
    param->velocity = m_Velocity;
    param->actorUserParam = m_UserParam;
}

void Sound3DActor::ClearUpdateCallback(SoundHandle& handle)
{
    if (handle.IsAttachedSound())
    {
        handle.detail_GetAttachedSound()->ClearAmbientArgUpdateCallback();
    }
}

} // namespace snd
} // namespace nw