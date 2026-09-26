// Filename: snd_SequenceSoundFileReader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SequenceSoundFileReader.h>

namespace nw {
namespace snd {
namespace internal {

namespace
{

const u32 SIGNATURE_DATA_BLOCK  = NW_UT_MAKE_SIGWORD('D', 'A', 'T', 'A');
const u32 SIGNATURE_LABEL_BLOCK = NW_UT_MAKE_SIGWORD('L', 'A', 'B', 'L');

const u32 SUPPORTED_FILE_VERSION = 0x01000000;
const u32 CURRENT_FILE_VERSION   = 0x01000000;

bool IsValidFileHeader( const void* sequenceFile)
{
    const ut::BinaryFileHeader* header =
        reinterpret_cast<const ut::BinaryFileHeader*>(sequenceFile);

    NW_ASSERTMSG(header->signature == SequenceSoundFileReader::SIGNATURE_FILE,
        "invalid file signature. sequence file is not available.");
    if (header->signature != SequenceSoundFileReader::SIGNATURE_FILE)
    {
        return false;
    }

    NW_ASSERTMSG(header->version >= SUPPORTED_FILE_VERSION,
            "sequence file is not supported version.\n"
            "please reconvert file using new version tools.\n"
            "(SUPPORTED_FILE_VERSION:0x%08x >= your version:0x%08x)\n",
            SUPPORTED_FILE_VERSION, header->version);
    if (header->version < SUPPORTED_FILE_VERSION) 
    {
        return false;
    }

    NW_ASSERTMSG(header->version <= CURRENT_FILE_VERSION,
            "sequence file is not supported version.\n"
            "please reconvert file using new version tools.\n"
            "(CURRENT_FILE_VERSION:0x%08x >= your version:0x%08x)\n",
            CURRENT_FILE_VERSION, header->version);
    if (header->version > CURRENT_FILE_VERSION) 
    {    
        return false;
    }

    return true;
}

} // namespace


SequenceSoundFileReader::SequenceSoundFileReader(const void* sequenceFile): 
    m_pHeader(NULL),
    m_pDataBlockBody(NULL),
    m_pLabelBlockBody(NULL)
{
    NW_NULL_ASSERT(sequenceFile);

    if (!IsValidFileHeader(sequenceFile))
    {
        return;
    }

    m_pHeader = reinterpret_cast<const SequenceSoundFile::FileHeader*>(sequenceFile);

    const SequenceSoundFile::DataBlock* dataBlock = m_pHeader->GetDataBlock();
    NW_NULL_ASSERT(dataBlock);
    NW_ASSERT(dataBlock->header.kind == SIGNATURE_DATA_BLOCK);
    m_pDataBlockBody = &dataBlock->body;

    const SequenceSoundFile::LabelBlock* labelBlock = m_pHeader->GetLabelBlock();
    NW_NULL_ASSERT(labelBlock);
    NW_ASSERT(labelBlock->header.kind == SIGNATURE_LABEL_BLOCK);
    m_pLabelBlockBody = &labelBlock->body;
}

const void* SequenceSoundFileReader::GetSequenceData() const
{
    NW_NULL_ASSERT(m_pDataBlockBody);
    return m_pDataBlockBody->GetSequenceData();
}

bool SequenceSoundFileReader::GetOffsetByLabel( const char* label, u32* offsetPtr ) const
{
    NW_NULL_ASSERT(m_pLabelBlockBody);
    return m_pLabelBlockBody->GetOffsetByLabel(label, offsetPtr);
}

const char* SequenceSoundFileReader::GetLabelByOffset(u32 offset) const
{
    NW_NULL_ASSERT(m_pLabelBlockBody);
    return m_pLabelBlockBody->GetLabelByOffset(offset);
}

} // namespace internal
} // namespace snd
} // namespace nw