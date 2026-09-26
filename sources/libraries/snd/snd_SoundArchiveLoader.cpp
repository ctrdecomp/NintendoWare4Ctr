// Filename: snd_SoundArchiveLoader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundArchiveLoader.h>
#include <nw/snd/snd_SoundMemoryAllocatable.h>
#include <nw/snd/snd_BankFileReader.h>
#include <nw/snd/snd_GroupFileReader.h>
#include <nw/snd/snd_WaveSoundFileReader.h>
#include <nw/snd/snd_WaveArchiveFileReader.h>

#include <nw/io/io_FileStream.h>

namespace nw {
namespace snd {
namespace internal {

namespace
{

const u32 REQUIRED_SIZE_FOR_LOAD_WARC_FILE_HEADER =
    NW_ROUND_UP_32B(sizeof(internal::WaveArchiveFile::FileHeader) + 32);

class FileStreamHandle
{
public:
    FileStreamHandle(io::FileStream* stream): 
        m_pStream(stream) 
    {
    }
    ~FileStreamHandle()
    {
        if(m_pStream != NULL)
        {
            m_pStream->Close();
        }
    }

    io::FileStream* GetFileStream()
    {
        return m_pStream;
    }
    io::FileStream* operator->()
    {
        return m_pStream;
    }
    operator bool() const
    {
        return m_pStream != NULL;
    }

private:
    io::FileStream* m_pStream;
};

}

SoundArchiveLoader::SoundArchiveLoader(): 
    m_pSoundArchive(NULL),
    m_LoadDataCallCount(0),
    m_IsCancelLoading(false)
{
}

SoundArchiveLoader::~SoundArchiveLoader()
{
    m_pSoundArchive = NULL;
}

void SoundArchiveLoader::SetSoundArchive(const SoundArchive* arc)
{
    m_pSoundArchive = arc;
}

bool SoundArchiveLoader::CancelLoading()
{
    if(m_LoadDataCallCount > 0)
    {
        m_IsCancelLoading = true;
        return true;
    }
    return false;
}

bool SoundArchiveLoader::IsAvailable() const
{
    if(m_pSoundArchive == NULL)
    {
        return false;
    }
    if(!m_pSoundArchive->IsAvailable())
    {
        return false;
    }

    return true;
}

bool SoundArchiveLoader::LoadData(SoundArchive::ItemId itemId, SoundMemoryAllocatable* pAllocator, 
    u32 loadFlag, size_t loadBlockSize)
{
    NW_NULL_ASSERT(m_pSoundArchive);
    NW_ALIGN32_ASSERT(loadBlockSize);

    if(!IsAvailable())
    {
        return false;
    }
    if(itemId == SoundArchive::INVALID_ID)
    {
        return false;
    }
    if(pAllocator == NULL)
    {
        return false;
    }

    m_LoadDataCallCount++;

    bool result = false;
    if(m_IsCancelLoading)
    {
        result = false;
    }
    else
    {
        switch(internal::Util::GetItemType(itemId))
        {
        case internal::ItemType_Sound:
            switch(m_pSoundArchive->GetSoundType(itemId))
            {
            case SoundArchive::SOUND_TYPE_SEQ:
                result = LoadSequenceSound(itemId, pAllocator, loadFlag, loadBlockSize);
                break;
            case SoundArchive::SOUND_TYPE_WAVE:
                result = LoadWaveSound(itemId, pAllocator, loadFlag, loadBlockSize);
                break;
            case SoundArchive::SOUND_TYPE_STRM:
            default:
                result = false;
                break;
            }
            break;
        case internal::ItemType_Bank:
            result = LoadBank(itemId, pAllocator, loadFlag, loadBlockSize);
            break;
        case internal::ItemType_WaveArchive:
            result = LoadWaveArchive(itemId, pAllocator, loadFlag, loadBlockSize);
            break;
        case internal::ItemType_Group:
            result = LoadGroup(itemId, pAllocator, loadBlockSize);
            break;
        case internal::ItemType_SoundGroup:
            result = LoadSoundGroup(itemId, pAllocator, loadFlag, loadBlockSize);
            break;
        case internal::ItemType_Player:
        default:
            result = false;
            break;
        }
    }

    m_LoadDataCallCount--;
    if(m_LoadDataCallCount == 0)
    {
        m_IsCancelLoading = false;
    }
    return result;
}

bool SoundArchiveLoader::LoadData(const char* pItemName, SoundMemoryAllocatable* pAllocator, 
    u32 loadFlag, size_t loadBlockSize)
{
    NW_NULL_ASSERT(m_pSoundArchive);

    SoundArchive::ItemId id = m_pSoundArchive->GetItemId(pItemName);
    return LoadData(id, pAllocator, loadFlag, loadBlockSize);
}

const void* SoundArchiveLoader::LoadImpl(SoundArchive::FileId fileId, SoundMemoryAllocatable* pAllocator, 
    size_t loadBlockSize, bool needDeviceMemory)
{
    NW_NULL_ASSERT(m_pSoundArchive);
    NW_UNUSED_VARIABLE(loadBlockSize);

    const void* fileAddress = GetFileAddressImpl(fileId);
    if(fileAddress == NULL)
    {
        fileAddress = LoadFile(fileId, pAllocator, loadBlockSize, needDeviceMemory);
        if(fileAddress == NULL)
        {
            return NULL;
        }
        else
        {
            SetFileAddressToTable(fileId, fileAddress);
            return fileAddress;
        }
    }
    else
    {
        return fileAddress;
    }
}

bool SoundArchiveLoader::LoadSequenceSound(SoundArchive::ItemId soundId, SoundMemoryAllocatable* pAllocator, 
    u32 loadFlag, size_t loadBlockSize)
{
    if(loadFlag & LOAD_SEQ)
    {
        u32 fileId = m_pSoundArchive->GetItemFileId(soundId);
        const void* pFile = LoadImpl(fileId, pAllocator, loadBlockSize);
        if(pFile == NULL)
        {
            return false;
        }
    }

    if(loadFlag & LOAD_BANK || loadFlag & LOAD_WARC)
    {
        SoundArchive::SequenceSoundInfo info;
        if(!m_pSoundArchive->ReadSequenceSoundInfo(soundId, &info))
        {
            return false;
        }

        for(int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
        {
            u32 bankId = info.bankIds[i];
            if(bankId != SoundArchive::INVALID_ID)
            {
                if(!LoadBank(bankId, pAllocator, loadFlag, loadBlockSize))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool SoundArchiveLoader::LoadWaveSound(SoundArchive::ItemId soundId, SoundMemoryAllocatable* pAllocator, 
    u32 loadFlag, size_t loadBlockSize, SoundArchive::ItemId waveSoundSetId)
{
    u32 wsdFileId = m_pSoundArchive->GetItemFileId(soundId);
    if(loadFlag & LOAD_WSD)
    {
        const void* pFile = LoadImpl(wsdFileId, pAllocator, loadBlockSize);
        if(pFile == NULL)
        {
            return false;
        }
    }

    if(loadFlag & LOAD_WARC)
    {
        const void* pWsdFile = GetFileAddressImpl(wsdFileId);

        if(pWsdFile != NULL)
        {
            u32 index;
            {
                SoundArchive::WaveSoundInfo info;
                if(!m_pSoundArchive->detail_ReadWaveSoundInfo(soundId, &info))
                {
                    return false;
                }
                index = info.index;
            }
            u32 warcId = SoundArchive::INVALID_ID;
            u32 waveIndex;
            {
                internal::WaveSoundFileReader reader(pWsdFile);
                internal::WaveSoundNoteInfo info;
                if(!reader.ReadNoteInfo(&info, index, 0))
                {
                    return false;
                }
                warcId = info.waveArchiveId;
                waveIndex = info.waveIndex;
            }
            if(!LoadWaveArchiveImpl(warcId, waveIndex, pAllocator, loadFlag, loadBlockSize))
            {
                return false;
            }
        }
        else
        {
            SoundArchive::ItemId itemId;
            if(waveSoundSetId != SoundArchive::INVALID_ID)
            {
                itemId = waveSoundSetId;
            }
            else
            {
                itemId = soundId;
            }
            const internal::Util::Table<u32>* pWarcIdTable = 
                m_pSoundArchive->detail_GetWaveArchiveIdTable(itemId);

            NW_NULL_ASSERT(pWarcIdTable);

            for(u32 i = 0; i < pWarcIdTable->count; i++)
            {
                if(!LoadWaveArchive(pWarcIdTable->item[i], pAllocator, loadFlag, loadBlockSize))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool SoundArchiveLoader::LoadBank(SoundArchive::ItemId bankId, SoundMemoryAllocatable* pAllocator, 
    u32 loadFlag, size_t loadBlockSize)
{
    u32 bankFileId = m_pSoundArchive->GetItemFileId(bankId);
    if(loadFlag & LOAD_BANK)
    {
        const void* pFile = LoadImpl(bankFileId, pAllocator, loadBlockSize);
        if(pFile == NULL)
        {
            return false;
        }
    }

    if(loadFlag & LOAD_WARC)
    {
        const void* pFile = GetFileAddressImpl(bankFileId);

        if(pFile != NULL)
        {
            internal::BankFileReader reader(pFile);
            const internal::Util::WaveIdTable& table = reader.GetWaveIdTable();

            for(u32 i = 0; i < table.GetCount(); i++)
            {
                const internal::Util::WaveId* pWaveId = table.GetWaveId(i);

                if(pWaveId == NULL)
                {
                    return false;
                }
                if(!LoadWaveArchiveImpl(pWaveId->waveArchiveId, pWaveId->waveIndex, pAllocator, 
                    loadFlag, loadBlockSize))
                {
                    return false;
                }
            }
        }
        else
        {
            const internal::Util::Table<ut::ResU32>* pWarcIdTable = 
                m_pSoundArchive->detail_GetWaveArchiveIdTable(bankId);

            NW_NULL_ASSERT(pWarcIdTable);

            for(u32 i = 0; i < pWarcIdTable->count; i++)
            {
                if(!LoadWaveArchive(pWarcIdTable->item[i], pAllocator, loadFlag, loadBlockSize))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool SoundArchiveLoader::LoadWaveArchiveImpl(SoundArchive::ItemId warcId, u32 waveIndex, 
    SoundMemoryAllocatable* pAllocator, u32 loadFlag, size_t loadBlockSize)
{
    SoundArchive::WaveArchiveInfo info;
    if(!m_pSoundArchive->ReadWaveArchiveInfo(warcId, &info))
    {
        return false;
    }
    if(info.isLoadIndividual)
    {
        if(!LoadIndividualWave(warcId, waveIndex, pAllocator, loadBlockSize))
        {
            return false;
        }
    }
    else
    {
        if(!LoadWaveArchive(warcId, pAllocator, loadFlag, loadBlockSize))
        {
            return false;
        }
    }
    return true;
}

bool SoundArchiveLoader::LoadWaveArchive(SoundArchive::ItemId warcId, SoundMemoryAllocatable* pAllocator, 
    u32 loadFlag, size_t loadBlockSize)
{
    if(loadFlag & LOAD_WARC)
    {
        u32 fileId = m_pSoundArchive->GetItemFileId(warcId);
        const void* pFile = LoadImpl(fileId, pAllocator, loadBlockSize, true);
        if(pFile == NULL)
        {
            return false;
        }
    }
    return true;
}

const void* SoundArchiveLoader::LoadWaveArchiveTable(SoundArchive::ItemId warcId, 
    SoundMemoryAllocatable* pAllocator, size_t loadBlockSize)
{
    u32 fileId = m_pSoundArchive->GetItemFileId(warcId);
    const void* pWaveArchiveFile = GetFileAddressFromTable(fileId);
    if(pWaveArchiveFile != NULL)
    {
        return pWaveArchiveFile;
    }

    u32 waveCount;
    {
        SoundArchive::WaveArchiveInfo info;
        if(!m_pSoundArchive->ReadWaveArchiveInfo(warcId, &info))
        {
            return NULL;
        }
        if(info.waveCount <= 0)
        {
            return NULL;
        }
        waveCount = info.waveCount;
    }

    u32 fileBlockOffset;
    {
        u8 pBuffer[REQUIRED_SIZE_FOR_LOAD_WARC_FILE_HEADER];
        u8* pAlignedBuffer = reinterpret_cast<u8*>(ut::RoundUp(pBuffer, 32));
        s32 readSize = ReadFile(fileId, pAlignedBuffer, sizeof(internal::WaveArchiveFile::FileHeader), 
            0, loadBlockSize);
        if(readSize != sizeof(internal::WaveArchiveFile::FileHeader))
        {
            return NULL;
        }
        const internal::WaveArchiveFile::FileHeader* pHeader = 
            reinterpret_cast<const internal::WaveArchiveFile::FileHeader*>(pAlignedBuffer);
        fileBlockOffset = pHeader->GetFileBlockOffset();
        u32 infoBlockOffset = pHeader->GetInfoBlockOffset();
        if(infoBlockOffset > fileBlockOffset)
        {
            return NULL;
        }
    }

    const u32 REQUIRED_SIZE = fileBlockOffset + waveCount * sizeof(u32) + 
        sizeof(internal::WaveArchiveFileReader::SIGNATURE_WARC_TABLE);

    void* buffer = pAllocator->Alloc(REQUIRED_SIZE);
    if(!internal::Util::IsDeviceMemory(reinterpret_cast<uptr>(buffer), REQUIRED_SIZE))
    {
        NW_ASSERTMSG(false, "buffer, buffer + REQUIRED_SIZE is not Device Memory.");
        return NULL;
    }
    {
        s32 readSize = ReadFile(fileId, buffer, fileBlockOffset, 0, loadBlockSize);
        if(readSize != fileBlockOffset)
        {
            return NULL;
        }
    }
    internal::WaveArchiveFileReader reader(buffer, true);

    std::memcpy(ut::AddOffsetToPtr(buffer, fileBlockOffset), 
        &internal::WaveArchiveFileReader::SIGNATURE_WARC_TABLE, 
        sizeof(internal::WaveArchiveFileReader::SIGNATURE_WARC_TABLE));
    reader.InitializeFileTable();

    SetFileAddressToTable(fileId, buffer);

    return buffer;
}

bool SoundArchiveLoader::LoadIndividualWave(SoundArchive::ItemId warcId, u32 waveIndex, 
    SoundMemoryAllocatable* pAllocator, size_t loadBlockSize)
{
    NW_UNUSED_VARIABLE(loadBlockSize);

    u32 fileId = m_pSoundArchive->GetItemFileId(warcId);
    const void* pWaveArchiveFile = GetFileAddressFromTable(fileId);
    if(pWaveArchiveFile == NULL)
    {
        pWaveArchiveFile = LoadWaveArchiveTable(warcId, pAllocator, loadBlockSize);
        if(pWaveArchiveFile == NULL)
        {
            return false;
        }
    }

    internal::WaveArchiveFileReader reader(pWaveArchiveFile, true);

    if(reader.IsLoaded(waveIndex))
    {
        return true;
    }

    const size_t WAVE_FILE_SIZE = reader.GetWaveFileSize(waveIndex);
    const size_t REQUIRED_SIZE = WAVE_FILE_SIZE + sizeof(IndividualWaveInfo);
    void* buffer = pAllocator->Alloc(REQUIRED_SIZE);
    if(!internal::Util::IsDeviceMemory(reinterpret_cast<uptr>(buffer), REQUIRED_SIZE))
    {
        NW_ASSERTMSG(false, "buffer, buffer + REQUIRED_SIZE is not Device Memory.");
        return false;
    }

    {
        IndividualWaveInfo iWavInfo(fileId, waveIndex);
        std::memcpy(buffer, &iWavInfo, sizeof(IndividualWaveInfo));
    }

    {
        void* loadingAddress = ut::AddOffsetToPtr(buffer, sizeof(IndividualWaveInfo));
        s32 readSize = ReadFile(fileId, loadingAddress, WAVE_FILE_SIZE, 
            reader.GetWaveFileOffsetFromFileHead(waveIndex), loadBlockSize);
        if(readSize != WAVE_FILE_SIZE)
        {
            return false;
        }
        reader.SetWaveFile(waveIndex, loadingAddress);

        nn::snd::CTR::FlushDataCache(reinterpret_cast<uptr>(loadingAddress), WAVE_FILE_SIZE);
    }
    return true;
}

bool SoundArchiveLoader::PostProcessForLoadedGroupFile(const void* pGroupFile, 
    SoundMemoryAllocatable* pAllocator, size_t loadBlockSize)
{
    internal::GroupFileReader reader(pGroupFile);
    bool isLinkGroup = false;
    u32 groupItemCount = reader.GetGroupItemCount();
    for(u32 i = 0; i < groupItemCount; i++)
    {
        internal::GroupItemLocationInfo info;
        if(!reader.ReadGroupItemLocationInfo(&info, i))
        {
            return false;
        }

        if(info.address != NULL)
        {
            SetFileAddressToTable(info.fileId, info.address);
        }
        else
        {
            if(i == 0)
            {
                isLinkGroup = true;
                break;
            }
        }
    }
    if(isLinkGroup)
    {
        u32 groupItemInfoExCount = reader.GetGroupItemExCount();
        for(u32 i = 0; i < groupItemInfoExCount; i++)
        {
            GroupFile::GroupItemInfoEx infoEx;
            if(reader.ReadGroupItemInfoEx(&infoEx, i))
            {
                if(!LoadData(infoEx.itemId, pAllocator, infoEx.loadFlag, loadBlockSize))
                {
                    return false;
                }
            }
        }
    }
    else
    {
        u32 groupItemInfoExCount = reader.GetGroupItemExCount();
        for(u32 i = 0; i < groupItemInfoExCount; i++)
        {
            GroupFile::GroupItemInfoEx infoEx;
            if(reader.ReadGroupItemInfoEx(&infoEx, i))
            {
                if(!(infoEx.loadFlag & LOAD_WARC))
                {
                    continue;
                }

                switch(Util::GetItemType(infoEx.itemId))
                {
                case ItemType_Sound:
                    switch(m_pSoundArchive->GetSoundType(infoEx.itemId))
                    {
                        case SoundArchive::SOUND_TYPE_SEQ:
                            SetWaveArchiveTableWithSeqInEmbeddedGroup(infoEx.itemId, pAllocator);
                            break;
                    }
                    break;
                case ItemType_Bank:
                    SetWaveArchiveTableWithBankInEmbeddedGroup(infoEx.itemId, pAllocator);
                    break;
                case ItemType_SoundGroup:
                    {
                        SoundArchive::SoundGroupInfo info;
                        if(!m_pSoundArchive->detail_ReadSoundGroupInfo(infoEx.itemId, &info))
                        {
                            continue;
                        }
                        if(info.startId == SoundArchive::INVALID_ID)
                        {
                            continue;
                        }
                        switch(m_pSoundArchive->GetSoundType(info.startId))
                        {
                            case SoundArchive::SOUND_TYPE_SEQ:
                                for(u32 id = info.startId; id <= info.endId; id++)
                                {
                                    SetWaveArchiveTableWithSeqInEmbeddedGroup(id, pAllocator);
                                }
                                break;
                            case SoundArchive::SOUND_TYPE_WAVE:
                                SetWaveArchiveTableWithWsdInEmbeddedGroup(info.startId, pAllocator);
                                break;
                        }
                    }
                    break;
                }
            }
        }
    }

    return true;
}

void SoundArchiveLoader::SetWaveArchiveTableWithSeqInEmbeddedGroup(SoundArchive::ItemId seqId, 
    SoundMemoryAllocatable* pAllocator)
{
    SoundArchive::SequenceSoundInfo info;
    if(!m_pSoundArchive->ReadSequenceSoundInfo(seqId, &info))
    {
        return;
    }
    for(int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
    {
        SetWaveArchiveTableWithBankInEmbeddedGroup(info.bankIds[i], pAllocator);
    }
}

void SoundArchiveLoader::SetWaveArchiveTableWithBankInEmbeddedGroup(SoundArchive::ItemId bankId, 
    SoundMemoryAllocatable* pAllocator)
{
    if(bankId == SoundArchive::INVALID_ID)
    {
        return;
    }

    SoundArchive::BankInfo bankInfo;
    if(!m_pSoundArchive->ReadBankInfo(bankId, &bankInfo))
    {
        return;
    }
    const void* bankFile = GetFileAddressFromTable(bankInfo.fileId);
    if(bankFile == NULL)
    {
        return;
    }

    BankFileReader bankReader(bankFile);
    const Util::WaveIdTable& table = bankReader.GetWaveIdTable();
    if(table.GetCount() <= 0)
    {
        return;
    }

    const Util::WaveId* pWaveId = table.GetWaveId(0U);
    if(pWaveId == NULL)
    {
        return;
    }
    SoundArchive::ItemId warcId = pWaveId->waveArchiveId;
    SetWaveArchiveTableInEmbeddedGroupImpl(warcId, pAllocator);
}

void SoundArchiveLoader::SetWaveArchiveTableWithWsdInEmbeddedGroup(SoundArchive::ItemId wsdId, 
    SoundMemoryAllocatable* pAllocator)
{
    if(wsdId == SoundArchive::INVALID_ID)
    {
        return;
    }

    SoundArchive::SoundInfo soundInfo;
    if(!m_pSoundArchive->ReadSoundInfo(wsdId, &soundInfo))
    {
        return;
    }
    SoundArchive::WaveSoundInfo wsdInfo;
    if(!m_pSoundArchive->detail_ReadWaveSoundInfo(wsdId, &wsdInfo))
    {
        return;
    }

    const void* wsdFile = GetFileAddressFromTable(soundInfo.fileId);
    if(wsdFile == NULL)
    {
        return;
    }

    WaveSoundFileReader reader(wsdFile);
    WaveSoundNoteInfo noteInfo;
    if(!reader.ReadNoteInfo(&noteInfo, wsdInfo.index, 0))
    {
        return;
    }

    SetWaveArchiveTableInEmbeddedGroupImpl(noteInfo.waveArchiveId, pAllocator);
}

void SoundArchiveLoader::SetWaveArchiveTableInEmbeddedGroupImpl(SoundArchive::ItemId warcId, 
    SoundMemoryAllocatable* pAllocator)
{
    SoundArchive::WaveArchiveInfo info;
    if(!m_pSoundArchive->ReadWaveArchiveInfo(warcId, &info))
    {
        return;
    }
    if(!info.isLoadIndividual)
    {
        return;
    }

    const void* warcFile = GetFileAddressFromTable(info.fileId);
    WaveArchiveFileReader loadedFileReader(warcFile, false);

    if(loadedFileReader.HasIndividualLoadTable())
    {
        return;
    }

    u32 fileBlockOffset = 
        reinterpret_cast<const WaveArchiveFile::FileHeader*>(warcFile)->GetFileBlockOffset();
    const u32 REQUIRED_TABLE_SIZE = fileBlockOffset + info.waveCount * sizeof(u32) + 
        sizeof(WaveArchiveFileReader::SIGNATURE_WARC_TABLE);

    void* buffer = pAllocator->Alloc(REQUIRED_TABLE_SIZE);
    if(buffer == NULL)
    {
        return;
    }

    std::memcpy(buffer, warcFile, fileBlockOffset);

    std::memcpy(ut::AddOffsetToPtr(buffer, fileBlockOffset), 
        &WaveArchiveFileReader::SIGNATURE_WARC_TABLE, 
        sizeof(WaveArchiveFileReader::SIGNATURE_WARC_TABLE));

    WaveArchiveFileReader reader(buffer, true);
    reader.InitializeFileTable();

    for(u32 i = 0; i < info.waveCount; i++)
    {
        reader.SetWaveFile(i, loadedFileReader.GetWaveFile(i));
    }

    SetFileAddressToTable(info.fileId, buffer);
}

bool SoundArchiveLoader::LoadGroup(SoundArchive::ItemId groupId, SoundMemoryAllocatable* pAllocator, 
    size_t loadBlockSize)
{
    const void* pGroupFile = NULL;
    {
        u32 fileId = m_pSoundArchive->GetItemFileId(groupId);

        pGroupFile = LoadImpl(fileId, pAllocator, loadBlockSize, true);
        if(pGroupFile == NULL)
        {
            return false;
        }
    }
    return PostProcessForLoadedGroupFile(pGroupFile, pAllocator);
}

bool SoundArchiveLoader::LoadSoundGroup(SoundArchive::ItemId soundGroupId, SoundMemoryAllocatable* pAllocator, 
    u32 loadFlag, size_t loadBlockSize)
{
    SoundArchive::SoundGroupInfo info;
    if(!m_pSoundArchive->detail_ReadSoundGroupInfo(soundGroupId, &info))
    {
        return false;
    }
    if(info.startId == SoundArchive::INVALID_ID)
    {
        return true;
    }

    switch(m_pSoundArchive->GetSoundType(info.startId))
    {
    case SoundArchive::SOUND_TYPE_SEQ:
        for(u32 id = info.startId; id <= info.endId; id++)
        {
            if(!LoadSequenceSound(id, pAllocator, loadFlag, loadBlockSize))
            {
                return false;
            }
        }
        break;
    case SoundArchive::SOUND_TYPE_WAVE:
        for(u32 id = info.startId; id <= info.endId; id++)
        {
            if(!LoadWaveSound(id, pAllocator, loadFlag, loadBlockSize, soundGroupId))
            {
                return false;
            }
        }
        break;
    }

    return true;
}

s32 SoundArchiveLoader::ReadFile(SoundArchive::FileId fileId, void* buffer, size_t size, 
    s32 offset, size_t loadBlockSize)
{
    FileStreamHandle stream = const_cast<SoundArchive*>(m_pSoundArchive)->detail_OpenFileStream(
        fileId, m_StreamArea, sizeof(m_StreamArea));
    if(!stream)
    {
        return -1;
    }
    if(!stream->CanSeek() || !stream->CanRead())
    {
        return -1;
    }

    stream->Seek(offset, io::FILE_STREAM_SEEK_BEGIN);

    if(loadBlockSize == 0)
    {
        s32 readByte = stream->Read(buffer, ut::RoundUp(static_cast<u32>(size), 32));
        if(readByte < 0)
        {
            return -1;
        }
    }
    else
    {
        u8* ptr = reinterpret_cast<u8*>(buffer);
        s32 restSize = size;
        while(restSize > 0)
        {
            if(m_IsCancelLoading)
            {
                return -1;
            }

            s32 curReadingSize = ut::RoundUp(ut::Min<s32>(loadBlockSize, restSize), 32);
            s32 readByte = stream->Read(ptr, curReadingSize);
            if(readByte < 0)
            {
                return -1;
            }
            if(restSize > readByte)
            {
                restSize -= readByte;
                ptr += readByte;
            }
            else
            {
                restSize = 0;
            }
        }
    }

    return size;
}

void* SoundArchiveLoader::LoadFile(SoundArchive::FileId fileId, SoundMemoryAllocatable* allocator, 
    size_t loadBlockSize, bool needDeviceMemory)
{
    NW_NULL_ASSERT(allocator);
    NW_NULL_ASSERT(m_pSoundArchive);

    SoundArchive::FileInfo fileInfo;
    if(!m_pSoundArchive->detail_ReadFileInfo(fileId, &fileInfo))
    {
        return NULL;
    }
    u32 fileSize = fileInfo.fileSize;
    if(fileSize == 0)
    {
        return NULL;
    }

    void* buffer = allocator->Alloc(fileSize);
    if(buffer == NULL)
    {
        return NULL;
    }
    if(needDeviceMemory)
    {
        if(!Util::IsDeviceMemory(reinterpret_cast<uptr>(buffer), fileSize))
        {
            NW_ASSERTMSG(false, "buffer, buffer + fileSize is not Device Memory.");
            return false;
        }
    }

    if(ReadFile(fileId, buffer, (s32)fileSize, 0, loadBlockSize) != fileSize)
    {
        return NULL;
    }
    nn::snd::CTR::FlushDataCache(reinterpret_cast<uptr>(buffer), fileSize);

    return buffer;
}

bool SoundArchiveLoader::IsDataLoaded(const char* pItemName, u32 loadFlag) const
{
    NW_NULL_ASSERT(m_pSoundArchive);

    SoundArchive::ItemId id = m_pSoundArchive->GetItemId(pItemName);
    return IsDataLoaded(id, loadFlag);
}

bool SoundArchiveLoader::IsDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag) const
{
    if(!IsAvailable())
    {
        return false;
    }
    if(itemId == SoundArchive::INVALID_ID)
    {
        return false;
    }

    switch(internal::Util::GetItemType(itemId))
    {
    case internal::ItemType_Sound:
        switch(m_pSoundArchive->GetSoundType(itemId))
        {
        case SoundArchive::SOUND_TYPE_SEQ:
            return IsSequenceSoundDataLoaded(itemId, loadFlag);
        case SoundArchive::SOUND_TYPE_WAVE:
            return IsWaveSoundDataLoaded(itemId, loadFlag);
        case SoundArchive::SOUND_TYPE_STRM:
        default:
            return false;
        }
    case internal::ItemType_Bank:
        return IsBankDataLoaded(itemId, loadFlag);
    case internal::ItemType_WaveArchive:
        return IsWaveArchiveDataLoaded(itemId, SoundArchive::INVALID_ID);
    case internal::ItemType_Group:
        return IsGroupDataLoaded(itemId);
    case internal::ItemType_SoundGroup:
        return IsSoundGroupDataLoaded(itemId, loadFlag);
    case internal::ItemType_Player:
    default:
        return false;
    }
}

bool SoundArchiveLoader::IsSequenceSoundDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag) const
{
    if(loadFlag & LOAD_SEQ)
    {
        u32 fileId = m_pSoundArchive->GetItemFileId(itemId);
        if(GetFileAddressImpl(fileId) == NULL)
        {
            return false;
        }
    }

    if(loadFlag & LOAD_BANK || loadFlag & LOAD_WARC)
    {
        SoundArchive::SequenceSoundInfo info;
        if(!m_pSoundArchive->ReadSequenceSoundInfo(itemId, &info))
        {
            return false;
        }

        for(int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
        {
            u32 bankId = info.bankIds[i];
            if(bankId != SoundArchive::INVALID_ID)
            {
                if(!IsBankDataLoaded(bankId, loadFlag))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool SoundArchiveLoader::IsWaveSoundDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag) const
{
    const void* pWsdFile = NULL;
    {
        u32 wsdFileId = m_pSoundArchive->GetItemFileId(itemId);
        pWsdFile = GetFileAddressImpl(wsdFileId);
    }

    if((loadFlag & LOAD_WSD) || (loadFlag & LOAD_WARC))
    {
        if(pWsdFile == NULL)
        {
            return false;
        }
    }

    if(loadFlag & LOAD_WARC)
    {
        u32 index;
        {
            SoundArchive::WaveSoundInfo info;
            if(!m_pSoundArchive->detail_ReadWaveSoundInfo(itemId, &info))
            {
                return false;
            }
            index = info.index;
        }
        u32 warcId;
        u32 waveIndex;
        {
            internal::WaveSoundFileReader reader(pWsdFile);
            internal::WaveSoundNoteInfo info;
            if(!reader.ReadNoteInfo(&info, index, 0))
            {
                return false;
            }
            warcId = info.waveArchiveId;
            waveIndex = info.waveIndex;
        }
        if(!IsWaveArchiveDataLoaded(warcId, waveIndex))
        {
            return false;
        }
    }
    return true;
}

bool SoundArchiveLoader::IsBankDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag) const
{
    const void* pBankFile = NULL;
    {
        u32 fileId = m_pSoundArchive->GetItemFileId(itemId);
        pBankFile = GetFileAddressImpl(fileId);
    }
    if((loadFlag & LOAD_BANK) || (loadFlag & LOAD_WARC))
    {
        if(pBankFile == NULL)
        {
            return false;
        }
    }

    if(loadFlag & LOAD_WARC)
    {
        internal::BankFileReader reader(pBankFile);
        const internal::Util::WaveIdTable& table = reader.GetWaveIdTable();

        for(u32 i = 0; i < table.GetCount(); i++)
        {
            const internal::Util::WaveId* pWaveId = table.GetWaveId(i);
            if(pWaveId == NULL)
            {
                return false;
            }
            u32 warcId = pWaveId->waveArchiveId;
            u32 waveIndex = pWaveId->waveIndex;
            if(!IsWaveArchiveDataLoaded(warcId, waveIndex))
            {
                return false;
            }
        }
    }
    return true;
}

bool SoundArchiveLoader::IsWaveArchiveDataLoaded(SoundArchive::ItemId itemId, u32 waveIndex) const
{
    const void* pWarcFile = NULL;
    {
        u32 fileId = m_pSoundArchive->GetItemFileId(itemId);
        pWarcFile = GetFileAddressImpl(fileId);
    }
    if(pWarcFile == NULL)
    {
        return false;
    }

    SoundArchive::WaveArchiveInfo info;
    if(!m_pSoundArchive->ReadWaveArchiveInfo(itemId, &info))
    {
        return false;
    }
    if(info.isLoadIndividual == true)
    {
        internal::WaveArchiveFileReader reader(pWarcFile, true);
        if(waveIndex != SoundArchive::INVALID_ID)
        {
            if(!reader.IsLoaded(waveIndex))
            {
                return false;
            }
        }
        else
        {
            u32 waveCount = reader.GetWaveFileCount();
            for(u32 i = 0; i < waveCount; i++)
            {
                if(!reader.IsLoaded(i))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool SoundArchiveLoader::IsGroupDataLoaded(SoundArchive::ItemId itemId) const
{
    u32 fileId = m_pSoundArchive->GetItemFileId(itemId);
    if(GetFileAddressImpl(fileId) == NULL)
    {
        return false;
    }
    return true;
}

bool SoundArchiveLoader::IsSoundGroupDataLoaded(SoundArchive::ItemId itemId, u32 loadFlag) const
{
    SoundArchive::SoundGroupInfo info;
    if(!m_pSoundArchive->detail_ReadSoundGroupInfo(itemId, &info))
    {
        return false;
    }

    switch(m_pSoundArchive->GetSoundType(info.startId))
    {
    case SoundArchive::SOUND_TYPE_SEQ:
        for(u32 id = info.startId; id <= info.endId; id++)
        {
            if(!IsSequenceSoundDataLoaded(id, loadFlag))
            {
                return false;
            }
        }
        break;
    case SoundArchive::SOUND_TYPE_WAVE:
        for(u32 id = info.startId; id <= info.endId; id++)
        {
            if(!IsWaveSoundDataLoaded(id, loadFlag))
            {
                return false;
            }
        }
        break;
    }
    return true;
}

const void* SoundArchiveLoader::GetFileAddressFromSoundArchive(SoundArchive::FileId fileId) const
{
    if(m_pSoundArchive == NULL)
    {
        return NULL;
    }
    return m_pSoundArchive->detail_GetFileAddress(fileId);
}

const void* SoundArchiveLoader::detail_GetFileAddressByItemId(SoundArchive::ItemId itemId) const
{
    if(m_pSoundArchive == NULL)
    {
        return NULL;
    }

    SoundArchive::FileId fileId = SoundArchive::INVALID_ID;
    const void* result = NULL;

    switch(Util::GetItemType(itemId))
    {
    case ItemType_Sound:
        {
            SoundArchive::SoundInfo info;
            m_pSoundArchive->ReadSoundInfo(itemId, &info);
            fileId = info.fileId;
        }
        break;
    case ItemType_Bank:
        {
            SoundArchive::BankInfo info;
            m_pSoundArchive->ReadBankInfo(itemId, &info);
            fileId = info.fileId;
        }
        break;
    case ItemType_WaveArchive:
        {
            SoundArchive::WaveArchiveInfo info;
            m_pSoundArchive->ReadWaveArchiveInfo(itemId, &info);
            fileId = info.fileId;
        }
        break;
    case ItemType_Group:
        {
            SoundArchive::GroupInfo info;
            m_pSoundArchive->detail_ReadGroupInfo(itemId, &info);
            fileId = info.fileId;
        }
        break;
    default:
        return NULL;
    }

    if(fileId != SoundArchive::INVALID_ID)
    {
        result = GetFileAddressImpl(fileId);
    }
    return result;
}

bool SoundArchiveLoader::detail_LoadWaveArchiveByBankFile(const void* bankFile, 
    SoundMemoryAllocatable* pAllocator)
{
    if(bankFile == NULL)
    {
        return false;
    }

    internal::BankFileReader reader(bankFile);
    const internal::Util::WaveIdTable& table = reader.GetWaveIdTable();

    for(u32 i = 0; i < table.GetCount(); i++)
    {
        const internal::Util::WaveId* pWaveId = table.GetWaveId(i);
        if(pWaveId == NULL)
        {
            return false;
        }
        if(!LoadWaveArchiveImpl(pWaveId->waveArchiveId, pWaveId->waveIndex, pAllocator, LOAD_WARC))
        {
            return false;
        }
    }
    return true;
}

bool SoundArchiveLoader::detail_LoadWaveArchiveByWaveSoundFile(const void* wsdFile, s32 wsdIndex, 
    SoundMemoryAllocatable* pAllocator)
{
    if(wsdFile == NULL)
    {
        return false;
    }

    u32 warcId = SoundArchive::INVALID_ID;
    u32 waveIndex;
    {
        internal::WaveSoundFileReader reader(wsdFile);
        internal::WaveSoundNoteInfo info;
        if(!reader.ReadNoteInfo(&info, wsdIndex, 0))
        {
            return false;
        }
        warcId = info.waveArchiveId;
        waveIndex = info.waveIndex;
    }
    if(!LoadWaveArchiveImpl(warcId, waveIndex, pAllocator, LOAD_WARC))
    {
        return false;
    }
    return true;
}

} // namespace internal
} // namespace snd
} // namespace nw