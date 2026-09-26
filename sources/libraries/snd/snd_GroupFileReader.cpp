// Filename: snd_GroupFileReader.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_GroupFileReader.h>

namespace nw {
namespace snd {
namespace internal {
namespace 
{
const u32 SIGNATURE_INFO_BLOCK  = NW_UT_MAKE_SIGWORD( 'I', 'N', 'F', 'O' );
const u32 SIGNATURE_FILE_BLOCK  = NW_UT_MAKE_SIGWORD( 'F', 'I', 'L', 'E' );
const u32 SIGNATURE_INFOEX_BLOCK  = NW_UT_MAKE_SIGWORD( 'I', 'N', 'F', 'X' );

const u32 SUPPORTED_FILE_VERSION = 0x01000000;
const u32 CURRENT_FILE_VERSION   = 0x01010000;

bool IsValidFileHeader(const void* groupFile)
{
    const ut::BinaryFileHeader* header = reinterpret_cast<const ut::BinaryFileHeader*>(groupFile);

    NW_ASSERTMSG(header->signature == GroupFileReader::SIGNATURE_FILE, "invalid file signature. group file is not available.");
    if (header->signature != GroupFileReader::SIGNATURE_FILE)
    {
        return false;
    }

    NW_ASSERTMSG(header->version >= SUPPORTED_FILE_VERSION,
            "group file is not supported version.\n"
            "please reconvert file using new version tools.\n"
            "(SUPPORTED_FILE_VERSION:0x%08x >= your version:0x%08x)\n",
            SUPPORTED_FILE_VERSION, header->version);

    if (header->version < SUPPORTED_FILE_VERSION) 
        return false;

    NW_ASSERTMSG(header->version <= CURRENT_FILE_VERSION,
            "group file is not supported version.\n"
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

GroupFileReader::GroupFileReader(const void* groupFile): 
    m_pInfoBlockBody(NULL),
    m_pFileBlockBody(NULL),
    m_pInfoExBlockBody(NULL)
{
    NW_NULL_ASSERT(groupFile);

    if (!IsValidFileHeader(groupFile))
    {
        return;
    }

    const GroupFile::FileHeader* header = static_cast<const GroupFile::FileHeader*>(groupFile);

    const GroupFile::InfoBlock* infoBlock = header->GetInfoBlock();
    const GroupFile::FileBlock* fileBlock = header->GetFileBlock();
    const GroupFile::InfoExBlock* infoExBlock = header->GetInfoExBlock();

    if (infoBlock == NULL)
    {
        return;
    }
    if (fileBlock == NULL)
    {
        return;
    }

    NW_ASSERT(infoBlock->header.kind == SIGNATURE_INFO_BLOCK);
    NW_ASSERT(fileBlock->header.kind == SIGNATURE_FILE_BLOCK);

    m_pInfoBlockBody = &(infoBlock->body);
    m_pFileBlockBody = &(fileBlock->body);

    if (infoExBlock != NULL)
    {
        NW_ASSERT(infoExBlock->header.kind == SIGNATURE_INFOEX_BLOCK);
        m_pInfoExBlockBody = &(infoExBlock->body);
    }
}

bool GroupFileReader::ReadGroupItemLocationInfo(GroupItemLocationInfo* out, u32 index) const
{
    if (m_pInfoBlockBody == NULL)
    {
        return false;
    }

    const GroupFile::GroupItemInfo* groupItemInfo = m_pInfoBlockBody->GetGroupItemInfo(index);
    if (groupItemInfo == NULL)
    {
        return false;
    }

    out->fileId = groupItemInfo->fileId;
    out->address = groupItemInfo->GetFileLocation(m_pFileBlockBody);
    return true;
}

u32 GroupFileReader::GetGroupItemExCount() const
{
    if (m_pInfoExBlockBody == NULL)
    {
        return 0;
    }

    return m_pInfoExBlockBody->GetGroupItemInfoExCount();
}

bool GroupFileReader::ReadGroupItemInfoEx(GroupFile::GroupItemInfoEx* out, u32 index) const
{
    if (m_pInfoExBlockBody == NULL)
    {
        return false;
    }

    const GroupFile::GroupItemInfoEx* groupItemInfoEx = m_pInfoExBlockBody->GetGroupItemInfoEx(index);
    if (groupItemInfoEx == NULL)
    {
        return false;
    }

    *out = *groupItemInfoEx;
    return true;
}

} // namespace internal
} // namespace snd
} // namespace nw