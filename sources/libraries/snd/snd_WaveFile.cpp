// Filename: snd_WaveFile.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_WaveFile.h>

#include <nw/ut/ut_Inlines.h>

namespace nw {
namespace snd {
namespace internal {

/* WaveFile::FileHeader */

const WaveFile::InfoBlock* WaveFile::FileHeader::GetInfoBlock() const
{
    return reinterpret_cast<const InfoBlock*>(GetBlock(28672));
}

const WaveFile::DataBlock* WaveFile::FileHeader::GetDataBlock() const
{
    return reinterpret_cast<const DataBlock*>(GetBlock(28673));
}

/* WaveFile::InfoBlockBody */

const WaveFile::ChannelInfo& WaveFile::InfoBlockBody::GetChannelInfo(s32 channelIndex) const
{
    NW_ASSERT(channelIndex < GetChannelCount());
    return *reinterpret_cast<const ChannelInfo*>(channelInfoReferenceTable.GetReferedItem(channelIndex));
}


/* WaveFile::ChannelInfo */

const void* WaveFile::ChannelInfo::GetSamplesAddress(const void* dataBlockBodyAddress) const
{
    return reinterpret_cast<const void*>(
        ut::AddOffsetToPtr(dataBlockBodyAddress, referToSamples.offset));
}

const WaveFile::DspAdpcmInfo& WaveFile::ChannelInfo::GetDspAdpcmInfo() const
{
    return *reinterpret_cast<const DspAdpcmInfo*>(
        ut::AddOffsetToPtr(this, referToAdpcmInfo.offset));
}


} // namespace internal
} // namespace snd
} // namespace nw
