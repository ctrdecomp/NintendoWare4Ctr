#ifndef NW_SND_SOUND_ARCHIVE_H_
#define NW_SND_SOUND_ARCHIVE_H_

#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_Config.h>

namespace nw  {
namespace snd {
namespace internal {

class SoundArchiveFileReader;

} // namespace internal

class SoundArchive
{
public:
    typedef u32 ItemId;

    static const ItemId INVALID_ID = 0xffffffff;

    typedef ItemId FileId;
    typedef ItemId StringId;

    enum SoundType
    {
        SOUND_TYPE_INVALID,
        SOUND_TYPE_SEQ,
        SOUND_TYPE_STRM,
        SOUND_TYPE_WAVE
    };

    struct SoundInfo
    {
        FileId fileId;
        ItemId playerId;

        int actorPlayerId;

        int playerPriority;
        int volume;
        PanMode panMode;
        PanCurve panCurve;

        bool isFrontBypass;
    };

    struct Sound3DInfo
    {
        enum DecayCurve
        {
            DECAY_CURVE_LOG    = 1,
            DECAY_CURVE_LINEAR = 2
        };

        enum FlagCtrl
        {
            FLAG_CTRL_VOLUME   = (1 << 0),
            FLAG_CTRL_PRIORITY = (1 << 1),
            FLAG_CTRL_PAN      = (1 << 2),
            FLAG_CTRL_SPAN     = (1 << 3),
            FLAG_CTRL_FILTER   = (1 << 4)
        };

        u32 flags;          
        f32 decayRatio;
        u8 decayCurve;
        u8 dopplerFactor;
    };

    static const u32 SEQ_BANK_MAX = nw::snd::SEQ_BANK_MAX;

    struct SequenceSoundInfo
    {
        u32 startOffset;
        u32 bankIds[SEQ_BANK_MAX];

        u32 allocateTrackFlags;
        u8  channelPriority;
        bool isReleasePriorityFix;

        SequenceSoundInfo(): 
            startOffset(0), 
            allocateTrackFlags(0), 
            channelPriority(0), 
            isReleasePriorityFix(false)
            {
            for (u32 i = 0; i < SEQ_BANK_MAX; i++)
                bankIds[i] = INVALID_ID;
        }
    };

    static const u32 STRM_TRACK_NUM = internal::STRM_TRACK_NUM;

    struct StreamSoundInfo
    {
        u16 allocTrackCount;
        u16 allocChannelCount;

        StreamSoundInfo(): 
            allocTrackCount(0),
            allocChannelCount(0)
            {
        }
    };

    struct WaveSoundInfo
    {
        u32     index;
        u32     allocTrackCount;
        u8      channelPriority;
        bool    isReleasePriorityFix;

        WaveSoundInfo(): 
            allocTrackCount(0), 
            channelPriority(0), 
            isReleasePriorityFix(false)
            {
        }
    };

    struct BankInfo
    {
        FileId fileId;

        BankInfo(): 
            fileId(INVALID_ID) 
            {
        }
    };

    struct PlayerInfo
    {
        int playableSoundMax;
        u32 playerHeapSize;

        PlayerInfo(): 
            playableSoundMax(0),
            playerHeapSize(0) 
            {
        }
    };

    struct SoundGroupInfo
    {
        ItemId startId;
        ItemId endId;
        const internal::Util::Table<ut::ResU32>* fileIdTable;

        SoundGroupInfo(): 
            startId(INVALID_ID),
            endId(INVALID_ID),
            fileIdTable(NULL) 
            {
        }
    };

    struct SoundArchivePlayerInfo
    {
        int sequenceSoundMax;
        int sequenceTrackMax;
        int streamSoundMax;
        int streamTrackMax;
        int streamChannelMax;
        int waveSoundMax;
        int waveTrackMax;
    };

    struct WaveArchiveInfo
    {
        u32  fileId;
        u32  waveCount;
        bool isLoadIndividual;
        u8   padding1;
        u16  padding2;

        WaveArchiveInfo(): 
            fileId(INVALID_ID), 
            isLoadIndividual(false) 
            {
        }
    };

    struct GroupInfo
    {
        FileId fileId;
        GroupInfo(): 
            fileId(INVALID_ID) 
            {
        }
    };

    struct FileInfo
    {
        u32 fileSize;
        u32 offsetFromFileBlockHead;
        const char* externalFilePath;

        FileInfo(): 
            fileSize(0xffffffff),
            offsetFromFileBlockHead(0xffffffff),
            externalFilePath(NULL)
            {
        }
    };

public:
    SoundArchive();
    
    virtual ~SoundArchive();
    virtual const void* detail_GetFileAddress(FileId fileId) const = 0;
    virtual size_t detail_GetRequiredStreamBufferSize() const = 0;
    virtual io::FileStream* OpenStream(void* buffer, int size, u32 begin, u32 length) = 0;
    virtual io::FileStream* OpenExtStream(void* buffer, int size, const char* extFilePath, u32 begin, u32 length) const = 0;

    bool IsAvailable() const;
    u32 GetSoundCount() const;
    u32 GetGroupCount() const;
    u32 GetPlayerCount() const;
    u32 GetSoundGroupCount() const;
    u32 GetBankCount() const;
    u32 GetWaveArchiveCount() const;

    u32 detail_GetFileCount() const;

    const char* GetItemLabel(ItemId id) const;
    ItemId GetItemId(const char* pLabel) const;
    FileId GetItemFileId(ItemId id) const;

    static ItemId GetSoundIdFromIndex(u32 index)
    {
        return internal::Util::GetMaskedItemId(index, internal::ItemType_Sound);
    }

    static ItemId GetSoundGroupIdFromIndex(u32 index)
    {
        return internal::Util::GetMaskedItemId(index, internal::ItemType_SoundGroup);
    }

    static ItemId GetBankIdFromIndex(u32 index)
    {
        return internal::Util::GetMaskedItemId(index, internal::ItemType_Bank);
    }

    static ItemId GetPlayerIdFromIndex(u32 index)
    {
        return internal::Util::GetMaskedItemId(index, internal::ItemType_Player);
    }

    static ItemId GetWaveArchiveIdFromIndex(u32 index)
    {
        return internal::Util::GetMaskedItemId(index, internal::ItemType_WaveArchive);
    }

    static ItemId GetGroupIdFromIndex(u32 index)
    {
        return internal::Util::GetMaskedItemId(index, internal::ItemType_Group);
    }

    bool ReadSoundInfo(ItemId soundId, SoundInfo* info) const;
    bool ReadSequenceSoundInfo(ItemId soundId, SequenceSoundInfo* info) const;
    bool ReadBankInfo(ItemId bankId, BankInfo* info) const;
    bool ReadPlayerInfo(ItemId playerId, PlayerInfo* info) const;
    bool ReadSoundArchivePlayerInfo(SoundArchivePlayerInfo* info) const;
    bool ReadSound3DInfo(ItemId soundId, Sound3DInfo* info) const;
    bool ReadWaveArchiveInfo(ItemId warcId, WaveArchiveInfo* info) const;

    bool detail_ReadStreamSoundInfo(ItemId soundId, StreamSoundInfo* info) const;
    bool detail_ReadWaveSoundInfo(ItemId soundId, WaveSoundInfo* info) const;
    bool detail_ReadSoundGroupInfo(ItemId groupId, SoundGroupInfo* info) const;
    bool detail_ReadGroupInfo(ItemId groupId, GroupInfo* info) const;
    bool detail_ReadFileInfo(FileId fileId, FileInfo* info) const;
    io::FileStream* detail_OpenFileStream(FileId fileId, void* buffer, int size);

    u32 GetSoundUserParam(ItemId soundId) const;
private:
    static const int FILE_PATH_CHARS_MAX = 256;

    io::FileStream* OpenExtStreamImpl(void* buffer, int size, const char* extFilePath, u32 begin, u32 length) const;

    internal::SoundArchiveFileReader* m_pFileReader;
    char m_ExtFileRoot[FILE_PATH_CHARS_MAX];

    u32 m_FileBlockOffset;
};

} 
}

#endif // NW_SND_SOUND_ARCHIVE_H_
