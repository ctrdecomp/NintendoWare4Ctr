#ifndef NW_SND_GROUP_FILE_READER_H_
#define NW_SND_GROUP_FILE_READER_H_

#include <nw/snd/snd_GroupFile.h>

namespace nw {
namespace snd {
namespace internal {

struct GroupItemLocationInfo
{
    u32 fileId;
    const void* address;
};

class GroupFileReader
{
public:
    static const u32 SIGNATURE_FILE = NW_UT_MAKE_SIGWORD('C', 'G', 'R', 'P');

    GroupFileReader(const void* groupFile);

    u32 GetGroupItemCount() const { return m_pInfoBlockBody->GetGroupItemInfoCount(); }
    bool ReadGroupItemLocationInfo( GroupItemLocationInfo* out, u32 index ) const;

    u32 GetGroupItemExCount() const;
    bool ReadGroupItemInfoEx(GroupFile::GroupItemInfoEx* out, u32 index) const;

private:
    const GroupFile::InfoBlockBody* m_pInfoBlockBody;
    const GroupFile::FileBlockBody* m_pFileBlockBody;
    const GroupFile::InfoExBlockBody* m_pInfoExBlockBody;
};

} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_GROUP_FILE_READER_H_

