
#ifndef NW_SND_WAVE_FILE_H_
#define NW_SND_WAVE_FILE_H_

#include <nn/types.h>

#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_Util.h>
#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/ut/ut_ResTypes.h>

namespace nw {
namespace snd {
namespace internal {

struct WaveFile
{
    enum EncodeMethod
    {
        PCM8,
        PCM16,
        DSP_ADPCM,
        IMA_ADPCM
    };

    struct InfoBlock;
    struct ChannelInfo;
    struct DspAdpcmInfo;
    struct DataBlock;

    struct FileHeader : public Util::SoundFileHeader
    {
        const InfoBlock* GetInfoBlock() const;
        const DataBlock* GetDataBlock() const;
    };

    struct InfoBlockBody
    {
        u8 encoding;
        u8 isLoop;
        u16 padding;
        nw::ut::ResU32 sampleRate;
        nw::ut::ResU32 loopStartFrame;
        nw::ut::ResU32 loopEndFrame;
        nw::ut::ResU32 reserved;
        Util::ReferenceTable channelInfoReferenceTable;

        inline s32 GetChannelCount() const { return channelInfoReferenceTable.count; }
        const ChannelInfo& GetChannelInfo( s32 channelIndex ) const;
    };

    struct InfoBlock
    {
        ut::BinaryBlockHeader   header;
        InfoBlockBody           body;
    };

    struct ChannelInfo
    {
        Util::Reference     referToSamples;
        Util::Reference     referToAdpcmInfo;
        nw::ut::ResU32      reserved;

        const void* GetSamplesAddress(const void* dataBlockBodyAddress) const;
        const DspAdpcmInfo& GetDspAdpcmInfo() const;
    };

    struct DspAdpcmInfo
    {
        DspAdpcmParam      adpcmParam;
        DspAdpcmLoopParam  adpcmLoopParam;
    };

    struct DataBlock
    {
        ut::BinaryBlockHeader   header;
        union
        {
            s8  pcm8[1];
            s16 pcm16[1];
            u8  byte[1];
        };
    };
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_WAVE_FILE_H_