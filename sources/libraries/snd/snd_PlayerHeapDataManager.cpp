// Filename: snd_PlayerHeapDataManager.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_PlayerHeapDataManager.h>

namespace nw {
namespace snd {
namespace internal {

PlayerHeapDataManager::PlayerHeapDataManager(): 
    m_IsInitialized(false),
    m_IsFinalized (true)
{
}

PlayerHeapDataManager::~PlayerHeapDataManager()
{
    Finalize();
}

void PlayerHeapDataManager::Initialize(const SoundArchive* arc)
{
    if (m_IsInitialized)
    {
        return;
    }
    m_IsInitialized = true;
    m_IsFinalized = false;

    for (int i = 0; i < FILE_ADDRESS_COUNT; i++)
    {
        m_FileAddress[i].fileId = SoundArchive::INVALID_ID;
        m_FileAddress[i].address = NULL;
    }
    SetSoundArchive(arc);
}

void PlayerHeapDataManager::Finalize()
{
    if (m_IsFinalized)
    {
        return;
    }
    m_IsInitialized = false;
    m_IsFinalized = true;
    SetSoundArchive(NULL);
}

const void* PlayerHeapDataManager::SetFileAddress(SoundArchive::FileId fileId, const void* address)
{
    return SetFileAddressToTable(fileId, address);
}
const void* PlayerHeapDataManager::GetFileAddress(SoundArchive::FileId fileId) const
{
    return GetFileAddressFromTable(fileId);
}

void PlayerHeapDataManager::InvalidateData( const void* /*start*/, const void* /*end*/ )
{
    for (int i = 0; i < FILE_ADDRESS_COUNT; i++)
    {
        m_FileAddress[i].fileId = SoundArchive::INVALID_ID;
        m_FileAddress[i].address = NULL;
    }
}

const void* PlayerHeapDataManager::SetFileAddressToTable(
    SoundArchive::FileId fileId, const void* address)
{
    for (int i = 0; i< FILE_ADDRESS_COUNT; i++)
    {
        if (m_FileAddress[i].fileId == fileId)
        {
            const void* prev = m_FileAddress[i].address;
            m_FileAddress[i].address = address;
            return prev;
        }
    }

    for (int i = 0; i< FILE_ADDRESS_COUNT; i++)
    {
        if (m_FileAddress[i].fileId == SoundArchive::INVALID_ID)
        {
            m_FileAddress[i].fileId = fileId;
            m_FileAddress[i].address = address;
            return NULL;
        }
    }

    NW_ASSERTMSG(false, "invalid PlayerHeap");
    return NULL;
}

const void* PlayerHeapDataManager::GetFileAddressFromTable(SoundArchive::FileId fileId) const
{
    for (int i = 0; i< FILE_ADDRESS_COUNT; i++)
    {
        if (m_FileAddress[i].fileId == fileId)
        {
            return m_FileAddress[i].address;
        }
    }
    return NULL;
}

const void* PlayerHeapDataManager::GetFileAddressImpl(SoundArchive::FileId fileId) const
{
    return GetFileAddressFromTable(fileId);
}

} // namespace internal
} // namespace snd
} // namespace nw

