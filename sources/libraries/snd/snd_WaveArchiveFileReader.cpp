// Filename: snd_WaveArchiveFileReader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_WaveArchiveFileReader.h>

namespace nw {
namespace snd {
namespace internal {

namespace
{

const u32 SIGNATURE_INFO_BLOCK = NW_UT_MAKE_SIGWORD('I', 'N', 'F', 'O');
const u32 SIGNATURE_FILE_BLOCK = NW_UT_MAKE_SIGWORD('F', 'I', 'L', 'E');

const u32 SUPPORTED_FILE_VERSION = 0x01000000;
const u32 CURRENT_FILE_VERSION   = 0x01000000;

bool IsValidFileHeader(const void* waveArchiveData)
{
    const ut::BinaryFileHeader& header = *reinterpret_cast<const ut::BinaryFileHeader*>(waveArchiveData);

    NW_ASSERTMSG(header.signature == WaveArchiveFileReader::SIGNATURE_FILE, "invalid file signature.");
    if (header.signature != WaveArchiveFileReader::SIGNATURE_FILE)
    {
        return false;
    }

    NW_ASSERTMSG(
            header.version >= SUPPORTED_FILE_VERSION,
            "wave archive file is not supported version.\n"
            "please reconvert file using new version tools.\n"
            "(SUPPORTED_FILE_VERSION:0x%08x >= your version:0x%08x)\n",
            SUPPORTED_FILE_VERSION, header.version);

    if (header.version < SUPPORTED_FILE_VERSION)
    {
        return false;
    }
    NW_ASSERTMSG(
            header.version <= CURRENT_FILE_VERSION,
            "wave archive file is not supported version.\n"
            "please reconvert file using new version tools.\n"
            "(CURRENT_FILE_VERSION:0x%08x <= your version:0x%08x)\n",
            CURRENT_FILE_VERSION, header.version);

    if (header.version > CURRENT_FILE_VERSION)
    {
        return false;
    }
    return true;
}

} // namespace

const u32 WaveArchiveFileReader::SIGNATURE_WARC_TABLE = NW_UT_MAKE_SIGWORD('C', 'W', 'A', 'T');

WaveArchiveFileReader::WaveArchiveFileReader(const void* pWaveArchiveFile, bool isIndividual): 
    m_pHeader(NULL),
    m_pInfoBlockBody(NULL),
    m_pLoadTable(NULL)
{
    NW_NULL_ASSERT(pWaveArchiveFile);

    if (!IsValidFileHeader(pWaveArchiveFile))
    {
        return;
    }

    m_pHeader = reinterpret_cast<const WaveArchiveFile::FileHeader*>(pWaveArchiveFile);
    m_pInfoBlockBody = &m_pHeader->GetInfoBlock()->body;

    if (isIndividual)
    {
        m_pLoadTable = reinterpret_cast<IndividualLoadTable*>(ut::AddOffsetToPtr(
            const_cast<void*>(pWaveArchiveFile),
            m_pHeader->GetFileBlockOffset() +
            sizeof(SIGNATURE_WARC_TABLE)));
    }
}

void WaveArchiveFileReader::InitializeFileTable()
{
    NW_NULL_ASSERT(m_pInfoBlockBody);

    for (u32 i = 0; i < GetWaveFileCount(); i++)
    {
        m_pLoadTable->waveFile[i] = NULL;
    }
}

u32 WaveArchiveFileReader::GetWaveFileCount() const
{
    NW_NULL_ASSERT(m_pInfoBlockBody);
    return m_pInfoBlockBody->GetWaveFileCount();
}

const void* WaveArchiveFileReader::GetWaveFile(u32 waveIndex) const
{
    NW_NULL_ASSERT(m_pInfoBlockBody);
    if (waveIndex >= GetWaveFileCount())
    {
        return NULL;
    }

    if (m_pLoadTable != NULL)
    {
        return GetWaveFileForIndividual(waveIndex);
    }
    return GetWaveFileForWhole(waveIndex);
}

u32 WaveArchiveFileReader::GetWaveFileSize(u32 waveIndex) const
{
    NW_NULL_ASSERT(m_pInfoBlockBody);
    return m_pInfoBlockBody->GetSize(waveIndex);
}

u32 WaveArchiveFileReader::GetWaveFileOffsetFromFileHead(u32 waveIndex) const
{
    NW_NULL_ASSERT(m_pInfoBlockBody);
    u32 result = + m_pHeader->GetFileBlockOffset()
        + offsetof(WaveArchiveFile::FileBlock, body)
        + m_pInfoBlockBody->GetOffsetFromFileBlockBody(waveIndex);
    return result;
}

const void* WaveArchiveFileReader::SetWaveFile(u32 waveIndex, const void* pWaveFile)
{
    if (m_pLoadTable == NULL)
    {
        return NULL;
    }
    if (waveIndex >= GetWaveFileCount())
    {
        return NULL;
    }

    const void* preAddress = GetWaveFileForIndividual(waveIndex);
    m_pLoadTable->waveFile[waveIndex] = pWaveFile;

    return preAddress;
}

bool WaveArchiveFileReader::HasIndividualLoadTable() const
{
    const u32* signature = reinterpret_cast<const u32*>(ut::AddOffsetToPtr(m_pHeader, m_pHeader->GetFileBlockOffset()));
    if (*signature == SIGNATURE_WARC_TABLE)
    {
        return true;
    }
    return false;
}

} // namespace internal
} // namespace snd
} // namespace nw