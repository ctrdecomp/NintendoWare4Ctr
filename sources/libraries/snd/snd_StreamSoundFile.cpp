// Filename: snd_StreamSoundFile.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_StreamSoundFile.h>

namespace nw {
namespace snd {
namespace internal {

/* StreamSoundFile::FileHeader */

const Util::ReferenceWithSize*
StreamSoundFile::FileHeader::GetReferenceBy( u16 typeId ) const
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (toBlocks[i].typeId == typeId)
        {
            return &toBlocks[i];
        }
    }
    return NULL;
}

u32 StreamSoundFile::FileHeader::GetInfoBlockSize() const
{
    return GetReferenceBy(16384)->size;
}

u32 StreamSoundFile::FileHeader::GetSeekBlockSize() const
{
    return GetReferenceBy(16385)->size;
}

u32 StreamSoundFile::FileHeader::GetDataBlockSize() const
{
    return GetReferenceBy(16386)->size;
}

u32 StreamSoundFile::FileHeader::GetInfoBlockOffset() const
{
    return GetReferenceBy(16384)->offset;
}

u32 StreamSoundFile::FileHeader::GetSeekBlockOffset() const
{
    return GetReferenceBy(16385)->offset;
}

u32 StreamSoundFile::FileHeader::GetDataBlockOffset() const
{
    return GetReferenceBy(16386)->offset;
}

/* StreamSoundFile::InfoBlockBody */

const StreamSoundFile::StreamSoundInfo*
StreamSoundFile::InfoBlockBody::GetStreamSoundInfo() const
{
    if (toStreamSoundInfo.typeId != 16640)
    {
        return NULL;
    }
    return static_cast<const StreamSoundInfo*>(
        ut::AddOffsetToPtr(this, toStreamSoundInfo.offset));
}
const StreamSoundFile::TrackInfoTable*
StreamSoundFile::InfoBlockBody::GetTrackInfoTable() const
{
    if (toTrackInfoTable.typeId != 257)
    {
        return NULL;
    }
    return static_cast<const TrackInfoTable*>(
        ut::AddOffsetToPtr(this, toTrackInfoTable.offset));
}

const StreamSoundFile::ChannelInfoTable*
StreamSoundFile::InfoBlockBody::GetChannelInfoTable() const
{
    if (toChannelInfoTable.typeId != 257)
    {
        return NULL;
    }
    return static_cast<const ChannelInfoTable*>(
        ut::AddOffsetToPtr(this, toChannelInfoTable.offset));
}

/* StreamSoundFile::TrackInfoTable */

const StreamSoundFile::TrackInfo*
StreamSoundFile::TrackInfoTable::GetTrackInfo(u32 index) const
{
    return static_cast<const TrackInfo*>(
        table.GetReferedItem(index, 16641));
}

/* StreamSoundFile::ChannelInfoTable */

const StreamSoundFile::ChannelInfo*
StreamSoundFile::ChannelInfoTable::GetChannelInfo(u32 index) const
{
    NW_ASSERT(index < table.count);

    return static_cast<const ChannelInfo*>(table.GetReferedItem(index, 16642));
}

/* StreamSoundFile::ChannlInfo */

const StreamSoundFile::DspAdpcmChannelInfo*
StreamSoundFile::ChannelInfo::GetDspAdpcmChannelInfo() const
{
    if (!toDetailChannelInfo.IsValidTypeId(768))
    {
        return NULL;
    }

    return reinterpret_cast<const DspAdpcmChannelInfo*>(ut::AddOffsetToPtr(this, toDetailChannelInfo.offset));
}

} // namespace internal
} // namespace snd
} // namespace nw

