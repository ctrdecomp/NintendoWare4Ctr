// Filename: snd_WaveSoundFileReader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_WaveSoundFileReader.h>
#include <nw/snd/snd_WaveFile.h>

namespace nw {
namespace snd {
namespace internal {
namespace
{

const u32 SUPPORTED_FILE_VERSION = 0x01000000;
const u32 CURRENT_FILE_VERSION = 0x01000100;

const u32 SIGNATURE_INFO_BLOCK  = NW_UT_MAKE_SIGWORD('I', 'N', 'F', 'O');

bool IsValidFileHeader(const void* bankFile)
{
    const ut::BinaryFileHeader* header = reinterpret_cast<const ut::BinaryFileHeader*>(bankFile);

    if (header->signature != WaveSoundFileReader::SIGNATURE_FILE) 
    {
        return false;
    }

    if (header->version < SUPPORTED_FILE_VERSION) 
    {
        return false;
    }

    if (header->version > CURRENT_FILE_VERSION) 
    {
        return false;
    }

    return true;
}

} // namespace

WaveSoundFileReader::WaveSoundFileReader(const void* waveSoundFile):
    m_pHeader(NULL),
    m_pInfoBlockBody(NULL)
{
    NW_NULL_ASSERT(waveSoundFile);

    if (!IsValidFileHeader(waveSoundFile))
    {
        return;
    }

    m_pHeader = reinterpret_cast<const WaveSoundFile::FileHeader*>(waveSoundFile);

    const WaveSoundFile::InfoBlock* infoBlock = m_pHeader->GetInfoBlock();
    if (infoBlock == NULL)
    {
        return;
    }
    m_pInfoBlockBody = &infoBlock->body;
}

bool WaveSoundFileReader::ReadNoteInfo(WaveSoundNoteInfo* dst, u32 index, u32 noteIndex) const
{
    const WaveSoundFile::NoteInfo& src = m_pInfoBlockBody->GetWaveSoundData(index).GetNoteInfo(noteIndex);
    const Util::WaveId* pWaveId = m_pInfoBlockBody->GetWaveIdTable().GetWaveId(src.waveIdTableIndex);

    if (pWaveId == NULL)
    {
        return false;
    }
    
    dst->waveArchiveId = pWaveId->waveArchiveId;
    dst->waveIndex = pWaveId->waveIndex;
    dst->pitch = src.GetPitch();
    dst->adshr = src.GetAdshrCurve();
    dst->originalKey = src.GetOriginalKey();
    dst->pan = src.GetPan();
    dst->surroundPan = src.GetSurroundPan();
    dst->volume = src.GetVolume();
    return true;
}

bool WaveSoundFileReader::ReadWaveSoundInfo(WaveSoundInfo* dst, u32 index) const
{
    NW_NULL_ASSERT(m_pInfoBlockBody)
    NW_NULL_ASSERT(dst);
    const WaveSoundFile::WaveSoundInfo& src = m_pInfoBlockBody->GetWaveSoundData(index).GetWaveSoundInfo();

    dst->pitch = src.GetPitch();
    dst->pan = src.GetPan();
    dst->surroundPan = src.GetSurroundPan();
    src.GetSendValue(&dst->mainSend, dst->fxSend, AUX_BUS_NUM); 
    dst->adshr = src.GetAdshrCurve();

    return true;
}

} // namespace internal
} // namespace snd
} // namespace nw