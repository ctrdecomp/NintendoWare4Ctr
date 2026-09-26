#ifndef NW_SND_SOUND_ARCHIVE_FILE_READER_H_
#define NW_SND_SOUND_ARCHIVE_FILE_READER_H_

#include <nw/snd/snd_SoundArchiveFile.h>
#include <nw/snd/snd_SoundArchive.h>

namespace nw {
namespace snd {
namespace internal {

class SoundArchiveFileReader
{
public:
    static const u32 SIGNATURE_FILE = NW_UT_MAKE_SIGWORD('C', 'S', 'A', 'R');

    SoundArchiveFileReader();
    ~SoundArchiveFileReader() {}

    void Initialize(const void* soundArchiveData);
    void Finalize();

    void SetStringBlock(const void* stringBlock);
    void SetInfoBlock(const void* infoBlock);
    void SetInfoChunk(const void* infoChuck);

    u32 GetStringBlockSize() const { return m_Header.GetStringBlockSize(); }
    s32 GetStringBlockOffset() const { return m_Header.GetStringBlockOffset(); }
    u32 GetInfoBlockSize() const { return m_Header.GetInfoBlockSize(); }
    s32 GetInfoBlockOffset() const { return m_Header.GetInfoBlockOffset(); }
    u32 GetFileBlockSize() const { return m_Header.GetFileBlockSize(); }
    s32 GetFileBlockOffset() const { return m_Header.GetFileBlockOffset(); }

    u32 GetStringCount() const;
    SoundArchive::ItemId GetItemId(const char* pStr) const;
    const char* GetItemLabel(SoundArchive::ItemId id) const;
    SoundArchive::FileId GetItemFileId(SoundArchive::ItemId id) const;

    void DumpTree() const;
    const char* GetString(SoundArchive::ItemId stringId) const;

    u32 GetSoundCount() const;
    u32 GetBankCount() const;
    u32 GetPlayerCount() const;
    u32 GetSoundGroupCount() const;
    u32 GetGroupCount() const;
    u32 GetWaveArchiveCount() const;
    u32 GetFileCount() const;

    bool ReadSoundInfo(SoundArchive::ItemId soundId, SoundArchive::SoundInfo* info) const;
    bool ReadBankInfo(SoundArchive::ItemId bankId,SoundArchive::BankInfo* info ) const;
    bool ReadPlayerInfo(SoundArchive::ItemId playerId, SoundArchive::PlayerInfo* info) const;
    bool ReadSoundGroupInfo(SoundArchive::ItemId soundGroupId, SoundArchive::SoundGroupInfo* info) const;
    bool ReadGroupInfo(SoundArchive::ItemId groupId, SoundArchive::GroupInfo* info) const;
    bool ReadWaveArchiveInfo(SoundArchive::ItemId warcId, SoundArchive::WaveArchiveInfo* info) const;
    bool ReadFileInfo(SoundArchive::FileId id, SoundArchive::FileInfo* info, int index = 0) const;
    bool ReadSoundArchivePlayerInfo(SoundArchive::SoundArchivePlayerInfo* info) const;

    SoundArchive::SoundType GetSoundType(SoundArchive::ItemId soundId) const;
    u32 GetSoundUserParam(SoundArchive::ItemId soundId) const;
    bool ReadSound3DInfo(SoundArchive::ItemId soundId, SoundArchive::Sound3DInfo* info) const;
    bool ReadSequenceSoundInfo(SoundArchive::ItemId soundId, SoundArchive::SequenceSoundInfo* info) const;
    bool ReadStreamSoundInfo(SoundArchive::ItemId soundId, SoundArchive::StreamSoundInfo* info) const;
    bool ReadWaveSoundInfo(SoundArchive::ItemId soundId, SoundArchive::WaveSoundInfo* info) const;

    const internal::Util::Table<u32>* GetWaveArchiveIdTable(SoundArchive::ItemId id) const;

private:
    SoundArchiveFile::FileHeader m_Header;
    const SoundArchiveFile::StringBlockBody* m_pStringBlockBody;
    const SoundArchiveFile::InfoBlockBody* m_pInfoBlockBody;
    const SoundArchiveFile::FileBlockBody* m_pFileBlockBody;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_ARCHIVE_FILE_READER_H_