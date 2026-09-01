#ifndef NW_SND_SOUND_THREAD_H_
#define NW_SND_SOUND_THREAD_H_

#include <nn/os.h>
#include <nw/os.h>
#include <nw/ut/ut_LinkList.h>
#include <nw/ut/ut_PreProcessor.h>
#include <nw/ut/ut_LinkList.h>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_Global.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

class SoundThread
{
public:
    class SoundFrameCallback
    {
      public:
        ut::LinkListNode m_Link;

        virtual ~SoundFrameCallback() {}
        virtual void OnBeginSoundFrame() {}
        virtual void OnEndSoundFrame() {}
    };

    class PlayerCallback
    {
    public:
        ut::LinkListNode m_Link;

        virtual ~PlayerCallback() {}
        virtual void OnUpdateFrameSoundThread() {}
        virtual void OnUpdateVoiceSoundThread() {}
        virtual void OnShutdownSoundThread() {}
    };
    static SoundThread& GetInstance();
    bool CreateSoundThread(uptr stackBufferAddress,size_t stackBufferSize,s32 priority,
        uptr userThreadStackBufferAddress,size_t userThreadStackBufferSize,s32 userThreadPriority,
        s32 coreNo,bool isEnableGetTick
    );
    void Destroy();
    bool IsCreated() const { return m_CreateFlag != 0; }

    void Initialize();
    void Finalize();
      
    void Pause(bool pauseFlag) { m_PauseFlag = pauseFlag; }
    void FrameProcess();
    
    void Lock() { m_CriticalSection.Enter(); }
    void Unlock() { m_CriticalSection.Leave(); }

    void SoundThreadProc();

private:
    typedef ut::LinkList< SoundFrameCallback, offsetof(SoundFrameCallback,m_Link)> SoundFrameCallbackList;
    typedef ut::LinkList< PlayerCallback, offsetof(PlayerCallback,m_Link)> PlayerCallbackList;

    nn::os::Tick            m_SoundThreadSumTick;
    int                     m_SoundThreadCount;
    int                     m_DspCycles;
    Util::PerfHistogram     m_PerfHistogram;
    bool                    m_IsBeginCalcPerf;

    typedef nn::os::InterCoreCriticalSection CriticalSection; // Why typedef this hal

    mutable CriticalSection m_CriticalSection;

    SoundFrameCallbackList  m_SoundFrameCallbackList;
    PlayerCallbackList      m_PlayerCallbackList;
    SoundFrameUserCallback  m_UserCallback;
    uptr                    m_UserCallbackArg;
    s32                     m_SoundThreadCoreNo;
    bool                    m_CreateFlag;
    bool                    m_PauseFlag;
    bool                    m_IsEnableGetTick;
    volatile bool           m_IsFinalizing;
};

class SoundThreadLock
{
public:
    SoundThreadLock()
    {
        SoundThread::GetInstance().Lock();
    }

    ~SoundThreadLock()
    {
        SoundThread::GetInstance().Unlock();
    }
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SoundThreadLock);
};
} // namespace driver 
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_THREAD_H_
