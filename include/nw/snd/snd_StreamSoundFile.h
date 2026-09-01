#ifndef NW_SND_STREAM_SOUND_FILE_H_
#define NW_SND_STREAM_SOUND_FILE_H_

#include <nn/types.h>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_Global.h>
#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/ut/ut_ResTypes.h>

namespace nw {
namespace snd {
namespace internal {

struct StreamSoundFile
{
    struct InfoBlock;
    struct InfoBlockBody;
    struct SeekBlock;
    struct DataBlock;

    struct FileHeader : public ut::BinaryFileHeader
    {
    private:
        static const int BLOCK_SIZE = 3;

    public:
        Util::ReferenceWithSize toBlocks[BLOCK_SIZE];

        u32 GetInfoBlockSize() const;
        u32 GetSeekBlockSize() const;
        u32 GetDataBlockSize() const;

        u32 GetInfoBlockOffset() const;
        u32 GetSeekBlockOffset() const;
        u32 GetDataBlockOffset() const;

        const InfoBlock* GetInfoBlock() const
        {
            return static_cast<const InfoBlock*>(ut::AddOffsetToPtr(this, GetInfoBlockOffset()));
        }

    private:
        const Util::ReferenceWithSize* GetReferenceBy( u16 typeId ) const;
    };

    struct StreamSoundInfo;
    struct TrackInfoTable;
    struct ChannelInfoTable;
    struct InfoBlockBody
    {
        Util::Reference toStreamSoundInfo;
        Util::Reference toTrackInfoTable;
        Util::Reference toChannelInfoTable;

        const StreamSoundInfo* GetStreamSoundInfo() const;
        const TrackInfoTable* GetTrackInfoTable() const;
        const ChannelInfoTable* GetChannelInfoTable() const;
    };

    struct InfoBlock
    {
        ut::BinaryBlockHeader header;
        InfoBlockBody body;
    };

    struct StreamSoundInfo
    {
        u8 encodeMethod;
        bool isLoop;
        u8 channelCount;
        u8 padding;
        ut::ResU32 sampleRate;
        ut::ResU32 loopStart;
        ut::ResU32 frameCount;
        ut::ResU32 blockCount;

        ut::ResU32 oneBlockBytes;
        ut::ResU32 oneBlockSamples;

        ut::ResU32 lastBlockBytes;
        ut::ResU32 lastBlockSamples;
        ut::ResU32 lastBlockPaddedBytes;

        ut::ResU32 sizeofSeekInfoAtom;
        ut::ResU32 seekInfoIntervalSamples;

        Util::Reference sampleDataOffset;
    };

    struct TrackInfo;
    struct TrackInfoTable
    {
        Util::ReferenceTable table;

        const TrackInfo* GetTrackInfo(u32 index) const;
        u32 GetTrackCount() const {return table.count; }
    };

    struct GlobalChannelIndexTable;
    struct TrackInfo
    {
        u8 volume;
        u8 pan;
        u16 padding;
        Util::Reference toGlobalChannelIndexTable;

        u32 GetTrackChannelCount() const { return GetGlobalChannelIndexTable().GetCount(); }
        u8 GetGlobalChannelIndex(u32 index) const { return GetGlobalChannelIndexTable().GetGlobalIndex(index); }
    private:
        const GlobalChannelIndexTable& GetGlobalChannelIndexTable() const
        {
            return *reinterpret_cast<const GlobalChannelIndexTable*>(ut::AddOffsetToPtr(this, toGlobalChannelIndexTable.offset));
        }
    };
    struct GlobalChannelIndexTable
    {
        Util::Table<u8> table;

        u32 GetCount() const { return table.count; }
        u8 GetGlobalIndex(u32 index) const{ return table.item[index]; }
    };

    struct ChannelInfo;
    struct ChannelInfoTable
    {
        Util::ReferenceTable table;

        NW_INLINE u32 GetChannelCount() const { return table.count; }
        const ChannelInfo* GetChannelInfo( u32 index ) const;
    };

    struct DspAdpcmChannelInfo;
    struct ChannelInfo
    {
        Util::Reference toDetailChannelInfo;
        const DspAdpcmChannelInfo* GetDspAdpcmChannelInfo() const;
    };

    struct DspAdpcmChannelInfo
    {
        DspAdpcmParam       param;
        DspAdpcmLoopParam   loopParam;
    };

    struct SeekBlock
    {
        ut::BinaryBlockHeader   header;
    };
    struct DataBlock
    {
        ut::BinaryBlockHeader   header;
    };
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_STREAM_SOUND_FILE_H_