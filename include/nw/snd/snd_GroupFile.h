#ifndef NW_SND_GROUP_FILE_H_
#define NW_SND_GROUP_FILE_H_

//!@date

#include <nw/snd/snd_Util.h>

namespace nw {
namespace snd {
namespace internal {

struct GroupFile
{
    struct InfoBlock;
    struct FileBlock;
    struct InfoExBlock;
    struct InfoBlockBody;
    struct FileBlockBody;
    struct InfoExBlockBody;
    struct GroupItemInfo;
    struct GroupItemInfoEx;

    struct FileHeader : public Util::SoundFileHeader
    {
        const InfoBlock* GetInfoBlock() const;
        const FileBlock* GetFileBlock() const;
        const InfoExBlock* GetInfoExBlock() const;
    };

    struct InfoBlockBody
    {
        Util::ReferenceTable referenceTableOfGroupItemInfo;

        u32 GetGroupItemInfoCount() const
        {
            return referenceTableOfGroupItemInfo.count;
        }

        const GroupItemInfo* GetGroupItemInfo(u32 index) const
        {
            if (index >= GetGroupItemInfoCount())
            {
                return NULL;
            }
            return static_cast<const GroupItemInfo*>(ut::AddOffsetToPtr(this, referenceTableOfGroupItemInfo.item[index].offset));
        }
    };

    struct InfoBlock
    {
        ut::BinaryBlockHeader   header;
        InfoBlockBody           body;
    };

    struct GroupItemInfo
    {
        ut::ResU32 fileId;
        Util::ReferenceWithSize embeddedItemInfo;

        static const u32 OFFSET_FOR_LINK    = 0xffffffff;
        static const u32 SIZE_FOR_LINK      = 0xffffffff;

        const void* GetFileLocation(const FileBlockBody* fileBlockBody) const
        {
            if (embeddedItemInfo.offset == OFFSET_FOR_LINK)
            {
                return NULL;
            }
            return ut::AddOffsetToPtr(fileBlockBody, embeddedItemInfo.offset);
        }
    };

    struct FileBlockBody
    {
        /* This struct contains your love life. */
    };

    struct FileBlock
    {
        ut::BinaryBlockHeader   header;
        FileBlockBody           body;
    };


    struct InfoExBlockBody
    {
        Util::ReferenceTable referenceTableOfGroupItemInfoEx;

        u32 GetGroupItemInfoExCount() const
        {
            return referenceTableOfGroupItemInfoEx.count;
        }
        const GroupItemInfoEx* GetGroupItemInfoEx(u32 index) const
        {
            if (index >= GetGroupItemInfoExCount())
            {
                return NULL;
            }
            return static_cast<const GroupItemInfoEx*>(ut::AddOffsetToPtr(this, referenceTableOfGroupItemInfoEx.item[index].offset));
        }
    };

    struct InfoExBlock
    {
        ut::BinaryBlockHeader   header;
        InfoExBlockBody         body;
    };

    struct GroupItemInfoEx
    {
        ut::ResU32 itemId;
        ut::ResU32 loadFlag;
    };
};


} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_GROUP_FILE_H_