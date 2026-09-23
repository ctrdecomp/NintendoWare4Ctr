// Filename: snd_BankFileReader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_BankFileReader.h>

namespace nw {
namespace snd {
namespace internal {
namespace
{

const u32 SIGNATURE_INFO_BLOCK  = NW_UT_MAKE_SIGWORD('I', 'N', 'F', 'O');

const u32 SUPPORTED_FILE_VERSION = 0x01000000;
const u32 CURRENT_FILE_VERSION   = 0x01000100;

bool IsValidFileHeader(const void* bankFile)
{
    const ut::BinaryFileHeader* header = reinterpret_cast<const ut::BinaryFileHeader*>(bankFile);

    if (header->signature != BankFileReader::SIGNATURE_FILE) 
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

BankFileReader::BankFileReader(const void* bankFile):
    m_pHeader(NULL), m_pInfoBlockBody(NULL)
{
    if (!IsValidFileHeader(bankFile)) 
    {
        return;
    }
    m_pHeader = reinterpret_cast<const BankFile::FileHeader*>(bankFile);

    const BankFile::InfoBlock* infoBlock = m_pHeader->GetInfoBlock();
    m_pInfoBlockBody = &infoBlock->body;
}

bool BankFileReader::ReadVelocityRegionInfo(VelocityRegionInfo* info, int programNo, int key, int velocity) const
{
    if (programNo < 0 || programNo >= m_pInfoBlockBody->GetInstrumentCount())
    {
        return false;
    }

    const BankFile::Instrument* pInstrument = m_pInfoBlockBody->GetInstrument(programNo);
    if (pInstrument == NULL)
    {
        return false;
    }

    const BankFile::KeyRegion* pKeyRegion = pInstrument->GetKeyRegion(key);
    if (pKeyRegion == NULL)
    {
        return false;
    }

    const BankFile::VelocityRegion* pVelRegion = pKeyRegion->GetVelocityRegion(velocity);
    if (pVelRegion == NULL)
    {
        return false;
    }

    const Util::WaveId& waveId = m_pInfoBlockBody->GetWaveId(pVelRegion->waveIdTableIndex);

    if (waveId.waveIndex == 0xffffffff)
    {
        return false;
    }

    info->waveArchiveId  = waveId.waveArchiveId;
    info->waveIndex  = waveId.waveIndex;

    const BankFile::RegionParameter* pRegParameter = pVelRegion->GetRegionParameter();
    if (pRegParameter == NULL)
    {
        info->originalKey = pVelRegion->GetOriginalKey();
        info->volume = pVelRegion->GetVolume();
        info->pan = pVelRegion->GetPan();
        info->pitch = pVelRegion->GetPitch();
        info->isIgnoreNoteOff = pVelRegion->IsIgnoreNoteOff();
        info->keyGroup = pVelRegion->GetKeyGroup();
        info->interpolationType = pVelRegion->GetInterpolationType();
        info->adshrCurve = pVelRegion->GetAdshrCurve();
    }
    else
    {
        info->originalKey = pRegParameter->originalKey;
        info->volume = pRegParameter->volume;
        info->pan = pRegParameter->pan;
        info->pitch= pRegParameter->pitch;
        info->isIgnoreNoteOff = pRegParameter->isIgnoreNoteOff;
        info->keyGroup = pRegParameter->keyGroup;
        info->interpolationType = pRegParameter->interpolationType;
        info->adshrCurve = pRegParameter->adshrCurve;
    }

    return true;
}

const Util::WaveIdTable& BankFileReader::GetWaveIdTable() const
{
    return m_pInfoBlockBody->GetWaveIdTable();
}

} // namespace internal
} // namespace snd
} // namespace nw