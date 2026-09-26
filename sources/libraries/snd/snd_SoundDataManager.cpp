// Filename: snd_SoundDataManager.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundDataManager.h>

#include <nw/snd/snd_DisposeCallbackManager.h>
#include <nw/snd/snd_DriverCommandManager.h>
#include <nw/snd/snd_SoundArchiveLoader.h>
#include <nw/snd/snd_GroupFileReader.h>
#include <nw/snd/snd_WaveArchiveFileReader.h>
#include <nw/snd/snd_Util.h>
#include <nn/os.h>

namespace nw {
namespace snd {


SoundDataManager::SoundDataManager(): 
    m_pFileTable (NULL),
    m_pFileManager(NULL)
{
}

SoundDataManager::~SoundDataManager()
{
}

size_t SoundDataManager::GetRequiredMemSize(const SoundArchive* arc) const
{
    NW_NULL_ASSERT(arc);
    size_t size = 0;

    size += ut::RoundUp(
        sizeof(u32) + sizeof(FileAddress) * arc->detail_GetFileCount(), 4);
    return size;
}

bool SoundDataManager::Initialize(const SoundArchive* arc, void* buffer, u32 size)
{
    NW_NULL_ASSERT(arc);
    NW_NULL_ASSERT(buffer);
    NW_ALIGN4_ASSERT(buffer);
    NW_ASSERT(size >= GetRequiredMemSize(arc));

    void* endp = static_cast<char*>(buffer) + size;
    void* buf = buffer;

    if (!CreateFileAddressTable(arc, &buf, endp))
    {
        return false;
    }

    NW_ASSERT(static_cast<char*>(buf) - static_cast<char*>(buffer) == GetRequiredMemSize(arc));
    SetSoundArchive(arc);

    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    
    internal::DriverCommandDisposeCallback* command = cmdmgr.AllocCommand<internal::DriverCommandDisposeCallback>();
    command->id = internal::DRIVER_COMMAND_REGIST_DISPOSE_CALLBACK;
    command->callback = this;
    cmdmgr.PushCommand(command);
    
    return true;
}

void SoundDataManager::Finalize()
{
    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    
    internal::DriverCommandDisposeCallback* command = cmdmgr.AllocCommand<internal::DriverCommandDisposeCallback>();
    command->id = internal::DRIVER_COMMAND_UNREGIST_DISPOSE_CALLBACK;
    command->callback = this;
    cmdmgr.PushCommand(command);
    u32 tag = cmdmgr.FlushCommand(true);
    cmdmgr.WaitCommandReply(tag);
    
    m_pFileManager = NULL;
    m_pFileTable = NULL;
}

bool SoundDataManager::CreateFileAddressTable(const SoundArchive* arc, void** buffer, void* endp)
{
    size_t requiredSize =
        sizeof(u32) + sizeof(FileAddress) * arc->detail_GetFileCount();

    void* ep = ut::RoundUp(ut::AddOffsetToPtr(*buffer, requiredSize), 4);
    if (ut::ComparePtr(ep, endp) > 0)
    {
        return false;
    }
    m_pFileTable = reinterpret_cast<FileTable*>(*buffer);
    *buffer = ep;


    m_pFileTable->count = arc->detail_GetFileCount();
    for(u32 i = 0; i < m_pFileTable->count; i++)
    {
        m_pFileTable->item[i].address = NULL;
    }

    return true;
}

void SoundDataManager::InvalidateData(const void* start, const void* end)
{
    if (m_pFileTable != NULL)
    {
        for(u32 i = 0; i < m_pFileTable->count; i++)
        {
            const void* addr = m_pFileTable->item[i].address;
            if (start <= addr && addr <= end)
            {
                m_pFileTable->item[i].address = NULL;
            }
        }
    }

    if (ut::GetOffsetFromPtr(start, end) >= sizeof(IndividualWaveInfo))
    {
        if (start != NULL)
        {
            const IndividualWaveInfo* info = reinterpret_cast<const IndividualWaveInfo*>(start);
            if (info->signature == SoundDataManager::SIGNATURE_INDIVIDUAL_WAVE)
            {
                const void* pWarcTable = GetFileAddressFromTable(info->fileId);
                if (pWarcTable != NULL)
                {
                    internal::WaveArchiveFileReader reader(pWarcTable, true);
                    reader.SetWaveFile(info->waveIndex, NULL);
                }
            }
        }
    }    
}

const void* SoundDataManager::detail_GetFileAddress(SoundArchive::FileId fileId) const
{
    return GetFileAddressImpl(fileId);
}

const void* SoundDataManager::GetFileAddressImpl(SoundArchive::FileId fileId) const
{
    if (m_pFileManager != NULL)
    {
        const void* addr = m_pFileManager->GetFileAddress(fileId);
        if (addr != NULL) 
        {
            return addr;
        }
    }

    {
        const void* addr = GetFileAddressFromSoundArchive(fileId);
        if (addr != NULL) 
        {
            return addr;
        }
    }

    {
        const void* fileData = GetFileAddressFromTable( fileId );
        if (fileData != NULL) 
        {
            return fileData;
        }
    }
    return NULL;
}

const void* SoundDataManager::SetFileAddressToTable(
    SoundArchive::FileId fileId, const void* address)
{
    if (m_pFileTable == NULL)
    {
        NW_WARNING(m_pFileTable != NULL, "Failed to SoundDataManager::SetFileAddress because file table is not allocated.\n");
        return NULL;
    }

    NW_MINMAXLT_ASSERT(fileId, 0, m_pFileTable->count);

    const void* preAddress = m_pFileTable->item[fileId].address;
    m_pFileTable->item[fileId].address = address;
    return preAddress;
}

const void* SoundDataManager::GetFileAddressFromTable(SoundArchive::FileId fileId) const
{
    if (m_pFileTable == NULL)
    {
        NW_WARNING(m_pFileTable != NULL, "Failed to SoundDataManager::GetFileAddress because file table is not allocated.\n");
        return NULL;
    }

    if (fileId >= m_pFileTable->count) 
    {
        return NULL;
    }

    return m_pFileTable->item[fileId].address;
}

const void* SoundDataManager::SetFileAddress(SoundArchive::FileId fileId, const void* address)
{
    return SetFileAddressToTable(fileId, address);
}

u32 SoundDataManager::detail_GetFileIdFromTable(const void* address) const
{
    for (u32 i = 0; i < m_pFileTable->count; i++)
    {
        if (address == m_pFileTable->item[i].address)
        {
            return i;
        }
    }
    return SoundArchive::INVALID_ID;
}

} // namespace snd
} // namespace nw