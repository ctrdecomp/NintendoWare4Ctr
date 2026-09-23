// Filename: snd_WaveFileReader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_WaveFileReader.h>

namespace nw {
namespace snd {
namespace internal {

namespace {

const u32 SIGNATURE_INFO_BLOCK = NW_UT_MAKE_SIGWORD('I', 'N', 'F', 'O');
const u32 SIGNATURE_DATA_BLOCK = NW_UT_MAKE_SIGWORD('D', 'A', 'T', 'A');

const u32 SUPPORTED_FILE_VERSION = 0x02000000;
const u32 CURRENT_FILE_VERSION = 0x02010000;

bool IsValidFileHeader(const void* waveFile)
{
    const ut::BinaryFileHeader& header = *reinterpret_cast<const ut::BinaryFileHeader*>(waveFile);

    NW_ASSERTMSG(header.signature == WaveFileReader::SIGNATURE_FILE, "invalid file signature.");
    if (header.signature != WaveFileReader::SIGNATURE_FILE)
    {
        return false;
    }

    NW_ASSERTMSG(
            header.version >= SUPPORTED_FILE_VERSION,
            "wave file is not supported version.\n"
            "please reconvert file using new version tools.\n"
            "(SUPPORTED_FILE_VERSION:0x%08x >= your version:0x%08x)\n",
            SUPPORTED_FILE_VERSION, header.version
    );
    if (header.version < SUPPORTED_FILE_VERSION)
    {
        return false;
    }
    NW_ASSERTMSG(
            header.version <= CURRENT_FILE_VERSION,
            "wave file is not supported version.\n"
            "please reconvert file using new version tools.\n"
            "(CURRENT_FILE_VERSION:0x%08x <= your version:0x%08x)\n",
            CURRENT_FILE_VERSION, header.version
    );
    if (header.version > CURRENT_FILE_VERSION)
    {
        return false;
    }
    return true;
}

} // namespace


SampleFormat WaveFileReader::GetSampleFormat(u8 format)
{
    switch (format)
    {
    case WaveFile::PCM8: 
        return SAMPLE_FORMAT_PCM_S8;
    case WaveFile::PCM16: 
        return SAMPLE_FORMAT_PCM_S16;
    case WaveFile::DSP_ADPCM: 
        return SAMPLE_FORMAT_DSP_ADPCM;
    default:
        NW_ASSERTMSG(false, "Unknown wave data format(%d)", format);
        return SAMPLE_FORMAT_DSP_ADPCM;
    }
}

WaveFileReader::WaveFileReader(const void* waveFile): 
    m_pInfoBlockBody(NULL)
{
    if (!IsValidFileHeader(waveFile))
    {
        return;
    }

    const WaveFile::FileHeader* header = reinterpret_cast<const WaveFile::FileHeader*>(waveFile);

    const WaveFile::InfoBlock* infoBlock = header->GetInfoBlock();
    const WaveFile::DataBlock* dataBlock = header->GetDataBlock();

    if (infoBlock == NULL)
    {
        return;
    }
    if (dataBlock == NULL)
    {
        return;
    }

    NW_ASSERT(infoBlock->header.kind == SIGNATURE_INFO_BLOCK);
    NW_ASSERT(dataBlock->header.kind == SIGNATURE_DATA_BLOCK);

    m_pInfoBlockBody = &infoBlock->body;
    m_pDataBlockBody = &dataBlock->byte;
}

bool WaveFileReader::ReadWaveInfo(WaveInfo* info, const void* waveDataOffsetOrigin) const
{
    NW_ASSERT(m_pInfoBlockBody);

    const SampleFormat format = GetSampleFormat( m_pInfoBlockBody->encoding );
    info->sampleFormat = format;
    info->channelCount = m_pInfoBlockBody->GetChannelCount();
    info->sampleRate = m_pInfoBlockBody->sampleRate;
    info->loopFlag = (m_pInfoBlockBody->isLoop == 1);
    info->loopStartFrame = m_pInfoBlockBody->loopStartFrame;
    info->loopEndFrame = m_pInfoBlockBody->loopEndFrame;

    for (s32 i = 0; i < m_pInfoBlockBody->GetChannelCount(); i++)
    {
        if (i >= WAVE_CHANNEL_MAX)
        {
            continue;
        }

        WaveInfo::ChannelParam& channelParam = info->channelParam[ i ];

        const WaveFile::ChannelInfo& channelInfo = m_pInfoBlockBody->GetChannelInfo( i );

        if (channelInfo.referToAdpcmInfo.offset != 0)
        {
            const WaveFile::DspAdpcmInfo& adpcmInfo = channelInfo.GetDspAdpcmInfo();
            channelParam.adpcmParam = adpcmInfo.adpcmParam;
            channelParam.adpcmLoopParam = adpcmInfo.adpcmLoopParam;
        }

        channelParam.dataAddress = GetWaveDataAddress(&channelInfo, waveDataOffsetOrigin);
    }

    return true;
}

const void* WaveFileReader::GetWaveDataAddress(const WaveFile::ChannelInfo* info,
    const void* waveDataOffsetOrigin ) const
{
    NW_NULL_ASSERT(m_pInfoBlockBody);
    NW_NULL_ASSERT(info);

#if 0
    const void* waveDataAddress = NULL;

    bool offsetIsDataBlock = (waveDataOffsetOrigin == NULL);
    
    if (waveDataOffsetOrigin == NULL) {
        waveDataOffsetOrigin = m_pInfoBlockBody;
    }

    switch(m_pInfoBlockBody->dataLocationType) 
    {
    case WaveFile::WAVE_DATA_LOCATION_OFFSET:
        waveDataAddress = ut::AddOffsetToPtr(waveDataOffsetOrigin, m_pInfoBlockBody->dataLocation);
        if (offsetIsDataBlock) {
            waveDataAddress = ut::AddOffsetToPtr(waveDataAddress, 8);
        }
        break;
    case WaveFile::WAVE_DATA_LOCATION_ADDRESS:
        waveDataAddress = reinterpret_cast<const void*>(m_pInfoBlockBody->dataLocation);
        break;
    default:
        return NULL;
    }

    waveDataAddress = ut::AddOffsetToPtr(waveDataAddress, waveChannelInfo->channelDataOffset);
    
    return waveDataAddress;
#endif

    return info->GetSamplesAddress(m_pDataBlockBody);
}


} // namespace internal
} // namespace snd
} // namespace nw