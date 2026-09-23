#ifndef NW_SND_TASK_MANAGER_H_
#define NW_SND_TASK_MANAGER_H_

#include <nw/snd/snd_Task.h>
#include <nw/snd/snd_Util.h>
#include <nw/ut/ut_LinkList.h>
#include <nn/os.h>

namespace nw {
namespace snd {
namespace internal {

class TaskManager
{
public:
    enum TaskPriority
    {
        PRIORITY_LOW,
        PRIORITY_MIDDLE,
        PRIORITY_HIGH
    };
    
    static TaskManager& GetInstance();
    
    void Initialize();
    void Finalize();

    void ExecuteTask();
    void AppendTask(Task* task, TaskPriority priority);
    void CancelTask(Task* task);
    void CancelTaskById(u32 id);
    void CancelAllTask();

    void WaitTask();
    void CancelWaitTask();
    
private:
    static const u32 PRIORITY_NUM = PRIORITY_HIGH - PRIORITY_LOW + 1;
    static const u32 THREAD_MESSAGE_BUFSIZE = 32;
    
    enum Message
    {
        MESSAGE_APPEND
    };
    
    typedef ut::LinkList< Task, offsetof(Task,m_TaskLink)> TaskList;
    
    TaskManager();
    Task* PopTask();
    bool RemoveTask(Task* task);
    void RemoveTaskById(u32 id);
    
    Task* GetNextTask();
    Task* GetNextTask( TaskPriority priority, bool doRemove );
    
    TaskList m_TaskList[PRIORITY_NUM];
    Task* volatile m_pCurrentTask;
    volatile bool m_IsWaitTaskCancel;
    nn::os::CriticalSection m_CriticalSection;
    nn::os::BlockingQueue   m_BlockingQueue;
    uptr                    m_MsgBuffer[THREAD_MESSAGE_BUFSIZE];
};


} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_TASK_MANAGER_H_