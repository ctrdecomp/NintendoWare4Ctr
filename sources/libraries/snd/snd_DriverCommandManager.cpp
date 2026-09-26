// Filename: snd_DriverCommandManager.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_DriverCommandManager.h>

namespace nw {
namespace snd {
namespace internal {

DriverCommandManager::DriverCommandManager():
    m_SendCommandQueue(),
    m_RecvCommandQueue(),
    m_Available(false)
{
}

DriverCommandManager::~DriverCommandManager()
{
}

DriverCommandManager& DriverCommandManager::GetInstance()
{
    DriverCommandManager instance;
    return instance;
}

DriverCommandManager& DriverCommandManager::GetInstanceForTaskThread()
{
    DriverCommandManager instance;
    return instance;
}

void DriverCommandManager::Initialize(void* commandBuffer, u32 commandBufferSize)
{
    m_CommandListBegin = NULL;
    m_CommandListEnd = NULL;
    m_CommandTag = 0;
    m_CommandMemoryAreaBegin = 0;
    m_CommandMemoryAreaEnd = 0;
    m_CommandMemoryAreaZeroFlag = false;
    m_CommandMemoryArea = reinterpret_cast<u32*>(commandBuffer);
    m_CommandMemoryAreaSize = commandBufferSize >> 2;
    m_SendCommandQueue.Initialize(m_SendCommandQueueBuffer, 32);
    m_RecvCommandQueue.Initialize(m_RecvCommandQueueBuffer, 33);
    m_Available = true;
}

void DriverCommandManager::Finalize()
{
    m_SendCommandQueue.Finalize();
    m_RecvCommandQueue.Finalize();
    m_Available = false;
}

u32 DriverCommandManager::PushCommand(DriverCommand* command)
{
    if(m_CommandListEnd != NULL)
    {
        m_CommandListEnd->next = command;
        m_CommandListEnd = command;
    }
    else
    {
        m_CommandListBegin = command;
        m_CommandListEnd = command;
    }
}

void DriverCommandManager::RecvCommandReply()
{
    uptr msg;
    while(m_RecvCommandQueue.TryDequeue(&msg))
    {
        FinalizeCommandList(reinterpret_cast<DriverCommand*>(msg));
    }
}

u32 DriverCommandManager::FlushCommand(bool forceFlag)
{
    if (m_CommandListBegin == NULL) 
    {
        if (!forceFlag) 
            return 0;

        DriverCommand* command = AllocCommand<DriverCommand>();
        command->id = DRIVER_COMMAND_DUMMY;
        PushCommand(command);
    }
    
    uptr msg = reinterpret_cast<uptr>(m_CommandListBegin);
    u32 tag = m_CommandTag;
    
    nn::os::ARM::DataMemoryBarrier();

    if (forceFlag)
    {
        m_SendCommandQueue.Enqueue(msg);
    }
    else
    {
        if (!m_SendCommandQueue.TryEnqueue(msg)) 
        {
            return 0;
        }
    }
    
    m_CommandListBegin->tag = tag;
    m_CommandTag++;
    
    m_CommandListBegin = NULL;
    m_CommandListEnd = NULL;
    
    return tag;
}

void DriverCommandManager::WaitCommandReply(u32 tag)
{
    for(;;)
    {
        uptr msg = m_RecvCommandQueue.Dequeue();
        DriverCommand* commandList = reinterpret_cast<DriverCommand*>(msg);
        FinalizeCommandList(commandList);
        
        if (tag == commandList->tag) break;
    }
}

void DriverCommandManager::ProcessCommand()
{
    uptr msg;
    while(m_SendCommandQueue.TryDequeue(&msg))
    {
        DriverCommand::ProcessCommandList(reinterpret_cast<DriverCommand*>(msg));
        nn::os::ARM::DataMemoryBarrier();
        m_RecvCommandQueue.Enqueue(msg);
    }
}

void* DriverCommandManager::AllocMemory(u32 count)
{
    NW_ASSERT(count <= m_CommandMemoryAreaSize);
    
    void* ptr = TryAllocMemory(count);
    if (ptr != NULL) 
        return ptr;

    RecvCommandReply();
    ptr = TryAllocMemory(count);
    if (ptr != NULL) 
        return ptr;

    while(ptr == NULL)
    {
        u32 tag = FlushCommand(true);
        WaitCommandReply(tag);
        ptr = TryAllocMemory(count);
    }
    
    return ptr;
}

void* DriverCommandManager::TryAllocMemory(u32 count)
{
    void* ptr = NULL;
    
    if (m_CommandMemoryAreaZeroFlag)
        return NULL;
    
    if (m_CommandMemoryAreaEnd > m_CommandMemoryAreaBegin) 
    {
        if (m_CommandMemoryAreaBegin + count <= m_CommandMemoryAreaEnd) 
        {
            ptr = &m_CommandMemoryArea[m_CommandMemoryAreaBegin];
            m_CommandMemoryAreaBegin += count;
        }
    }
    else 
    {
        if (m_CommandMemoryAreaBegin + count <= m_CommandMemoryAreaSize) 
        {
            ptr = &m_CommandMemoryArea[m_CommandMemoryAreaBegin];
            m_CommandMemoryAreaBegin += count;
        }
        else if (count <= m_CommandMemoryAreaEnd) 
        {
            ptr = &m_CommandMemoryArea[0];
            m_CommandMemoryAreaBegin = count;
        }
    }
    
    if (ptr != NULL && m_CommandMemoryAreaBegin == m_CommandMemoryAreaEnd) 
    {
        m_CommandMemoryAreaZeroFlag = true;
    }
    return ptr;
}

void DriverCommandManager::FinalizeCommandList(DriverCommand* commandList)
{
    m_FinishCommandTag = commandList->tag;
    
    DriverCommand* command = commandList;
    
    while(command != NULL)
    {
        if (command->next == NULL) 
        {
            m_CommandMemoryAreaEnd = command->memory_next;
            m_CommandMemoryAreaZeroFlag = false;
            if (m_CommandMemoryAreaBegin == m_CommandMemoryAreaEnd) 
            {
                m_CommandMemoryAreaBegin = m_CommandMemoryAreaEnd = 0;
            }
            break;
        }
        
        command = command->next;
    }
}

} // namespace internal
} // namespace snd
} // namespace nw