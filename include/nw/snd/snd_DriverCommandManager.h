#ifndef NW_SND_DRIVER_COMMAND_MANAGER_H_
#define NW_SND_DRIVER_COMMAND_MANAGER_H_

#include <nw/snd/snd_DriverCommand.h>
#include <nw/snd/snd_Config.h>
#include <nn/os.h>

namespace nw {
namespace snd {
namespace internal {

class DriverCommandManager
{
public:
    static DriverCommandManager& GetInstance();
    static DriverCommandManager& GetInstanceForTaskThread();

    ~DriverCommandManager();
    
    void Initialize(void* commandBuffer, u32 commandBufferSize);
    void Finalize();
    
    template<typename Command>
    Command* AllocCommand();
    u32 GetAllocatableCommandSize() const;
    
    u32 PushCommand(DriverCommand* command);
    void RecvCommandReply();
    
    u32 FlushCommand(bool forceFlag);
    void WaitCommandReply(u32 tag);

    bool IsFinishCommand(u32 tag) const;
    
    void ProcessCommand();
    
private:
    static const int COMMAND_QUEUE_SIZE = 32;
    
    DriverCommandManager();
    
    void* AllocMemory(u32 count);
    void* TryAllocMemory(u32 count);
    void FinalizeCommandList(DriverCommand* command);

    typedef nn::os::InterCoreBlockingQueue CommandQueue;
    
    CommandQueue   m_SendCommandQueue;
    uptr m_SendCommandQueueBuffer[COMMAND_QUEUE_SIZE];
    CommandQueue   m_RecvCommandQueue;
    uptr m_RecvCommandQueueBuffer[COMMAND_QUEUE_SIZE];
    
    DriverCommand* m_CommandListBegin;
    DriverCommand* m_CommandListEnd;
    
    u32 m_CommandTag;
    u32 m_FinishCommandTag;
    
    u32* m_CommandMemoryArea;
    u32 m_CommandMemoryAreaSize;
    
    u32 m_CommandMemoryAreaBegin;
    u32 m_CommandMemoryAreaEnd;

    bool m_CommandMemoryAreaZeroFlag;
    bool m_Available;
};

template< typename Command >
Command* DriverCommandManager::AllocCommand()
{
    u32 commandAreaCount = (sizeof(Command) + sizeof(m_CommandMemoryArea[0]) - 1) / sizeof(m_CommandMemoryArea[0]);
    
    Command* command = reinterpret_cast<Command*>(AllocMemory(commandAreaCount));
    command->memory_next = m_CommandMemoryAreaBegin;
    
    return command;
}


} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_DRIVER_COMMAND_MANAGER_H_