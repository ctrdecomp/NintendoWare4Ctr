#ifndef NW_SND_SOUND_SYSTEM_H_
#define NW_SND_SOUND_SYSTEM_H_

#include <nn/os.h>
#include <nn/snd.h>
#include <nw/snd/snd_Config.h>
#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_HardwareManager.h>
#include <nw/snd/snd_VoiceManager.h>
#include <nw/snd/snd_SoundThread.h>
#include <nw/snd/snd_MmlParser.h>
#include <nw/snd/snd_TaskThread.h>
#include <nw/snd/snd_ThreadStack.h>

namespace nw {
namespace snd {

class SoundSystem
{
public:
    struct SoundSystemParam
    {
        s32 soundThreadPriority;
        s32 soundThreadCoreNo;
        size_t soundThreadStackSize;
        size_t soundThreadCommandBufferSize;
        s32 taskThreadPriority;
        size_t taskThreadStackSize;
        size_t taskThreadCommandBufferSize;
        bool autoCreateSoundThread;
        bool enableGetSoundThreadTick;

        static const s32 DEFAULT_SOUND_THREAD_PRIORITY = 4;
        static const s32 DEFAULT_TASK_THREAD_PRIORITY = 3;
        static const u32 DEFAULT_SOUND_THREAD_STACK_SIZE = 16 * 1024;
        static const u32 DEFAULT_TASK_THREAD_STACK_SIZE = 16 * 1024;

        static const u32 DEFAULT_SOUND_THREAD_COMMAND_BUFFER_SIZE = 128 * 1024;
        static const u32 DEFAULT_TASK_THREAD_COMMAND_BUFFER_SIZE = 8 * 1024;

        SoundSystemParam(): 
            soundThreadPriority(DEFAULT_SOUND_THREAD_PRIORITY),
            soundThreadCoreNo(0),
            soundThreadStackSize(DEFAULT_SOUND_THREAD_STACK_SIZE),
            soundThreadCommandBufferSize(DEFAULT_SOUND_THREAD_COMMAND_BUFFER_SIZE),
            taskThreadPriority(DEFAULT_TASK_THREAD_PRIORITY),
            taskThreadStackSize(DEFAULT_TASK_THREAD_STACK_SIZE),
            taskThreadCommandBufferSize(DEFAULT_TASK_THREAD_COMMAND_BUFFER_SIZE),
            autoCreateSoundThread(true),
            enableGetSoundThreadTick(true)
        {
        }
    };

    static void Initialize(const SoundSystemParam& param, uptr workMem, size_t workMemSize);
    static size_t GetRequiredMemSize(const SoundSystemParam& param);
    static size_t detail_GetRequiredDriverCommandManagerMemSize(const SoundSystemParam& param);
    static void detail_InitializeDriverCommandManager(const SoundSystemParam& param, uptr workMem, size_t workMemSize);
    static void Finalize();
    static bool IsInitialized();

    static void LockSoundThread()
    {
        internal::driver::SoundThread::GetInstance().Lock();
    }

    static void UnlockSoundThread()
    {
        internal::driver::SoundThread::GetInstance().Unlock();
    }

    class SoundThreadScopedLock
    {
    public:
        SoundThreadScopedLock()
        {
            LockSoundThread();
        }

        ~SoundThreadScopedLock()
        {
            UnlockSoundThread();
        }
    private:
        NW_DISALLOW_COPY_AND_ASSIGN( SoundThreadScopedLock );
    };

    static void EnterSleep()
    {
        if (s_IsEnterSleep)
        {
            return;
        }

        s_IsEnterSleep = true;
        s_IsStreamLoadWait = true;
        internal::TaskThread::GetInstance().Lock();
    }

    static void LeaveSleep()
    {
        if (!s_IsEnterSleep)
        {
            return;
        }

        internal::TaskThread::GetInstance().Unlock();
        s_IsStreamLoadWait = false;
        s_IsEnterSleep = false;
    }

    static bool detail_IsStreamLoadWait()
    {
        return s_IsStreamLoadWait;
    }

    static void SetOutputMode(OutputMode mode)
    {
        internal::driver::HardwareManager::GetInstance().SetOutputMode( mode );
    }

    static OutputMode GetOutputMode()
    {
        return internal::driver::HardwareManager::GetInstance().GetOutputMode();
    }

    static void SetMasterVolume(float volume, int fadeTimes)
    {
        internal::driver::HardwareManager::GetInstance().SetMasterVolume(volume, fadeTimes);
    }

    static float GetMasterVolume()
    {
        return internal::driver::HardwareManager::GetInstance().GetMasterVolume();
    }

    static bool AppendEffect(AuxBus bus, nn::snd::CTR::FxDelay* fxDelay);
    static bool AppendEffect(AuxBus bus, nn::snd::CTR::FxReverb* fxReverb);
    static void ClearEffect(AuxBus bus, int fadeTimes = 0);

    static bool IsFinishedClearEffect(AuxBus bus)
    {
        return internal::driver::HardwareManager::GetInstance().IsFinishedClearEffect(bus);
    }

    static SrcType GetSrcType()
    { 
        return internal::driver::HardwareManager::GetInstance().GetSrcType();
    }

    static const int SOUND_THREAD_INTERVAL_USEC = NN_SND_USECS_PER_FRAME;

    static void EnableSeqPrintVar() { internal::driver::MmlParser::EnablePrintVar(true); }
    static void DisableSeqPrintVar() { internal::driver::MmlParser::EnablePrintVar(false); }

    static bool IsEnabledSeqPrintVar()
    {
        return internal::driver::MmlParser::IsEnabledPrintVar();
    }

    static int GetVoiceCount()
    {
        return internal::driver::VoiceManager::GetInstance().GetVoiceCount();
    }

    static int GetMaxVoiceCount(){ return s_MaxVoiceCount; }

    static void SetBiquadFilterCallback(int type, const BiquadFilterCallback* biquad)
    { 
        internal::driver::HardwareManager::GetInstance().SetBiquadFilterCallback(type, biquad);
    }

private:
    static int s_MaxVoiceCount;
    static internal::ThreadStack s_LoadThreadStack;
    static bool s_AutoCreateSoundThreadFlag;
    static bool s_IsInitialized;
    static bool s_IsStreamLoadWait;
    static bool s_IsEnterSleep;
    static bool s_IsInitializedDriverCommandManager;
};

} // namespace snd
} // namespace nw

#endif /* NW_SND_SOUND_SYSTEM_H_ */