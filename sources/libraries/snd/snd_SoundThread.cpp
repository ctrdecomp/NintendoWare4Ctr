// Filename: snd_SoundThread.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundThread.h>
#include <nw/snd/snd_DriverCommandManager.h>
#include <nw/snd/snd_VoiceManager.h>
#include <nw/snd/snd_HardwareManager.h>
#include <nw/snd/snd_ChannelManager.h>
#include <nw/snd/snd_SoundSystem.h>
#include <nw/ut.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

SoundThread::SoundThread(): 
    m_SoundThreadSumTick(nn::os::Tick(0)),
    m_SoundThreadCount(0),
    m_DspCycles(0),
    m_IsBeginCalcPerf(false),
    m_SoundThreadCoreNo(0),
    m_CreateFlag(false),
    m_PauseFlag(false),
    m_IsEnableGetTick(false),
    m_IsFinalizing(false),
    m_pPerfData(NULL),
    m_NoteOnCount(0),
    m_PerfDataNum(0),
    m_PerfDataCount(0)
{
}

SoundThread& SoundThread::GetInstance() 
{
    static SoundThread instance;
    return instance;
}

bool SoundThread::PrepareForCreate(bool enableGetTick)
{
    NW_ASSERTMSG(HardwareManager::GetInstance().IsInitialized(), "note initialized nw::snd::internal::HardwareManager.\n");
    if (m_CreateFlag)
    {
        return true;
    }
    m_CreateFlag = true;
    m_IsFinalizing = false;
    m_IsEnableGetTick = enableGetTick;
    return true;
}

bool SoundThread::CreateSoundThread(
    uptr stackBufferAddress, size_t stackBufferSize, s32 priority,
    uptr userThreadStackBufferAddress, size_t userThreadStackBufferSize, s32 userThreadPriority,
    s32 coreNo, bool isEnableGetTick)
{
    bool prepareResult = PrepareForCreate(isEnableGetTick);
    if (prepareResult == false)
    {
        return false;
    }

    m_SoundThreadCoreNo = coreNo;

    nn::snd::CTR::ThreadParameter mainThread;
    mainThread.stackBuffer = stackBufferAddress;
    mainThread.stackSize = stackBufferSize;
    mainThread.priority = (coreNo == 1) ? 0 : priority;
    
    nn::snd::CTR::ThreadParameter userThread;
    userThread.stackBuffer = userThreadStackBufferAddress;
    userThread.stackSize = userThreadStackBufferSize;
    userThread.priority = userThreadPriority;
    
    nn::Result result = nn::snd::CTR::StartSoundThread(
        &mainThread,
        SoundThreadFunc,
        reinterpret_cast<uptr>(&GetInstance()),
        coreNo == 1 ? &userThread : NULL,
        UserThreadCallback,
        reinterpret_cast<uptr>(&GetInstance()),
        coreNo);
    nn::snd::CTR::EnableSoundThreadTickCounter(isEnableGetTick);
    return result.IsSuccess();
}

void SoundThread::Initialize()
{
    m_CriticalSection.Initialize();
}

void SoundThread::Destroy() {
    if (!m_CreateFlag) 
    {
        return;
    }

    m_IsFinalizing = true;

    if (m_SoundThreadCoreNo == 1)
    {
        nn::snd::CTR::FinalizeUserSoundThread();
    }
    nn::snd::CTR::FinalizeSoundThread();
}

void SoundThread::Finalize()
{
    Destroy();
    
    NW_UT_LINKLIST_FOREACH_SAFE(it, m_PlayerCallbackList,
        { it->OnShutdownSoundThread(); })

    m_CriticalSection.Finalize();
}

void SoundThread::SoundThreadFunc(uptr arg)
{
    SoundThread* th = reinterpret_cast<SoundThread*>(arg);

    th->SoundThreadProc();
}

void SoundThread::UserThreadCallback(uptr arg)
{
    SoundThread* th = reinterpret_cast<SoundThread*>(arg);

    if (th->m_UserCallback != NULL)
    {
        th->m_UserCallback(th->m_UserCallbackArg);
    }
}

void SoundThread::RegisterSoundFrameCallback(SoundFrameCallback* callback)
{
	CriticalSection::ScopedLock lock(m_CriticalSection);
    m_SoundFrameCallbackList.PushBack(callback);
}

void SoundThread::UnregisterSoundFrameCallback(SoundFrameCallback* callback)
{
    CriticalSection::ScopedLock lock(m_CriticalSection);
    m_SoundFrameCallbackList.Erase(callback);
}

void SoundThread::RegisterPlayerCallback(PlayerCallback* callback)
{
    m_PlayerCallbackList.PushBack(callback);
}

void SoundThread::UnregisterPlayerCallback(PlayerCallback* callback)
{
    m_PlayerCallbackList.Erase(callback);
}

void SoundThread::FrameProcess()
{
	CriticalSection::ScopedLock lock(m_CriticalSection);

    {
        NW_UT_LINKLIST_FOREACH_SAFE(it, m_SoundFrameCallbackList,
            { it->OnBeginSoundFrame(); })
    }

    {
        {
            VoiceUpdate();
        }
    
        HardwareManager::GetInstance().Update();

        {
            DriverCommandManager::GetInstance().ProcessCommand();
            DriverCommandManager::GetInstanceForTaskThread().ProcessCommand();
        }

        {
            NW_UT_LINKLIST_FOREACH_SAFE(it, m_PlayerCallbackList,
                { it->OnUpdateFrameSoundThread(); })
        }
        {
            ChannelManager::GetInstance().UpdateAllChannel();
        }

        (void)Util::CalcRandom();

        VoiceManager::GetInstance().UpdateAllVoices();
    }

    {
        NW_UT_LINKLIST_FOREACH_SAFE(it, m_SoundFrameCallbackList,
            { it->OnEndSoundFrame(); })
    }
}

void SoundThread::SoundThreadProc()
{
    while (m_IsFinalizing == false)
    {
        nn::os::Tick sum, wait, etc;
        nn::snd::CTR::WaitForDspSync(&wait);

        {
            Util::AutoStopWatch watch(etc);
            FrameProcess();
            nn::snd::CTR::SendParameterToDsp();
        }
        sum = wait + etc;

        CalcProcessCost(sum);
    }
}

void SoundThread::VoiceUpdate()
{
    NW_UT_LINKLIST_FOREACH_SAFE(it, m_PlayerCallbackList,
        { it->OnUpdateVoiceSoundThread(); })
}

void SoundThread::CalcProcessCost(const nn::os::Tick& tick)
{
    if ( m_IsEnableGetTick == true )
    {
        m_SoundThreadSumTick += tick;
        m_SoundThreadCount += 1;
        m_DspCycles += nn::snd::CTR::GetDspCycles();

        if (m_IsBeginCalcPerf == true)
        {
            f32 load = tick.ToTimeSpan().GetMicroSeconds() / SND_DENOM;
            m_PerfHistogram.SetLoad(load);

            if (m_pPerfData != NULL)
            {
                if (m_PerfDataCount < m_PerfDataNum)
                {
                    PerfData& data = m_pPerfData[m_PerfDataCount];
                    data.voice = SoundSystem::GetVoiceCount();
                    data.noteOn = m_NoteOnCount;
                    data.load = load;

                    m_PerfDataCount += 1;
                }
            }
            m_NoteOnCount = 0;
        }
    }
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw