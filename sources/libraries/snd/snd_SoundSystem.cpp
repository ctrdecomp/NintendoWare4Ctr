// Filename: snd_SoundSystem.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundSystem.h>
#include <nw/snd/snd_Voice.h>
#include <nw/snd/snd_Channel.h>
#include <nw/snd/snd_ChannelManager.h>
#include <nw/snd/snd_DriverCommandManager.h>
#include <nw/snd/snd_SequenceSoundPlayer.h>
#include <nw/snd/snd_TaskManager.h>
#include <nw/ut/ut_MiddlewareString.h>
#include <nw/version.h>

namespace nw {
namespace snd {

internal::ThreadStack SoundSystem::s_LoadThreadStack;
bool SoundSystem::s_AutoCreateSoundThreadFlag = false;
bool SoundSystem::s_IsInitialized = false;
bool SoundSystem::s_IsStreamLoadWait = false;
bool SoundSystem::s_IsEnterSleep = false;
bool SoundSystem::s_IsInitializedDriverCommandManager = false;
int SoundSystem::s_MaxVoiceCount = NN_SND_VOICE_NUM;

void SoundSystem::detail_InitializeDriverCommandManager(const SoundSystemParam& param, uptr workMem, size_t workMemSize )
{
    if (s_IsInitializedDriverCommandManager)
    {
        return;
    }

    uptr ptr = workMem;

    uptr soundThreadCommandBufferPtr = ptr;
    ptr += param.soundThreadCommandBufferSize;

    uptr taskThreadCommandBufferPtr = ptr;
    ptr += param.taskThreadCommandBufferSize;

    internal::DriverCommandManager::GetInstance().Initialize(reinterpret_cast<void*>(soundThreadCommandBufferPtr),param.soundThreadCommandBufferSize);
    internal::DriverCommandManager::GetInstanceForTaskThread().Initialize(reinterpret_cast<void*>(taskThreadCommandBufferPtr),param.taskThreadCommandBufferSize);

    s_IsInitializedDriverCommandManager = true;
}

void SoundSystem::Initialize(const SoundSystemParam& param, uptr workMem, size_t workMemSize)
{
    if (s_IsInitialized)
    {
        return;
    }

    uptr ptr = workMem;

    if (s_IsInitializedDriverCommandManager == false)
    {
        size_t workMemSizeForDriverCommandManager = detail_GetRequiredDriverCommandManagerMemSize(param);
        detail_InitializeDriverCommandManager(param, ptr, workMemSizeForDriverCommandManager);
        ptr += workMemSizeForDriverCommandManager;
    }

    uptr soundThreadStackPtr = NULL;
    uptr userSoundThreadStackPtr = NULL;

    if (param.autoCreateSoundThread)
    {
        soundThreadStackPtr = ptr;
        ptr += param.soundThreadStackSize;
        if (param.soundThreadCoreNo == 1)
        {
            userSoundThreadStackPtr = ptr;
            ptr += param.soundThreadStackSize;
        }
    }

    internal::driver::SoundThread::GetInstance().Initialize();

    uptr loadThreadStackPtr = ptr;
    ptr += param.taskThreadStackSize;

    internal::TaskManager::GetInstance().Initialize();

    s_LoadThreadStack.Initialize(loadThreadStackPtr, param.taskThreadStackSize);

    uptr voiceWork = ptr;
    ptr += internal::driver::VoiceManager::GetInstance().GetRequiredMemSize(s_MaxVoiceCount);
    internal::driver::VoiceManager::GetInstance().Initialize(reinterpret_cast<void*>(voiceWork), internal::driver::VoiceManager::GetInstance().GetRequiredMemSize(s_MaxVoiceCount));

    uptr channelWork = ptr;
    ptr += internal::driver::ChannelManager::GetInstance().GetRequiredMemSize(s_MaxVoiceCount);
    internal::driver::ChannelManager::GetInstance().Initialize(reinterpret_cast<void*>(channelWork), internal::driver::ChannelManager::GetInstance().GetRequiredMemSize(s_MaxVoiceCount));

    internal::driver::SequenceSoundPlayer::InitSequenceSoundPlayer();

    internal::TaskThread::GetInstance().Create(param.taskThreadPriority, s_LoadThreadStack);

    if (param.autoCreateSoundThread)
    {
        internal::driver::SoundThread::GetInstance().CreateSoundThread(soundThreadStackPtr, param.soundThreadStackSize, param.soundThreadPriority,
                userSoundThreadStackPtr, param.soundThreadStackSize, param.soundThreadPriority,
                param.soundThreadCoreNo, param.enableGetSoundThreadTick);
    }

    s_AutoCreateSoundThreadFlag = param.autoCreateSoundThread;
    s_IsInitialized = true;
}

void SoundSystem::Finalize()
{
    if (!s_IsInitialized)
    {
        return;
    }
    // Tasks
    internal::TaskManager::GetInstance().CancelAllTask();
    internal::TaskThread::GetInstance().Destroy();
    internal::TaskManager::GetInstance().Finalize();

    // Managers
    internal::driver::SoundThread::GetInstance().Finalize();
    internal::driver::ChannelManager::GetInstance().Finalize();
    internal::driver::VoiceManager::GetInstance().Finalize();
    internal::driver::HardwareManager::GetInstance().Finalize();
    s_LoadThreadStack.Finalize();
    internal::driver::SoundThread::GetInstance().Finalize();
    internal::DriverCommandManager::GetInstance().Finalize();
    internal::DriverCommandManager::GetInstanceForTaskThread().Finalize();
    s_IsInitializedDriverCommandManager = false;
    s_IsInitialized = false;
}

bool SoundSystem::IsInitialized()
{
    return s_IsInitialized;
}

size_t SoundSystem::GetRequiredMemSize(const SoundSystemParam& param)
{
    size_t result = detail_GetRequiredDriverCommandManagerMemSize(param) + param.taskThreadStackSize
    + internal::driver::VoiceManager::GetInstance().GetRequiredMemSize(s_MaxVoiceCount)
    + internal::driver::ChannelManager::GetInstance().GetRequiredMemSize(s_MaxVoiceCount);

    if (param.autoCreateSoundThread == true)
    {
        result += param.soundThreadStackSize;
    }

    return result;
}

bool SoundSystem::AppendEffect(AuxBus bus, FxBase* effect)
{
    if (!effect->Initialize())
    {
        return false;
    }
    
    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    
    internal::DriverCommandEffect* command = cmdmgr.AllocCommand<internal::DriverCommandEffect>();
    command->id = internal::DRIVER_COMMAND_APPEND_EFFECT;
    command->bus = bus;
    command->effect = effect;
    cmdmgr.PushCommand(command);
    
    return true;
}

bool SoundSystem::AppendEffect(AuxBus bus, nn::snd::CTR::FxDelay* fxDelay)
{
    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    
    internal::DriverCommandEffectSdkDelay* command = cmdmgr.AllocCommand<internal::DriverCommandEffectSdkDelay>();
    command->id = internal::DRIVER_COMMAND_APPEND_EFFECT_SDK_DELAY;
    command->bus = bus;
    command->effect = fxDelay;
    cmdmgr.PushCommand(command);

    return true;
}

bool SoundSystem::AppendEffect(AuxBus bus, nn::snd::CTR::FxReverb* fxReverb)
{
    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    
    internal::DriverCommandEffectSdkReverb* command = cmdmgr.AllocCommand<internal::DriverCommandEffectSdkReverb>();
    command->id = internal::DRIVER_COMMAND_APPEND_EFFECT_SDK_REVERB;
    command->bus = bus;
    command->effect = fxReverb;
    cmdmgr.PushCommand(command);

    return true;
}

void SoundSystem::ClearEffect(AuxBus bus, int fadeTimes)
{
    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    
    internal::DriverCommandEffect* command = cmdmgr.AllocCommand<internal::DriverCommandEffect>();
    command->id = internal::DRIVER_COMMAND_CLEAR_EFFECT;
    command->bus = bus;
    command->fadeTimes = fadeTimes;
    cmdmgr.PushCommand(command);
    
    u32 tag = cmdmgr.FlushCommand(true);
    cmdmgr.WaitCommandReply(tag);
}
} // namespace snd
} // namespace nw