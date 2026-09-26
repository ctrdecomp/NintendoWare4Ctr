// Filename: snd_PlayerHeap.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_PlayerHeap.h>
#include <nw/snd/snd_SoundThread.h>
#include <nw/snd/snd_DisposeCallbackManager.h>
#include <nw/snd/snd_DriverCommandManager.h>

namespace nw {
namespace snd {
namespace internal {

PlayerHeap::PlayerHeap(): 
    m_pSound(NULL),
    m_pPlayer(NULL),
    m_pStartAddress(NULL),
    m_pEndAddress(NULL),
    m_pAllocAddress(NULL)
{
}

PlayerHeap::~PlayerHeap()
{
    Destroy();
}

bool PlayerHeap::Create(void* startAddress, size_t size)
{
    void* endAddress = ut::AddOffsetToPtr(startAddress, size);
    startAddress = ut::RoundUp(startAddress, 32);
    if (startAddress > endAddress)
    {
        return false;
    }

    m_pStartAddress = startAddress;
    m_pEndAddress = endAddress;
    m_pAllocAddress = m_pStartAddress;

    return true;
}

void PlayerHeap::Destroy()
{
    Clear();
    m_pAllocAddress = NULL;
}

void* PlayerHeap::Alloc(size_t size)
{
    NW_ALIGN32_ASSERT(m_pAllocAddress);

    void* endp = ut::AddOffsetToPtr(m_pAllocAddress, size);
    if (endp > m_pEndAddress)
    {
        return NULL;
    }

    void* allocAddress = m_pAllocAddress;
    m_pAllocAddress = ut::RoundUp(endp, 32);
    return allocAddress;
}

void PlayerHeap::Clear()
{
    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    
    internal::DriverCommandInvalidateData* command = cmdmgr.AllocCommand<internal::DriverCommandInvalidateData>();
    command->id = internal::DRIVER_COMMAND_INVALIDATE_DATA;
    command->mem = m_pStartAddress;
    command->size = static_cast<unsigned long>(ut::GetOffsetFromPtr(m_pStartAddress, m_pAllocAddress));
    cmdmgr.PushCommand(command);
    
    m_pAllocAddress = m_pStartAddress;
}

size_t PlayerHeap::GetFreeSize() const
{
    size_t offset = ut::GetOffsetFromPtr(m_pAllocAddress, m_pEndAddress);
    NW_ASSERT(offset >= 0);
    return offset;
}

void PlayerHeap::AttachSound(BasicSound* sound)
{
    NW_NULL_ASSERT(sound);
    NW_ASSERT(m_pSound == NULL);

    m_pSound = sound;
}

void PlayerHeap::DetachSound(BasicSound* sound)
{
    NW_NULL_ASSERT(sound);
    NW_ASSERT(sound == m_pSound);
    NW_UNUSED_VARIABLE(sound);

    m_pSound = NULL;
}

} // namespace internal
} // namespace snd
} // namespace nw