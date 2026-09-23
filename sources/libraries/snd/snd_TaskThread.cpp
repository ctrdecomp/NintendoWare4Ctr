// Filename: snd_TaskThread.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_TaskThread.h>
#include <nw/snd/snd_TaskManager.h>
#include <nw/snd/snd_DriverCommandManager.h>

namespace nw {
namespace snd {
namespace internal {

TaskThread::TaskThread(): 
    m_IsFinished(false),
    m_IsCreated(false)
{
}

TaskThread::~TaskThread()
{
    if (m_IsCreated)
    {
        Destroy();
    }
}

bool TaskThread::Create(s32 priority, ThreadStack& stack)
{
    if (m_IsCreated)
    {
        Destroy();
    }

    m_IsFinished = false;
    
    m_CriticalSection.Initialize();
    bool result = m_Thread.TryStart(ThreadFunc, reinterpret_cast<uptr>(this), stack, priority).IsSuccess();

    if (result == false)
    {
        return false;
    }

    m_IsCreated = true;
    
    return true;
}

void TaskThread::Destroy()
{
    if (!m_IsCreated)
    {
        return;
    }

    m_IsFinished = true;
    TaskManager::GetInstance().CancelWaitTask();

    m_Thread.Join();
    m_Thread.Finalize();
    m_CriticalSection.Finalize();

    m_IsCreated = false;
}

void TaskThread::ThreadFunc(uptr arg)
{
    TaskThread* taskThread = reinterpret_cast<TaskThread*>(arg);

    taskThread->ThreadProc();
}

void TaskThread::ThreadProc()
{
    while (!m_IsFinished)
    {
        TaskManager::GetInstance().WaitTask();
        if (m_IsFinished)
        {
            break;
        }
        
        {
            nn::os::CriticalSection::ScopedLock lock(m_CriticalSection);
            TaskManager::GetInstance().ExecuteTask();
        }

        DriverCommandManager::GetInstanceForTaskThread().RecvCommandReply();
    }
}

TaskThread& TaskThread::GetInstance()
{
    static TaskThread instance;
    return instance;
}

} // namespace nw::snd::internal
} // namespace nw::snd
} // namespace nw