// Filename: snd_StreamSoundFileReader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_StreamSoundFileReader.h>
#include <nw/snd/snd_WaveFile.h>

namespace nw {
namespace snd {
namespace internal {

namespace
{
const u32 SIGNATURE_FILE = NW_UT_MAKE_SIGWORD('C', 'S', 'T', 'M');
const u32 SIGNATURE_INFO_BLOCK = NW_UT_MAKE_SIGWORD('I', 'N', 'F', 'O');
const u32 SIGNATURE_SEEK_BLOCK = NW_UT_MAKE_SIGWORD('S', 'E', 'E', 'K');
const u32 SIGNATURE_DATA_BLOCK = NW_UT_MAKE_SIGWORD('D', 'A', 'T', 'A');

const int SUPPORTED_FILE_VERSION = 0x02000000;
const int CURRENT_FILE_VERSION = 0x02000000;

} // namespace

StreamSoundFileReader::StreamSoundFileReader(): 
    m_pHeader(NULL),
    m_pInfoBlockBody(NULL)
{
}

void StreamSoundFileReader::Initialize(const void* streamSoundFile)
{
    NW_NULL_ASSERT(streamSoundFile);

    if (!IsValidFileHeader(streamSoundFile)) 
    {
        return;
    }

    m_pHeader = reinterpret_cast<const StreamSoundFile::FileHeader*>(streamSoundFile);
    const StreamSoundFile::InfoBlock* infoBlock = m_pHeader->GetInfoBlock();
    NW_ASSERT(infoBlock->header.kind == SIGNATURE_INFO_BLOCK);

    m_pInfoBlockBody = &infoBlock->body;
    NW_ASSERT(m_pInfoBlockBody->GetStreamSoundInfo()->oneBlockBytes % 32 == 0);
}

void StreamSoundFileReader::Finalize()
{
    m_pHeader = NULL;
    m_pInfoBlockBody = NULL;
}

bool StreamSoundFileReader::ReadStreamSoundInfo(
    StreamSoundFile::StreamSoundInfo* strmInfo) const
{
    NW_NULL_ASSERT(m_pInfoBlockBody);
    const StreamSoundFile::StreamSoundInfo* info =
        m_pInfoBlockBody->GetStreamSoundInfo();

    NW_ALIGN32_ASSERT(info->oneBlockBytes);
    NW_ALIGN32_ASSERT(info->lastBlockPaddedBytes);

    *strmInfo = *info;

    return true;
}

bool StreamSoundFileReader::ReadStreamTrackInfo(
    TrackInfo* pTrackInfo, int trackIndex) const
{
    NW_NULL_ASSERT(m_pInfoBlockBody);
    NW_NULL_ASSERT(pTrackInfo);
    const StreamSoundFile::TrackInfoTable* table =
        m_pInfoBlockBody->GetTrackInfoTable();

    if (trackIndex >= static_cast<int>( table->GetTrackCount()))
    {
        return false;
    }

    const StreamSoundFile::TrackInfo* src = table->GetTrackInfo( trackIndex );
    pTrackInfo->volume = src->volume;
    pTrackInfo->pan = src->pan;
    pTrackInfo->channelCount = static_cast<u8>(src->GetTrackChannelCount());

    u32 count = ut::Min(static_cast<int>(pTrackInfo->channelCount), WAVE_CHANNEL_MAX);
    for (u32 i = 0; i < count; i++)
    {
        pTrackInfo->globalChannelIndex[ i ] = src->GetGlobalChannelIndex(i);
    }

    return true;
}

bool StreamSoundFileReader::ReadDspAdpcmChannelInfo(
    DspAdpcmParam* pParam, DspAdpcmLoopParam* pLoopParam,
    int channelIndex) const
{
    NW_NULL_ASSERT(m_pInfoBlockBody);
    NW_NULL_ASSERT(pParam);
    NW_NULL_ASSERT(pLoopParam);

    const StreamSoundFile::DspAdpcmChannelInfo* src =
        m_pInfoBlockBody->
        GetChannelInfoTable()->
        GetChannelInfo(channelIndex)->
        GetDspAdpcmChannelInfo();
    if (src == NULL) 
    {
        return false;
    }

    *pParam = src->param;
    *pLoopParam = src->loopParam;
    return true;
}

bool StreamSoundFileReader::IsValidFileHeader(const void* streamSoundFile) const
{
    NW_NULL_ASSERT(streamSoundFile);

    const ut::BinaryFileHeader& header =
        *reinterpret_cast<const ut::BinaryFileHeader*>( streamSoundFile );

    NW_ASSERTMSG(header.signature == SIGNATURE_FILE, "invalid file signature. stream data is not available.");

    if (header.signature != SIGNATURE_FILE) 
    {
        return false;
    }

    NW_ASSERTMSG(
        header.version >= SUPPORTED_FILE_VERSION,
        "stream file is not supported version.\n"
        "please reconvert file using new version tools.\n"
        "(SUPPORTED_FILE_VERSION:0x%08x >= your version:0x%08x)\n",
        SUPPORTED_FILE_VERSION, header.version);

    if (header.version < SUPPORTED_FILE_VERSION) 
    {
        return false;
    }

    NW_ASSERTMSG(
        header.version <= CURRENT_FILE_VERSION,
        "stream file is not supported version.\n"
        "please reconvert file using new version tools.\n"
        "(CURRENT_FILE_VERSION:0x%08x <= your version:0x%08x)\n",
            CURRENT_FILE_VERSION, header.version);

    if (header.version > CURRENT_FILE_VERSION) 
    {
        return false;
    }

    return true;
}

} // namespace internal
} // namespace snd
} // namespace nw