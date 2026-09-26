#ifndef NW_SND_STREAM_SOUND_FILE_LOADER_H_
#define NW_SND_STREAM_SOUND_FILE_LOADER_H_

#include <nw/io/io_FileStream.h>
#include <nw/snd/snd_StreamSoundFileReader.h>

namespace nw {
namespace snd {
namespace internal {

class StreamSoundFileLoader
{
public:
    StreamSoundFileLoader(io::FileStream& stream): 
        m_Stream(stream)
    {
    }

    bool LoadFileHeader(void* buffer, unsigned long size);

    bool ReadStreamInfo( StreamSoundFile::StreamSoundInfo* strmInfo) const;
    bool ReadStreamTrackInfo(
        StreamSoundFileReader::TrackInfo* trackInfo, int trackIndex) const;
    bool ReadDspAdpcmChannelInfo(DspAdpcmParam* adpcmParam,
        DspAdpcmLoopParam* adpcmLoopParam, int channelIndex) const;
    u32 GetChannelCount() const;
    u32 GetTrackCount() const;

    u32 GetDataBlockOffset() const
    {
        return m_Reader.GetDataBlockOffset();
    }

    bool ReadAdpcBlockData(u16* yn1, u16* yn2, int blockIndex, int channelCount);

private:
    io::FileStream& m_Stream;
    StreamSoundFileReader m_Reader;
};

} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_STREAM_SOUND_FILE_LOADER_H_

