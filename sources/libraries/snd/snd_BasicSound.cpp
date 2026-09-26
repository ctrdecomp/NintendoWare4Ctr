// Filename: snd_BasicSound.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_BasicSound.h>
#include <nw/snd/snd_BasicSoundPlayer.h>
#include <nw/snd/snd_SoundPlayer.h>
#include <nw/snd/snd_ExternalSoundPlayer.h>
#include <nw/snd/snd_SoundActor.h>
#include <nw/snd/snd_DriverCommandManager.h>
#include <nw/snd/snd_DriverCommand.h>
#include <nw/snd/snd_SoundHandle.h>
#include <nw/snd/snd_SoundPlayer.h>
#include <cstring>

namespace nw {
namespace snd {
namespace internal {

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(BasicSound);

BasicSound::BasicSound(): 
    m_InitializeFlag(false)
{
}

void BasicSound::Initialize()
{
    driver::BasicSoundPlayer* basicPlayer = GetBasicSoundPlayerHandle();
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    
    m_pPlayerHeap = NULL;
    m_pGeneralHandle = NULL;
    m_pTempGeneralHandle = NULL;
    m_pSoundPlayer = NULL;
    m_pSoundActor = NULL;
    m_pExtSoundPlayer = NULL;
    m_PlayerState = PLAYER_STATE_INIT;
    m_Id = INVALID_ID;
        
    m_AmbientInfo.paramUpdateCallback = NULL;
    m_AmbientInfo.argUpdateCallback = NULL;
    m_AmbientInfo.argAllocatorCallback = NULL;
    m_AmbientInfo.arg = NULL;
    m_AmbientInfo.argSize = 0;

    m_PlayerState   = PLAYER_STATE_INIT;
    m_PauseState    = PAUSE_STATE_NORMAL;
    m_UnPauseFlag    = false;
    m_StartFlag      = false;
    m_StartedFlag    = false;
    m_AutoStopFlag   = false;
    m_FadeOutFlag    = false;
    m_PlayerAvailableFlag = false;

    m_AutoStopCounter = 0;
    m_UpdateCounter  = 0;

    m_FadeVolume.InitValue(1.0f);
    m_PauseFadeVolume.InitValue(1.0f);

    m_InitVolume     = 1.0f;
    m_ExtPitch       = 1.0f;
    m_ExtPan         = 0.0f;
    m_ExtMoveVolume.InitValue(1.0f);
    m_LpfFreq = 0.0f;
    m_BiquadFilterType = -1; // Type None
    m_BiquadFilterValue = 0.0f;
    m_ExtSurroundPan = 0.0f;

    m_MainSend = 0.0f;
    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        m_FxSend[i] = 0.0f;
    }

    // AmbientParam
    m_AmbientParam.volume            = 1.0f;
    m_AmbientParam.pitch             = 1.0f;
    m_AmbientParam.pan               = 0.0f;
    m_AmbientParam.span              = 0.0f;
    m_AmbientParam.fxSend            = 0.0f;
    m_AmbientParam.lpf               = 0.0f;
    m_AmbientParam.biquadFilterValue = 0.0f;
    m_AmbientParam.biquadFilterType  = 0;
    m_AmbientParam.priority          = 0;

    m_ActorParam.Reset();
    
    DriverCommandPlayerInit* command = cmdmgr.AllocCommand<DriverCommandPlayerInit>();
    command->id = DRIVER_COMMAND_PLAYER_INIT;
    command->player = basicPlayer;
    command->availableFlagPtr = &m_PlayerAvailableFlag;
    cmdmgr.PushCommand(command);

    m_InitializeFlag = true;
}

// Priority

void BasicSound::SetPriority(int priority, int ambientPriority)
{
    NW_MINMAX_ASSERT(priority, PRIORITY_MIN, PRIORITY_MAX);
    
    m_Priority = static_cast<u8>(priority);
    m_AmbientParam.priority = ambientPriority;
}

void BasicSound::GetPriority(int* priority, int* ambientPriority) const
{
    if (priority != NULL)
    {
        *priority = m_Priority;
    }
    if (ambientPriority != NULL)
    {
        *ambientPriority = m_AmbientParam.priority;
    }
}

void BasicSound::Finalize()
{
    if (!m_InitializeFlag)
    {
        return;
    }
    
    if (m_StartedFlag)
    {
        DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    
        DriverCommandPlayer* command = cmdmgr.AllocCommand<DriverCommandPlayer>();
        command->id = DRIVER_COMMAND_PLAYER_STOP;
        command->player = GetBasicSoundPlayerHandle();
        command->flag = m_FadeOutFlag;
        cmdmgr.PushCommand(command);
    }
    m_PlayerAvailableFlag = false;
    m_PlayerState = PLAYER_STATE_STOP;

    SetId(INVALID_ID);

    if (IsAttachedGeneralHandle())
    {
        DetachGeneralHandle();
    }

    if (IsAttachedTempGeneralHandle())
    {
        DetachTempGeneralHandle();
    }

    if (IsAttachedTempSpecialHandle())
    {
        DetachTempSpecialHandle();
    }

    if (m_pPlayerHeap != NULL) 
    {
        m_pSoundPlayer->detail_FreePlayerHeap(this);
    }

    if (m_pSoundPlayer != NULL)
    {
        m_pSoundPlayer->detail_RemoveSound(this);
    }

    if (m_pExtSoundPlayer != NULL)
    {
        m_pExtSoundPlayer->RemoveSound(this);
    }

    if (m_AmbientInfo.argAllocatorCallback != NULL)
    {
        m_AmbientInfo.argAllocatorCallback->detail_FreeAmbientArg(m_AmbientInfo.arg, this);
        m_AmbientInfo.arg = NULL;
    }

    m_StartedFlag = false;
    m_FadeOutFlag = false;

    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandPlayer* command = cmdmgr.AllocCommand<DriverCommandPlayer>();
    command->id = DRIVER_COMMAND_PLAYER_FINALIZE;
    command->player = GetBasicSoundPlayerHandle();
    cmdmgr.PushCommand(command);

    m_InitializeFlag = false;
}

void BasicSound::StartPrepared()
{
    m_StartFlag = true;
}

void BasicSound::Stop( int fadeFrames )
{
    if ((fadeFrames == 0 ) || (m_PlayerState != PLAYER_STATE_PLAY) ||
        (m_PauseState == PAUSE_STATE_PAUSED))
    {
        Finalize();
        return;
    }
    
    int frames = static_cast<int>(fadeFrames * m_FadeVolume.GetValue());
    m_FadeVolume.SetTarget(0.0f, frames);
    SetPlayerPriority(PRIORITY_MIN);
    m_AutoStopFlag = false;
    m_PauseState = PAUSE_STATE_NORMAL;
    m_UnPauseFlag = false;
    m_FadeOutFlag = true;
}

void BasicSound::Pause(bool flag, int fadeFrames)
{
    int frames;
    if (flag)
    {
        switch(m_PauseState) {
        case PAUSE_STATE_NORMAL:
        case PAUSE_STATE_PAUSING:
        case PAUSE_STATE_UNPAUSING:
            frames = static_cast<int>(fadeFrames * m_PauseFadeVolume.GetValue());
            if (frames <= 0)
            {
                frames = 1;
            }
            m_PauseFadeVolume.SetTarget(0.0f, frames);
            m_PauseState = PAUSE_STATE_PAUSING;
            m_UnPauseFlag = false;
            break;
        case PAUSE_STATE_PAUSED:
            //! Fallthrough
            return;
        default:
            NW_ASSERTMSG(false, "Unexpected pause state %d", m_PauseState);
            return;
        }
    }
    else
    {
        switch(m_PauseState) {
        case PAUSE_STATE_NORMAL:
            //! Fallthrough
            return;
        case PAUSE_STATE_PAUSING:
        case PAUSE_STATE_UNPAUSING:
        case PAUSE_STATE_PAUSED:
            frames = static_cast<int>(fadeFrames * ( 1.0f - m_PauseFadeVolume.GetValue()));
            if (frames <= 0)
            {
                frames = 1;
            }
            m_PauseFadeVolume.SetTarget(1.0f, frames);
            m_PauseState = PAUSE_STATE_UNPAUSING;
            m_UnPauseFlag = true;
            break;
        default:
            NW_ASSERTMSG(false, "Unexpected pause state %d", m_PauseState);
            return;
        }
    }
}

void BasicSound::SetAutoStopCounter(int frames)
{
    m_AutoStopCounter = frames;
    m_AutoStopFlag = (frames > 0);
}

void BasicSound::FadeIn(int frames)
{
    if (m_FadeOutFlag)
    {
        return;
    }
    if (m_UpdateCounter == 0)
    {
        m_FadeVolume.InitValue(0.0f);
        m_FadeVolume.SetTarget(1.0f, frames);
    }
}

bool BasicSound::IsPause() const
{
    return (m_PauseState == PAUSE_STATE_PAUSING) || (m_PauseState == PAUSE_STATE_PAUSED);
}

void BasicSound::Update()
{
    if (m_AutoStopFlag)
    {
        if (m_AutoStopCounter == 0)
        {
            if ((m_PauseState == PAUSE_STATE_NORMAL) ||
                (m_PauseState == PAUSE_STATE_UNPAUSING))
            {
                Stop(0);
                return;
            }
        }
        else
        {
            --m_AutoStopCounter;
        }
    }

    bool playerStartFlag = false;
    if (!m_StartedFlag)
    {
        if (!m_StartFlag) 
            return;
        if (!IsPrepared()) 
            return;
        
        m_PlayerState = PLAYER_STATE_PLAY;
        playerStartFlag = true;
    }

    if (m_PlayerState == PLAYER_STATE_PLAY)
    {
        if (m_UpdateCounter < 0xffffffff)
        {
            m_UpdateCounter++;
        }
    }

    if (m_PlayerAvailableFlag && GetBasicSoundPlayerHandle()->IsPlayFinished())
    {
        Finalize();
        return;
    }

    switch (m_PauseState)
    {
    case PAUSE_STATE_PAUSING:
        m_PauseFadeVolume.Update();
        break;
    case PAUSE_STATE_UNPAUSING:
        m_PauseFadeVolume.Update();
        UpdateMoveValue();
        break;
    case PAUSE_STATE_NORMAL:
        UpdateMoveValue();
        break;
    }

    if (m_AmbientInfo.argUpdateCallback != NULL)
    {
        m_AmbientInfo.argUpdateCallback->detail_UpdateAmbientArg(m_AmbientInfo.arg, this);
    }

    if (m_AmbientInfo.paramUpdateCallback != NULL)
    {
        SoundAmbientParam ambientParam;
        if (m_UpdateCounter > 0)
        {
            ambientParam.volume            = m_AmbientParam.volume;
            ambientParam.pitch             = m_AmbientParam.pitch;
            ambientParam.pan               = m_AmbientParam.pan;
            ambientParam.span              = m_AmbientParam.span;
            ambientParam.fxSend            = m_AmbientParam.fxSend;
            ambientParam.lpf               = m_AmbientParam.lpf;
            ambientParam.biquadFilterValue = m_AmbientParam.biquadFilterValue;
            ambientParam.biquadFilterType  = m_AmbientParam.biquadFilterType;
            ambientParam.priority          = m_AmbientParam.priority;
            ambientParam.userData          = m_AmbientParam.userData;
        }
        else
        {
            ambientParam.userData = 0;
        }
        m_AmbientInfo.paramUpdateCallback->detail_UpdateAmbientParam(
            m_AmbientInfo.arg,
            m_Id,
            &ambientParam
        );

        m_AmbientParam.volume            = ambientParam.volume;
        m_AmbientParam.pitch             = ambientParam.pitch;
        m_AmbientParam.pan               = ambientParam.pan;
        m_AmbientParam.span              = ambientParam.span;
        m_AmbientParam.fxSend            = ambientParam.fxSend;
        m_AmbientParam.lpf               = ambientParam.lpf;
        m_AmbientParam.biquadFilterValue = ambientParam.biquadFilterValue;
        m_AmbientParam.biquadFilterType  = ambientParam.biquadFilterType;
        m_AmbientParam.priority          = ambientParam.priority;
        m_AmbientParam.userData          = ambientParam.userData;

    }

    if (m_pSoundActor != NULL)
    {
        m_ActorParam = m_pSoundActor->detail_GetActorParam();
    }

    UpdateParam();

    if (m_FadeOutFlag && m_FadeVolume.IsFinished())
    {
        m_FadeOutFlag = false;
        Finalize();
        return;
    }
    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();

    if (playerStartFlag)
    {
        DriverCommandPlayer* command = cmdmgr.AllocCommand<DriverCommandPlayer>();
        command->id = DRIVER_COMMAND_PLAYER_START;
        command->player = GetBasicSoundPlayerHandle();
        cmdmgr.PushCommand(command);
        
        m_StartedFlag = true;
        m_StartFlag = false;
    }

    if (m_PauseState == PAUSE_STATE_PAUSING)
    {
        if (m_PauseFadeVolume.IsFinished())
        {
            DriverCommandPlayer* command = cmdmgr.AllocCommand<DriverCommandPlayer>();
            command->id = DRIVER_COMMAND_PLAYER_PAUSE;
            command->player = GetBasicSoundPlayerHandle();
            command->flag = true;
            cmdmgr.PushCommand(command);
        
            m_PauseState = PAUSE_STATE_PAUSED;
        }
    }
    else if (m_PauseState == PAUSE_STATE_UNPAUSING)
    {
        if (m_PauseFadeVolume.IsFinished())
        {
            m_PauseState = PAUSE_STATE_NORMAL;
        }
    }
    if (m_UnPauseFlag) {
        DriverCommandPlayer* command = cmdmgr.AllocCommand<DriverCommandPlayer>();
        command->id = DRIVER_COMMAND_PLAYER_PAUSE;
        command->player = GetBasicSoundPlayerHandle();
        command->flag = false;
        cmdmgr.PushCommand(command);
        
        m_UnPauseFlag = false;
    }
}

void BasicSound::UpdateMoveValue()
{
    m_FadeVolume.Update();
    m_ExtMoveVolume.Update();
}

void BasicSound::UpdateParam()
{
    f32 volume = 1.0f;
    volume *= m_InitVolume;
    volume *= GetSoundPlayer()->GetVolume();
    volume *= m_ExtMoveVolume.GetValue();
    volume *= m_FadeVolume.GetValue();
    volume *= m_PauseFadeVolume.GetValue();
    volume *= m_AmbientParam.volume;
    volume *= m_ActorParam.volume;

    f32 pan = 0.0f;
    pan += m_ExtPan;
    pan += m_AmbientParam.pan;
    pan += m_ActorParam.pan;

    f32 surroundPan = 0.0f;
    surroundPan += m_ExtSurroundPan;
    surroundPan += m_AmbientParam.span;

    f32 pitch = 1.0f;
    pitch *= m_ExtPitch;
    pitch *= m_AmbientParam.pitch;
    pitch *= m_ActorParam.pitch;

    f32 lpfFreq = m_LpfFreq;
    lpfFreq += m_AmbientParam.lpf;
    lpfFreq += GetSoundPlayer()->GetLpfFreq();

    int biquadFilterType = m_BiquadFilterType;
    f32 biquadFilterValue = m_BiquadFilterValue;
    if (biquadFilterType == -1 /* Invalid Type */)
    {
        biquadFilterType = GetSoundPlayer()->GetBiquadFilterType();
        biquadFilterValue = GetSoundPlayer()->GetBiquadFilterValue();

        if(biquadFilterType == -1 /* Invalid Type */) 
        {
            biquadFilterType = m_AmbientParam.biquadFilterType;
            biquadFilterValue = m_AmbientParam.biquadFilterValue;
        }
    }

    f32 mainSend = 0.0f;
    mainSend += m_MainSend;
    mainSend += GetSoundPlayer()->GetMainSend();

    f32 fxSend[AUX_BUS_NUM];
    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        fxSend[i] = 0.0f;
        fxSend[i] += m_FxSend[i];
        fxSend[i] += GetSoundPlayer()->GetFxSend( static_cast<AuxBus>(i));
    }
    fxSend[0] += m_AmbientParam.fxSend;

    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    
    DriverCommandPlayerParam* command = cmdmgr.AllocCommand<DriverCommandPlayerParam>();
    command->id = DRIVER_COMMAND_PLAYER_PARAM;
    command->player = GetBasicSoundPlayerHandle();
    command->volume = volume;
    command->pitch = pitch;
    command->pan = pan ;
    command->surroundPan = surroundPan;
    command->lpfFreq = lpfFreq;
    command->biquadFilterType = biquadFilterType;
    command->biquadFilterValue = biquadFilterValue;
    command->mainSend = mainSend;
    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        command->fxSend[i] = fxSend[i];
    }
    
    cmdmgr.PushCommand(command);
}

// PlayerHeap

void BasicSound::AttachPlayerHeap(PlayerHeap* pHeap)
{
    NW_NULL_ASSERT(pHeap);
    NW_ASSERT(m_pPlayerHeap == NULL);

    m_pPlayerHeap = pHeap;
}

void BasicSound::DetachPlayerHeap(PlayerHeap* pHeap )
{
    NW_NULL_ASSERT(pHeap);
    NW_ASSERT(pHeap == m_pPlayerHeap);
    (void)pHeap;

    m_pPlayerHeap = NULL;
}

// SoundPlayer

void BasicSound::AttachSoundPlayer(SoundPlayer* player)
{
    NW_NULL_ASSERT(player);
    NW_ASSERT(m_pSoundPlayer == NULL);

    m_pSoundPlayer = player;
}

void BasicSound::DetachSoundPlayer(SoundPlayer* player)
{
    NW_NULL_ASSERT(player);
    NW_ASSERT(player == m_pSoundPlayer);
    (void)player;

    m_pSoundPlayer = NULL;
}

// SoundActor

void BasicSound::AttachSoundActor(SoundActor* actor)
{
    NW_NULL_ASSERT(actor);
    NW_ASSERT(m_pSoundActor == NULL);

    m_pSoundActor = actor;
}

void BasicSound::DetachSoundActor(SoundActor* actor)
{
    NW_NULL_ASSERT(actor);
    NW_ASSERT(actor == m_pSoundActor);
    (void)actor;

    m_pSoundActor = NULL;
}

// ExternalSoundPlayer

void BasicSound::AttachExternalSoundPlayer(ExternalSoundPlayer* extPlayer)
{
    NW_NULL_ASSERT(extPlayer);
    NW_ASSERT(m_pExtSoundPlayer == NULL);

    m_pExtSoundPlayer = extPlayer;
}

void BasicSound::DetachExternalSoundPlayer(ExternalSoundPlayer* extPlayer)
{
    NW_NULL_ASSERT(extPlayer);
    NW_ASSERT(extPlayer == m_pExtSoundPlayer);
    (void)extPlayer;

    m_pExtSoundPlayer = NULL;
}

// FadeFrames

int BasicSound::GetRemainingFadeFrames() const
{
    return m_FadeVolume.GetRemainingCount();
}

int BasicSound::GetRemainingPauseFadeFrames() const
{
    return m_PauseFadeVolume.GetRemainingCount();
}

void BasicSound::SetPlayerPriority(int priority)
{
    NW_MINMAX_ASSERT(priority, PRIORITY_MIN, PRIORITY_MAX);
    m_Priority = static_cast<u8>(priority);
    if (m_pSoundPlayer != NULL)
    {
        m_pSoundPlayer->detail_SortPriorityList(this);
    }

    OnUpdatePlayerPriority();
}

// InitialVolume

void BasicSound::SetInitialVolume(f32 volume)
{
    NW_ASSERT(volume >= 0.0f);
    if (volume < 0.0f)
    { 
        volume = 0.0f; 
    }
    m_InitVolume = volume;
}

f32 BasicSound::GetInitialVolume() const
{
    return m_InitVolume;
}

// Volume

void BasicSound::SetVolume(f32 volume, int frames)
{
    NW_ASSERT(volume >= 0.0f);
    if (volume < 0.0f)
    {
        volume = 0.0f;
    }
    m_ExtMoveVolume.SetTarget(volume, frames);
}

f32 BasicSound::GetVolume() const
{
    return m_ExtMoveVolume.GetValue();
}

// Pitch

void BasicSound::SetPitch(f32 pitch)
{
    NW_ASSERT(pitch >= 0.0f);
    m_ExtPitch = pitch;
}
f32 BasicSound::GetPitch() const
{
    return m_ExtPitch;
}

// Pan

void BasicSound::SetPan(f32 pan)
{
    m_ExtPan = pan;
}
f32 BasicSound::GetPan() const
{
    return m_ExtPan;
}

// LpfFreq

void BasicSound::SetLpfFreq(f32 lpfFreq)
{
    m_LpfFreq = lpfFreq;
}
f32 BasicSound::GetLpfFreq() const
{
    return m_LpfFreq;
}

// BiqaudFilter

void BasicSound::SetBiquadFilter(int type, f32 value)
{
    m_BiquadFilterType = static_cast<u8>(type);
    m_BiquadFilterValue = value;
}
void BasicSound::GetBiquadFilter(int* type, f32* value) const
{
    if (type != NULL)
    {
        *type = m_BiquadFilterType;
    }
    if (value != NULL)
    {
        *value = m_BiquadFilterValue;
    }
}

// SurroundPan

void BasicSound::SetSurroundPan(f32 surroundPan)
{
    m_ExtSurroundPan = surroundPan;
}

f32 BasicSound::GetSurroundPan() const
{
    return m_ExtSurroundPan;
}

// MainSend

void BasicSound::SetMainSend(f32 send)
{
    m_MainSend = send;
}

f32 BasicSound::GetMainSend() const
{
    return m_MainSend;
}

// FxSend

void BasicSound::SetFxSend(AuxBus bus, f32 send)
{
    NW_MINMAXLT_ASSERT(bus, 0, AUX_BUS_NUM);
    m_FxSend[bus] = send;
}

f32 BasicSound::GetFxSend(AuxBus bus) const
{
    if (bus < 0 || bus >= AUX_BUS_NUM)
    {
        return 0.0f;
    }
    return m_FxSend[bus];
}

// PanModes

void BasicSound::SetPanMode(PanMode panMode)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    
    DriverCommandPlayerPanParam* command = cmdmgr.AllocCommand<DriverCommandPlayerPanParam>();
    command->id = DRIVER_COMMAND_PLAYER_PANMODE;
    command->player = GetBasicSoundPlayerHandle();
    command->panMode = panMode;
    cmdmgr.PushCommand(command);
}

void BasicSound::SetPanCurve(PanCurve panCurve)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    
    DriverCommandPlayerPanParam* command = cmdmgr.AllocCommand<DriverCommandPlayerPanParam>();
    command->id = DRIVER_COMMAND_PLAYER_PANCURVE;
    command->player = GetBasicSoundPlayerHandle();
    command->panCurve = panCurve;
    cmdmgr.PushCommand(command);
}

void BasicSound::SetFrontBypass(bool isFrontBypass)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    
    DriverCommandPlayer* command = cmdmgr.AllocCommand<DriverCommandPlayer>();
    command->id = DRIVER_COMMAND_PLAYER_FRONTBYPASS;
    command->player = GetBasicSoundPlayerHandle();
    command->flag = isFrontBypass;
    cmdmgr.PushCommand(command);
}

void BasicSound::SetAmbientInfo(const AmbientInfo& ambientArgInfo)
{
    NW_NULL_ASSERT(ambientArgInfo.argAllocatorCallback);
    void* ambientArg = ambientArgInfo.argAllocatorCallback->detail_AllocAmbientArg(ambientArgInfo.argSize);
    if (ambientArg == NULL)
    {
        NW_WARNING(ambientArg != NULL, "Failed to alloc AmbientArg.");
        return;
    }
    std::memcpy(ambientArg, ambientArgInfo.arg, ambientArgInfo.argSize);

    m_AmbientInfo = ambientArgInfo;
    m_AmbientInfo.arg = ambientArg;
}

int BasicSound::GetAmbientPriority(const AmbientInfo& ambientInfo, u32 soundId)
{
    if (ambientInfo.paramUpdateCallback == NULL)
    {
        return 0;
    }

    int priority = ambientInfo.paramUpdateCallback->detail_GetAmbientPriority(
        ambientInfo.arg,
        soundId
    );

    return priority;
}

bool BasicSound::IsAttachedGeneralHandle()
{
    return m_pGeneralHandle != NULL;
}

bool BasicSound::IsAttachedTempGeneralHandle()
{
    return m_pTempGeneralHandle != NULL;
}

void BasicSound::DetachGeneralHandle()
{
    m_pGeneralHandle->DetachSound();
}

void BasicSound::DetachTempGeneralHandle()
{
    m_pTempGeneralHandle->DetachSound();
}

void BasicSound::SetId(u32 id)
{
    m_Id = id;
}

}
}
}