// Filename: snd_TaskManager.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_TaskManager.h>

namespace nw {
namespace snd {
namespace internal {

TaskManager::TaskManager(): 
    m_pCurrentTask(NULL),
    m_IsWaitTaskCancel(false)
{
}

TaskManager& TaskManager::GetInstance()
{
    static TaskManager instance;
    return instance;
}

void TaskManager::Initialize()
{
    m_CriticalSection.Initialize();
    m_BlockingQueue.Initialize(m_MsgBuffer, THREAD_MESSAGE_BUFSIZE);
}

void TaskManager::Finalize()
{
    m_CriticalSection.Finalize();
    m_BlockingQueue.Finalize();
}

void TaskManager::AppendTask(Task* task, TaskPriority priority)
{
    NW_MINMAXLT_ASSERT(priority, 0, PRIORITY_NUM);
    
    nn::os::CriticalSection::ScopedLock scopeLock(m_CriticalSection);
    
    task->m_Event.ClearSignal();
    task->m_Status = Task::STATUS_APPEND;
    m_TaskList[priority].PushBack(task);
    m_BlockingQueue.TryEnqueue(static_cast<uptr>(MESSAGE_APPEND));
}

Task* TaskManager::GetNextTask(TaskPriority priority, bool doRemove)
{
    NW_MINMAXLT_ASSERT(priority, 0, PRIORITY_NUM);
    
    nn::os::CriticalSection::ScopedLock scopeLock(m_CriticalSection);
    
    if (m_TaskList[priority].IsEmpty())
    {
        return NULL;
    }

    Task* task = &m_TaskList[priority].GetFront();
    if (doRemove) 
    {
        m_TaskList[priority].PopFront();
    }
    return task;
}

Task* TaskManager::PopTask()
{
    nn::os::CriticalSection::ScopedLock scopeLock(m_CriticalSection);
    
    Task* task = GetNextTask(PRIORITY_HIGH, true);
    if (task != NULL)
    {
        return task;
    }
    
    task = GetNextTask(PRIORITY_MIDDLE, true);
    if (task != NULL)
    {
        return task;
    }
    
    task = GetNextTask(PRIORITY_LOW, true);
    if (task != NULL)
    {
        return task;
    }
    
    return NULL;
}

Task* TaskManager::GetNextTask()
{
    Task* task = GetNextTask(PRIORITY_HIGH, false);
    if (task != NULL) 
    {
        return task;
    }

    task = GetNextTask(PRIORITY_MIDDLE, false);
    if (task != NULL) 
    {
        return task;
    }
    
    task = GetNextTask(PRIORITY_LOW, false);
    if (task != NULL)
    {
        return task;
    }
    
    return NULL;
}

void TaskManager::ExecuteTask()
{
    for(;;)
    {
        Task* task = PopTask();
        if (task == NULL) break;

        m_pCurrentTask = task;
        
        task->Execute();
        task->m_Status = Task::STATUS_DONE; 
        task->m_Event.Signal();
        m_pCurrentTask = NULL;
    }
}

void TaskManager::CancelTask(Task* task)
{
    if (RemoveTask(task)) 
    {
        return;
    }
    
    task->m_Event.Wait();
}

void TaskManager::CancelTaskById(u32 id)
{
    RemoveTaskById(id);
}

void TaskManager::CancelAllTask()
{
}

bool TaskManager::RemoveTask(Task* task)
{
    nn::os::CriticalSection::ScopedLock scopeLock(m_CriticalSection);
    
    for (int i = 0; i < PRIORITY_NUM; i++)
    {
        TaskPriority priority = static_cast<TaskPriority>(i);
        TaskList::Iterator itr = m_TaskList[priority].GetBeginIter();
        while (itr != m_TaskList[priority].GetEndIter())
        {
            TaskList::Iterator curItr = itr++;
            if (&*curItr == task) 
            {
                m_TaskList[priority].Erase(curItr);
                curItr->m_Status = Task::STATUS_CANCEL;
                curItr->m_Event.Signal();
                return true;
            }
        }
    }

    return false;
}

void TaskManager::RemoveTaskById(u32 id)
{
    nn::os::CriticalSection::ScopedLock scopeLock(m_CriticalSection);
    
    for (int i = 0; i < PRIORITY_NUM; i++)
    {
        TaskPriority priority = static_cast<TaskPriority>(i);
        TaskList::Iterator itr = m_TaskList[priority].GetBeginIter();
        while (itr != m_TaskList[priority].GetEndIter())
        {
            TaskList::Iterator curItr = itr++;
            if (curItr->m_Id == id) 
            {
                m_TaskList[priority].Erase(curItr);
                curItr->m_Status = Task::STATUS_CANCEL;
                curItr->m_Event.Signal();
            }
        }
    }
}

void TaskManager::WaitTask()
{
    m_IsWaitTaskCancel = false;

    while(GetNextTask() == NULL && ! m_IsWaitTaskCancel)
    {
        if (m_BlockingQueue.Dequeue() == MESSAGE_APPEND)
        {
            break;
        }
    }
}

void TaskManager::CancelWaitTask()
{
    m_IsWaitTaskCancel = true;

    m_BlockingQueue.Enqueue(static_cast<uptr>(MESSAGE_APPEND));
}

} // namespace internal
} // namespace snd
} // namespace nw