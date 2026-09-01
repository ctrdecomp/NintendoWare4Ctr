#ifndef NW_SND_STREAM_SOUND_FILE_READER_H_
#define NW_SND_STREAM_SOUND_FILE_READER_H_

#include <nw/snd/snd_StreamSoundFile.h>
#include <nw/snd/snd_Global.h>

namespace nw {
namespace snd {
namespace internal {

class StreamSoundFileReader
{
public:
    struct TrackInfo
    {
        u8 volume;
        u8 pan;
        u8 channelCount;
        u8 globalChannelIndex[WAVE_CHANNEL_MAX];
    };

    StreamSoundFileReader();
    void Initialize(const void* streamSoundFile);
    void Finalize();
    bool IsAvailable()const { return m_pHeader != NULL; }

    bool IsValidFileHeader(const void* streamSoundFile) const;

    bool ReadStreamSoundInfo(StreamSoundFile::StreamSoundInfo* strmInfo ) const;
    bool ReadStreamTrackInfo(TrackInfo* trackInfo, int trackIndex ) const;
    bool ReadDspAdpcmChannelInfo(DspAdpcmParam* param, DspAdpcmLoopParam* loopParam, int channelIndex) const;

    u32 GetChannelCount() const{ return m_pInfoBlockBody->GetChannelInfoTable()->GetChannelCount(); }

    u32 GetTrackCount() const{ return m_pInfoBlockBody->GetTrackInfoTable()->GetTrackCount(); }

    u32 GetSeekBlockOffset() const
    {
        if (IsAvailable())
            return m_pHeader->GetSeekBlockOffset();
        return 0;
    }

    u32 GetDataBlockOffset() const
    {
        if (IsAvailable())
            return m_pHeader->GetDataBlockOffset();
        return 0;
    }
private:
    const StreamSoundFile::FileHeader*      m_pHeader;
    const StreamSoundFile::InfoBlockBody*   m_pInfoBlockBody;
};

} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_STREAM_SOUND_FILE_READER_H_