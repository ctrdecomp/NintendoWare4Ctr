// Filename: snd_SoundArchive.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundArchive.h>

#include <cstring>
#include <nw/assert.h>
#include <nw/io/io_FileStream.h>
#include <nw/snd/snd_SoundArchiveFileReader.h>

namespace nw {
namespace snd {

namespace 
{
const char sNullString = '\0';
} // namespace


SoundArchive::SoundArchive(): 
    m_pFileReader(NULL)
{
    m_ExtFileRoot[0] = '/';
    m_ExtFileRoot[1] = '\0';
}

SoundArchive::~SoundArchive()
{
}

bool SoundArchive::IsAvailable() const
{
    if (m_pFileReader == NULL) 
    {
        return false;
    }
    return true;
}

void SoundArchive::Initialize(internal::SoundArchiveFileReader* fileReader)
{
    NW_NULL_ASSERT(fileReader);
    m_pFileReader = fileReader;
    m_FileBlockOffset = m_pFileReader->GetFileBlockOffset();
}

void SoundArchive::Finalize()
{
    m_pFileReader = NULL;
    m_ExtFileRoot[0] = '/';
    m_ExtFileRoot[1] = '\0';
}

u32 SoundArchive::GetSoundCount() const
{
    return m_pFileReader->GetSoundCount();
}

u32 SoundArchive::GetPlayerCount() const
{
    return m_pFileReader->GetPlayerCount();
}

u32 SoundArchive::GetSoundGroupCount() const
{
    return m_pFileReader->GetSoundGroupCount();
}

u32 SoundArchive::GetGroupCount() const
{
    return m_pFileReader->GetGroupCount();
}

u32 SoundArchive::GetBankCount() const
{
    return m_pFileReader->GetBankCount();
}

u32 SoundArchive::GetWaveArchiveCount() const
{
    return m_pFileReader->GetWaveArchiveCount();
}

u32 SoundArchive::detail_GetFileCount() const
{
    return m_pFileReader->GetFileCount();
}

const char* SoundArchive::GetItemLabel(ItemId id) const
{
    return m_pFileReader->GetItemLabel(id);
}
SoundArchive::ItemId SoundArchive::GetItemId(const char* pStr) const
{
    return m_pFileReader->GetItemId(pStr);
}
SoundArchive::FileId SoundArchive::GetItemFileId( ItemId id) const
{
    return m_pFileReader->GetItemFileId( id );
}

u32 SoundArchive::GetSoundUserParam( ItemId soundId ) const
{
    return m_pFileReader->GetSoundUserParam( soundId );
}

SoundArchive::SoundType SoundArchive::GetSoundType(ItemId soundId) const
{
    return m_pFileReader->GetSoundType(soundId);
}

bool SoundArchive::ReadSoundInfo(ItemId soundId, SoundInfo* info) const
{
    return m_pFileReader->ReadSoundInfo(soundId, info);
}

bool SoundArchive::ReadSequenceSoundInfo(ItemId soundId, SequenceSoundInfo* info) const
{
    return m_pFileReader->ReadSequenceSoundInfo(soundId, info);
}

bool SoundArchive::detail_ReadStreamSoundInfo(ItemId soundId, StreamSoundInfo* info) const
{
    return m_pFileReader->ReadStreamSoundInfo(soundId, info);
}

bool SoundArchive::detail_ReadWaveSoundInfo(ItemId soundId, WaveSoundInfo* info) const
{
    return m_pFileReader->ReadWaveSoundInfo(soundId, info);
}

bool SoundArchive::ReadPlayerInfo(ItemId playerId, PlayerInfo* info) const
{
    return m_pFileReader->ReadPlayerInfo(playerId, info);
}

bool SoundArchive::ReadSoundArchivePlayerInfo(SoundArchivePlayerInfo* info) const
{
    return m_pFileReader->ReadSoundArchivePlayerInfo(info);
}

bool SoundArchive::ReadSound3DInfo(ItemId soundId, Sound3DInfo* info) const
{
    return m_pFileReader->ReadSound3DInfo(soundId, info);
}

bool SoundArchive::ReadBankInfo(ItemId bankId, BankInfo* info) const
{
    return m_pFileReader->ReadBankInfo(bankId, info);
}

bool SoundArchive::ReadWaveArchiveInfo(ItemId warcId, WaveArchiveInfo* info) const
{
    return m_pFileReader->ReadWaveArchiveInfo(warcId, info);
}

bool SoundArchive::detail_ReadSoundGroupInfo(ItemId soundGroupId, SoundGroupInfo* info) const
{
    return m_pFileReader->ReadSoundGroupInfo(soundGroupId, info);
}

bool SoundArchive::detail_ReadGroupInfo(ItemId groupId, GroupInfo* info) const
{
    return m_pFileReader->ReadGroupInfo(groupId, info);
}

bool SoundArchive::detail_ReadFileInfo(FileId fileId, FileInfo* info) const
{
    return m_pFileReader->ReadFileInfo(fileId, info);
}

const internal::Util::Table<u32>*
SoundArchive::detail_GetWaveArchiveIdTable(ItemId id) const
{
    return m_pFileReader->GetWaveArchiveIdTable(id);
}

io::FileStream* SoundArchive::detail_OpenFileStream(FileId fileId, void* buffer, int size )
{
    FileInfo fileInfo;
    if (!detail_ReadFileInfo(fileId, &fileInfo))
    {
        return NULL;
    }

    if (fileInfo.externalFilePath != NULL)
    {
        io::FileStream* stream = OpenExtStreamImpl(buffer, size, fileInfo.externalFilePath, 0, 0);
        return stream;
    }

    io::FileStream* stream = OpenStream(buffer, size, fileInfo.offsetFromFileBlockHead + m_FileBlockOffset,
        fileInfo.fileSize);
    return stream;
}


io::FileStream* SoundArchive::OpenExtStreamImpl(void* buffer, int size, 
    const char* externalFilePath, u32 begin, u32 length) const
{
    const char* fullPath;
    char pathBuffer[FILE_PATH_MAX];

    if (externalFilePath[0] == '/')
    {
        fullPath = externalFilePath;
    }
    else
    {
        std::size_t fileLen = std::strlen(externalFilePath);
        std::size_t dirLen = std::strlen(m_ExtFileRoot);
        if (fileLen + dirLen >= sizeof(pathBuffer) / sizeof(pathBuffer[0]))
        {
            NW_WARNING(false, "Too long file path \"%s/%s\"", m_ExtFileRoot, externalFilePath);
            return NULL;
        }
        ut::strncpy(pathBuffer, FILE_PATH_MAX, m_ExtFileRoot, dirLen + 1);
        ut::strncat(pathBuffer, FILE_PATH_MAX, externalFilePath, fileLen + 1);
        fullPath = pathBuffer;
    }

    return OpenExtStream(buffer, size, fullPath, begin, length);
}

void SoundArchive::SetExternalFileRoot(const char* extFileRoot)
{
    std::size_t len = std::strlen(extFileRoot);
    std::size_t nullPos = len;

    ut::strncpy(m_ExtFileRoot, FILE_PATH_MAX, extFileRoot, len);

    if (extFileRoot[len - 1] != '/')
    {
        m_ExtFileRoot[len] = '/';
        nullPos++;
    }

    NW_ASSERT(nullPos < FILE_PATH_MAX);
    m_ExtFileRoot[nullPos] = '\0';
}

} // namespace snd
} // namespace nw

