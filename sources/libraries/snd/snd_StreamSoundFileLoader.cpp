// Filename: snd_StreamSoundFileLoader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_StreamSoundFileLoader.h>

namespace nw {
namespace snd {
namespace internal {


bool StreamSoundFileLoader::LoadFileHeader(void* buffer, unsigned long size)
{
    const size_t headerSize = NW_ROUND_UP_32B(sizeof(StreamSoundFile::FileHeader));
    u8 buffer2[headerSize + 32];

    m_Stream.Seek(0, io::FILE_STREAM_SEEK_BEGIN);
    s32 readSize = m_Stream.Read( ut::RoundUp(buffer2, 32), headerSize);
    if (readSize != headerSize)
    {
        return false;
    }

    StreamSoundFile::FileHeader* header = reinterpret_cast<StreamSoundFile::FileHeader*>(ut::RoundUp(buffer2, 32));

    StreamSoundFileReader reader;
    if (!reader.IsValidFileHeader(header))
    {
        return false;
    }

    u32 loadSize = header->GetInfoBlockOffset() + header->GetInfoBlockSize();
    if (loadSize > size)
    {
        return false;
    }

    m_Stream.Seek(0, io::FILE_STREAM_SEEK_BEGIN);
    readSize = m_Stream.Read(buffer, loadSize);
    if (readSize != loadSize)
    {
        return false;
    }

    m_Reader.Initialize(buffer);

    return true;
}

u32 StreamSoundFileLoader::GetTrackCount() const
{
    if (!m_Reader.IsAvailable()) 
    {
        return 0;
    }
    return m_Reader.GetTrackCount();
}

u32 StreamSoundFileLoader::GetChannelCount() const
{
    if (!m_Reader.IsAvailable()) 
    {
        return 0;
    }
    return m_Reader.GetChannelCount();
}

bool StreamSoundFileLoader::ReadStreamInfo(StreamSoundFile::StreamSoundInfo* strmInfo) const
{
    if (!m_Reader.IsAvailable()) 
    {
        return false;
    }
    m_Reader.ReadStreamSoundInfo(strmInfo);
    return true;
}

bool StreamSoundFileLoader::ReadStreamTrackInfo(StreamSoundFileReader::TrackInfo* trackInfo, int trackIndex) const
{
    if (!m_Reader.IsAvailable()) 
    {
        return false;
    }
    m_Reader.ReadStreamTrackInfo(trackInfo, trackIndex);
    return true;
}

bool StreamSoundFileLoader::ReadDspAdpcmChannelInfo(DspAdpcmParam* adpcmParam, 
    DspAdpcmLoopParam* adpcmLoopParam, int channelIndex) const
{
    if (!m_Reader.IsAvailable()) 
    {
        return false;
    }
    m_Reader.ReadDspAdpcmChannelInfo(adpcmParam, adpcmLoopParam, channelIndex);
    return true;
}

bool StreamSoundFileLoader::ReadAdpcBlockData(u16* yn1, u16* yn2, int blockIndex, int channelCount)
{
    if (!m_Reader.IsAvailable()) 
    {
        return false;
    }

    s32 readOffset = static_cast<s32>(
        m_Reader.GetSeekBlockOffset() +
        sizeof(ut::BinaryBlockHeader) +
        sizeof(s16) * 2 * channelCount * blockIndex
    );
    m_Stream.Seek(readOffset, io::FILE_STREAM_SEEK_BEGIN);

    u32 readDataSize = sizeof(s16) * 2 * channelCount;
    NW_ASSERT(readDataSize <= 32);

    NW_ALIGN(32) static u16 buffer[16];
    int readSize = m_Stream.Read(buffer, sizeof(buffer));
    if (readSize != sizeof(buffer)) 
    {
        return false;
    }

    for (int i = 0; i < channelCount; i++)
    {
        yn1[i] = buffer[i*2];
        yn2[i] = buffer[i*2+1];
    }

    return true;
}

} // namespace internal
} // namespace snd
} // namespace nw