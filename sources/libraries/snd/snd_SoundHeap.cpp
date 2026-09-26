// Filename: snd_SoundHeap.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundHeap.h>

#include <nw/snd/snd_SoundThread.h>
#include <nw/snd/snd_DriverCommandManager.h>

namespace nw {
namespace snd {

SoundHeap::SoundHeap(): 
    m_FrameHeap()
{
    m_CriticalSection.Initialize();
}

SoundHeap::~SoundHeap()
{
    Destroy();

    m_CriticalSection.Finalize();
}

bool SoundHeap::Create(void* startAddress, size_t size)
{
	nn::os::CriticalSection::ScopedLock lock(m_CriticalSection);

    return m_FrameHeap.Create(startAddress, size);
}

void SoundHeap::Destroy()
{
	nn::os::CriticalSection::ScopedLock lock(m_CriticalSection);

    if (!m_FrameHeap.IsValid()) 
    {
        return;
    }
    
    Clear();
    
    m_FrameHeap.Destroy();
}

void* SoundHeap::Alloc(size_t size)
{
	nn::os::CriticalSection::ScopedLock lock(m_CriticalSection);

    void* buffer = Alloc(size, DisposeCallbackFunc, NULL);
    return buffer;
}

void* SoundHeap::Alloc(size_t size, SoundHeap::DisposeCallback callback, void* callbackArg)
{
	nn::os::CriticalSection::ScopedLock lock(m_CriticalSection);

    return m_FrameHeap.Alloc(size, callback, callbackArg);
}

void SoundHeap::Clear()
{
    if (!m_FrameHeap.IsValid()) 
    {
        return;
    }
    
    {
        nn::os::CriticalSection::ScopedLock lock(m_CriticalSection);

        m_FrameHeap.Clear();
    }

    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    u32 tag = cmdmgr.FlushCommand(true);
    cmdmgr.WaitCommandReply(tag);    
}

int SoundHeap::SaveState()
{
	nn::os::CriticalSection::ScopedLock lock(m_CriticalSection);

    return m_FrameHeap.SaveState();
}

void SoundHeap::LoadState(int level)
{
    {
        nn::os::CriticalSection::ScopedLock lock(m_CriticalSection);

        m_FrameHeap.LoadState(level);

        internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
        u32 tag = cmdmgr.FlushCommand(true);
        cmdmgr.WaitCommandReply(tag);
    }
}

void SoundHeap::DisposeCallbackFunc(void* mem, unsigned long size, void* arg)
{
    NW_UNUSED_VARIABLE(arg);
    
    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();

    internal::DriverCommandInvalidateData* command = cmdmgr.AllocCommand<internal::DriverCommandInvalidateData>();
    command->id = internal::DRIVER_COMMAND_INVALIDATE_DATA;
    command->mem = mem;
    command->size = size;
    cmdmgr.PushCommand(command);
}

} // namespace snd
} // namespace nw