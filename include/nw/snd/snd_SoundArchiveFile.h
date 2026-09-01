#ifndef NW_SND_SOUND_ARCHIVE_FILE_H_
#define NW_SND_SOUND_ARCHIVE_FILE_H_

#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_SoundArchive.h>

namespace nw {
namespace snd {
namespace internal {

struct SoundArchiveFile
{
    struct StringBlock;
    struct InfoBlock;
    struct FileBlock;

    static const int BLOCK_SIZE = 3;

    struct FileHeader : public ut::BinaryFileHeader
    {
        Util::ReferenceWithSize toBlocks[BLOCK_SIZE];

        u32 GetStringBlockSize() const;
        u32 GetInfoBlockSize() const;
        u32 GetFileBlockSize() const;

        s32 GetStringBlockOffset() const;
        s32 GetInfoBlockOffset() const;
        s32 GetFileBlockOffset() const;

    private:
        const Util::ReferenceWithSize* GetReferenceBy( u16 typeId ) const;
    };

    struct StringTable;
    struct PatriciaTree;
    struct StringBlockBody
    {
    private:
        enum Sections
        {
            Sections_StringTable,
            Sections_PatriciaTree,
            Sections_Max = Sections_PatriciaTree
        };

    public:
        Util::Reference toSection[ Sections_Max ];

        const char* GetString( SoundArchive::ItemId stringId ) const;
        u32 GetStringCount() const { return GetStringTable()->GetCount(); }

        u32 GetItemId(const char* str) const{ return GetItemIdImpl(Sections_PatriciaTree, str); }

    private:
        const void* GetSection( Sections section ) const;
        const StringTable* GetStringTable() const{ return reinterpret_cast<const StringTable*>(GetSection(Sections_StringTable)); }
        const PatriciaTree* GetPatriciaTree(Sections section) const{ return reinterpret_cast<const PatriciaTree*>(GetSection(section)); }
        u32 GetItemIdImpl( Sections section, const char* str ) const;
    };

    struct PatriciaTree
    {
        struct NodeData
        {
            ut::ResU32 stringId;
            ut::ResU32 itemId;
        };

        struct Node
        {
            static const u16 FLAG_LEAF = ( 1 << 0 );
            ut::ResU16  flags;
            ut::ResU16  bit;
            ut::ResU32  leftIdx;
            ut::ResU32  rightIdx;
            NodeData    nodeData;
        };

        ut::ResU32 rootIdx;
        Util::Table<Node> nodeTable;

        const NodeData* GetNodeDataBy( const char* str, std::size_t len = 0 ) const;

        void* operator[](int idx) const
        {
            return const_cast<void*>(reinterpret_cast<const void*>(&nodeTable.item[idx]));
        }
        void* operator[](u32 idx) const
        {
            return operator[]( static_cast<int>(idx) );
        }
        void* operator[](const char* s) const
        {
            return operator()(s, 0);
        }
        void* operator()(const char* s, size_t len) const
        {
            return const_cast<void*>(reinterpret_cast<const void*>(GetNodeDataBy(s, len)));
        }
        u32 GetNumData() const { return nodeTable.count; }
        u32 GetCount() const { return GetNumData(); }
    };

    struct StringBlock
    {
        ut::BinaryBlockHeader   header;
        StringBlockBody         body;
    };

    struct StringTable
    {
        Util::ReferenceWithSizeTable table;

        const char* GetString(int stringId) const { return reinterpret_cast<const char*>(ut::AddOffsetToPtr(this, table.item[stringId].offset)); }
        u32 GetCount() const{ return table.count; }
    };

    struct SoundInfo;
    struct BankInfo;
    struct PlayerInfo;
    struct SoundGroupInfo;
    struct GroupInfo;
    struct WaveArchiveInfo;
    struct FileInfo;
    struct SoundArchivePlayerInfo;

    struct InfoBlockBody
    {
        Util::Reference toSoundInfoReferenceTable;
        Util::Reference toSoundGroupInfoReferenceTable;
        Util::Reference toBankInfoReferenceTable;
        Util::Reference toWaveArchiveInfoReferenceTable;
        Util::Reference toGroupInfoReferenceTable;
        Util::Reference toPlayerInfoReferenceTable;
        Util::Reference toFileInfoReferenceTable;
        Util::Reference toSoundArchivePlayerInfo;

        u32 GetSoundCount() const       { return GetSoundInfoReferenceTable().count; }
        u32 GetBankCount() const        { return GetBankInfoReferenceTable().count; }
        u32 GetPlayerCount() const      { return GetPlayerInfoReferenceTable().count; }
        u32 GetSoundGroupCount() const  { return GetSoundGroupInfoReferenceTable().count; }
        u32 GetGroupCount() const       { return GetGroupInfoReferenceTable().count; }
        u32 GetWaveArchiveCount() const { return GetWaveArchiveInfoReferenceTable().count; }
        u32 GetFileCount() const        { return GetFileInfoReferenceTable().count; }

        const SoundInfo*        GetSoundInfo( SoundArchive::ItemId soundId ) const;
        const BankInfo*         GetBankInfo( SoundArchive::ItemId bankId ) const;
        const PlayerInfo*       GetPlayerInfo( SoundArchive::ItemId playerId ) const;
        const SoundGroupInfo*   GetSoundGroupInfo( SoundArchive::ItemId soundGroupId ) const;
        const GroupInfo*        GetGroupInfo( SoundArchive::ItemId groupId ) const;
        const WaveArchiveInfo*  GetWaveArchiveInfo( SoundArchive::ItemId warcId ) const;
        const FileInfo*         GetFileInfo( SoundArchive::FileId fileId ) const;

        const SoundArchivePlayerInfo* GetSoundArchivePlayerInfo() const;

        SoundArchive::FileId    GetItemFileId( SoundArchive::ItemId id ) const;
        SoundArchive::StringId  GetItemStringId( SoundArchive::ItemId id ) const;

    private:
        const Util::ReferenceTable& GetSoundInfoReferenceTable() const;
        const Util::ReferenceTable& GetBankInfoReferenceTable() const;
        const Util::ReferenceTable& GetPlayerInfoReferenceTable() const;
        const Util::ReferenceTable& GetSoundGroupInfoReferenceTable() const;
        const Util::ReferenceTable& GetGroupInfoReferenceTable() const;
        const Util::ReferenceTable& GetWaveArchiveInfoReferenceTable() const;
        const Util::ReferenceTable& GetFileInfoReferenceTable() const;
    };

    struct InfoBlock
    {
        ut::BinaryBlockHeader   header;
        InfoBlockBody           body;
    };


    struct StreamSoundInfo;
    struct WaveSoundInfo;
    struct SequenceSoundInfo;
    struct Sound3DInfo;

    struct SoundInfo
    {
        ut::ResU32      fileId;
        ut::ResU32      playerId;
        ut::ResU8       volume;
        u8              padding0;
        u16             padding1;
        Util::Reference toDetailSoundInfo;
        Util::BitFlag   optionParameter;

        SoundArchive::SoundType     GetSoundType() const;
        const StreamSoundInfo&      GetStreamSoundInfo() const;
        const WaveSoundInfo&        GetWaveSoundInfo() const;
        const SequenceSoundInfo&    GetSequenceSoundInfo() const;
        const Sound3DInfo*          GetSound3DInfo() const;

        u32         GetStringId() const;
        PanMode     GetPanMode() const;
        PanCurve    GetPanCurve() const;
        u8          GetPlayerPriority() const;
        u8          GetActorPlayerId() const;
        u32         GetUserParam() const;
        bool        IsFrontBypass() const;
    };

    struct StreamSoundInfo
    {
        ut::ResU16 allocTrackCount;
        ut::ResU16 allocChannelCount;
        Util::BitFlag optionParameter;
    };

    struct WaveSoundInfo
    {
        ut::ResU32 index;
        ut::ResU32 allocTrackCount;
        Util::BitFlag optionParameter;

        u8 GetChannelPriority() const;
        u8 GetIsReleasePriorityFix() const;

    };

    struct SequenceSoundInfo
    {
        Util::Reference toBankIdTable;
        ut::ResU32      allocateTrackFlags;
        Util::BitFlag   optionParameter;

        void GetBankIds(u32* bankIds) const;
        u32 GetStartOffset() const;
        u8  GetChannelPriority() const;
        bool IsReleasePriorityFix() const;
    private:
        const Util::Table<ut::ResU32>& GetBankIdTable() const;
    };

    struct Sound3DInfo
    {
        ut::ResU32 flags;
        ut::ResF32 decayRatio;
        u8 decayCurve;
        u8 dopplerFactor;
        u8 padding[2];
        Util::BitFlag optionParameter;
    };

    struct BankInfo
    {
        ut::ResU32      fileId;
        Util::Reference toWaveArchiveItemIdTable;
        Util::BitFlag   optionParameter;

        u32 GetStringId() const;

        const Util::Table<ut::ResU32>* GetWaveArchiveItemIdTable() const
        {
            return reinterpret_cast<const Util::Table<ut::ResU32>*>(ut::AddOffsetToPtr(this, toWaveArchiveItemIdTable.offset));
        }
    };

    struct PlayerInfo
    {
        ut::ResU32      playableSoundMax;
        Util::BitFlag   optionParameter;

        u32 GetStringId() const;
        u32 GetPlayerHeapSize() const;
    };

    struct WaveSoundGroupInfo;
    struct SoundGroupInfo
    {
        ut::ResU32      startId;
        ut::ResU32      endId;
        Util::Reference toFileIdTable;
        Util::Reference toDetailSoundGroupInfo;
        Util::BitFlag   optionParameter;

        u32 GetStringId() const;

        inline const Util::Table<ut::ResU32>* GetFileIdTable() const
        {
            return reinterpret_cast<const Util::Table<ut::ResU32>*>(ut::AddOffsetToPtr(this, toFileIdTable.offset));
        }
        inline const WaveSoundGroupInfo* GetWaveSoundGroupInfo() const
        {
            if (toDetailSoundGroupInfo.typeId != 8709)
            {
                return NULL;
            }

            return reinterpret_cast<const WaveSoundGroupInfo*>(ut::AddOffsetToPtr(this, toDetailSoundGroupInfo.offset));
        }
    };

    struct WaveSoundGroupInfo
    {
        Util::Reference toWaveArchiveItemIdTable;
        Util::BitFlag   optionParameter;

        inline const Util::Table<ut::ResU32>* GetWaveArchiveItemIdTable() const
        {
            return reinterpret_cast<const Util::Table<ut::ResU32>*>(ut::AddOffsetToPtr(this, toWaveArchiveItemIdTable.offset));
        }
    };

    struct GroupInfo
    {
        ut::ResU32      fileId;
        Util::BitFlag   optionParameter;

        u32 GetStringId() const;
    };

    struct WaveArchiveInfo
    {
        ut::ResU32      fileId;
        bool            isLoadIndividual;
        u8              padding1;
        u16             padding2;
        Util::BitFlag   optionParameter;

        u32 GetStringId() const;
        u32 GetWaveCount() const;
    };

    enum FileLocationType
    {
        FILE_LOCATION_TYPE_INTERNAL,
        FILE_LOCATION_TYPE_EXTERNAL,
        FILE_LOCATION_TYPE_NONE
    };

    struct InternalFileInfo;
    struct ExternalFileInfo;

    struct FileInfo
    {
        Util::Reference toFileLocation;
        Util::BitFlag   optionParameter;

        FileLocationType GetFileLocationType() const;
        const InternalFileInfo* GetInternalFileInfo() const;
        const ExternalFileInfo* GetExternalFileInfo() const;
    };

    struct InternalFileInfo
    {
        Util::ReferenceWithSize toFileImageFromFileBlockBody;

        u32 GetFileSize() const
        {
            return toFileImageFromFileBlockBody.size;
        }
        u32 GetOffsetFromFileBlockHead() const
        {
            return toFileImageFromFileBlockBody.offset + sizeof(ut::BinaryBlockHeader);
        }
    };

    struct ExternalFileInfo
    {
        char filePath[1];
    };

    struct SoundArchivePlayerInfo
    {
        nw::ut::ResU16  sequenceSoundMax;
        nw::ut::ResU16  sequenceTrackMax;
        nw::ut::ResU16  streamSoundMax;
        nw::ut::ResU16  streamTrackMax;
        nw::ut::ResU16  streamChannelMax;
        nw::ut::ResU16  waveSoundMax;
        nw::ut::ResU16  waveTrackmax;
        u16             padding;
        nw::ut::ResU32  options;
    };

    struct FileBlockBody
    {
        /* Empty just like your love life is */
    };

    struct FileBlock
    {
        ut::BinaryBlockHeader   header;
        FileBlockBody           body;
    };

};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_ARCHIVE_FILE_H_