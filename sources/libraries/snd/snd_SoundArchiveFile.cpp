// Filename: snd_SoundArchiveFile.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundArchiveFile.h>

#include <cstring>
#include <nw/ut/ut_Inlines.h>
#include <nw/snd/snd_ItemType.h>

#include <nn/os.h>

namespace nw {
namespace snd {
namespace internal {

namespace
{
const u32 DEFAULT_STRING_ID = SoundArchive::INVALID_ID;
const PanMode DEFAULT_PAN_MODE = PAN_MODE_DUAL;
const PanCurve DEFAULT_PAN_CURVE = PAN_CURVE_SQRT;
const u8 DEFAULT_PLAYER_PRIORITY = 64;
const u8 DEFAULT_CHANNEL_PRIORITY = 64;
const u8 DEFAULT_ACTOR_PLAYER_ID = 0;
const u8 DEFAULT_IS_RELEASE_PRIORITY_FIX = 0;
const bool DEFAULT_IS_FRONT_BYPASS = false;
const u32 DEFAULT_USER_PARAM = 0xffffffff;
const u32 DEFAULT_EXTRA_USER_PARAM_COUNT = 0;
const u32 DEFAULT_SEQ_START_OFFSET = 0;
const u32 DEFAULT_WARC_WAVE_COUNT = 0;
const u32 DEFAULT_PLAYER_HEAP_SIZE = 0;

enum SoundInfoBitFlag
{
    SOUND_INFO_STRING_ID = 0x00,
    SOUND_INFO_PAN_PARAM,
    SOUND_INFO_PLAYER_PARAM,

    SOUND_INFO_OFFSET_TO_3D_PARAM = 0x08,
    SOUND_INFO_OFFSET_TO_SEND_PARAM,
    SOUND_INFO_OFFSET_TO_MOD_PARAM,

    SOUND_INFO_OFFSET_TO_RVL_PARAM = 0x10,
    SOUND_INFO_OFFSET_TO_CTR_PARAM,

    SOUND_INFO_REFERENCE_TO_EXTRA_USER_PARAM_TABLE = 0x1e,
    SOUND_INFO_USER_PARAM = 0x1f
};

enum WaveSoundInfoBitFlag
{
    WAVE_SOUND_INFO_PRIORITY = 0x00
};

enum SequenceSoundInfoBitFlag
{
    SEQ_SOUND_INFO_START_OFFSET = 0x00,
    SEQ_SOUND_INFO_PRIORITY
};

enum BankInfoBitFlag
{
    BANK_INFO_STRING_ID = 0x00
};

enum PlayerInfoBitFlag
{
    PLAYER_INFO_STRING_ID = 0x00,
    PLAYER_INFO_HEAP_SIZE
};

enum SoundGroupInfoBitFlag
{
    SOUND_GROUP_INFO_STRING_ID = 0x00
};

enum GroupInfoBitFlag
{
    GROUP_INFO_STRING_ID = 0x00
};

enum WaveArchiveInfoBitFlag
{
    WAVE_ARCHIVE_INFO_STRING_ID = 0x00,
    WAVE_ARCHIVE_INFO_WAVE_COUNT
};

ItemType GetItemTypeEnum(u32 id)
{
    return static_cast<ItemType>(Util::GetItemType(id));
}
}

const Util::ReferenceWithSize*
SoundArchiveFile::FileHeader::GetReferenceBy(u16 typeId) const
{
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        if(toBlocks[i].typeId == typeId)
        {
            return &toBlocks[i];
        }
    }
    return NULL;
}

u32 SoundArchiveFile::FileHeader::GetStringBlockSize() const
{
    return GetReferenceBy(0x2000)->size;
}
u32 SoundArchiveFile::FileHeader::GetInfoBlockSize() const
{
    return GetReferenceBy(0x2001)->size;
}
u32 SoundArchiveFile::FileHeader::GetFileBlockSize() const
{
    return GetReferenceBy(0x2002)->size;
}

s32 SoundArchiveFile::FileHeader::GetStringBlockOffset() const
{
    return GetReferenceBy(0x2000)->offset;
}
s32 SoundArchiveFile::FileHeader::GetInfoBlockOffset() const
{
    return GetReferenceBy(0x2001)->offset;
}
s32 SoundArchiveFile::FileHeader::GetFileBlockOffset() const
{
    return GetReferenceBy(0x2002)->offset;
}

const void* 
SoundArchiveFile::StringBlockBody::GetSection(Sections section) const
{
    if(section > Sections_Max) return NULL;
    return ut::AddOffsetToPtr(this, toSection[section].offset);
}

const char*
SoundArchiveFile::StringBlockBody::GetString(SoundArchive::ItemId stringId) const
{
    if(stringId == SoundArchive::INVALID_ID)
    {
        return NULL;
    }
    const StringTable* table = GetStringTable();
    if(table == NULL) return NULL;

    return table->GetString(stringId);
}

u32 SoundArchiveFile::StringBlockBody::GetItemIdImpl(Sections section, const char* str) const
{
    const PatriciaTree* tree = GetPatriciaTree(section);
    const PatriciaTree::NodeData* nodeData = tree->GetNodeDataBy(str);
    if(nodeData == NULL)
    {
        return SoundArchive::INVALID_ID;
    }
    if(std::strcmp(str, GetString(nodeData->stringId)) == 0)
    {
        return nodeData->itemId;
    }
    return SoundArchive::INVALID_ID;
}

const SoundArchiveFile::PatriciaTree::NodeData*
SoundArchiveFile::PatriciaTree::GetNodeDataBy(const char* str, std::size_t len) const
{
    if(rootIdx >= nodeTable.count)
    {
        return NULL;
    }

    const Node* node = &nodeTable.item[rootIdx];
    if(len == 0)
    {
        len = std::strlen(str);
    }

    while((node->flags & Node::FLAG_LEAF) == 0)
    {
        const int pos = (node->bit >> 3);
        const int bit = (node->bit & 7);
        u32 nodeIdx;
        if(pos < static_cast<int>(len) && str[pos] & (1 << (7 - bit)))
        {
            nodeIdx = node->rightIdx;
        }
        else
        {
            nodeIdx = node->leftIdx;
        }
        node = &nodeTable.item[nodeIdx];
    }
    return &node->nodeData;
}

const SoundArchiveFile::SoundInfo*
SoundArchiveFile::InfoBlockBody::GetSoundInfo(SoundArchive::ItemId itemId) const
{
    if(GetItemTypeEnum(itemId) != ItemType_Sound)
    {
        return NULL;
    }

    u32 index = Util::GetItemIndex(itemId);
    const Util::ReferenceTable& table = GetSoundInfoReferenceTable();
    if(index >= table.count)
    {
        return NULL;
    }
    return reinterpret_cast<const SoundInfo*>(table.GetReferedItem(index));
}

const SoundArchiveFile::BankInfo*
SoundArchiveFile::InfoBlockBody::GetBankInfo(SoundArchive::ItemId itemId) const
{
    if(GetItemTypeEnum(itemId) != ItemType_Bank)
    {
        return NULL;
    }

    u32 index = Util::GetItemIndex(itemId);
    const Util::ReferenceTable& table = GetBankInfoReferenceTable();
    if(index >= table.count)
    {
        return NULL;
    }
    return reinterpret_cast<const BankInfo*>(table.GetReferedItem(index));
}

const SoundArchiveFile::PlayerInfo*
SoundArchiveFile::InfoBlockBody::GetPlayerInfo(SoundArchive::ItemId itemId) const
{
    if(GetItemTypeEnum(itemId) != ItemType_Player)
    {
        return NULL;
    }

    u32 index = Util::GetItemIndex(itemId);
    const Util::ReferenceTable& table = GetPlayerInfoReferenceTable();
    if(index >= table.count)
    {
        return NULL;
    }
    return reinterpret_cast<const PlayerInfo*>(table.GetReferedItem(index));
}

const SoundArchiveFile::SoundGroupInfo*
SoundArchiveFile::InfoBlockBody::GetSoundGroupInfo(SoundArchive::ItemId itemId) const
{
    if(GetItemTypeEnum(itemId) != ItemType_SoundGroup)
    {
        return NULL;
    }

    u32 index = Util::GetItemIndex(itemId);
    const Util::ReferenceTable& table = GetSoundGroupInfoReferenceTable();
    if(index >= table.count)
    {
        return NULL;
    }
    return reinterpret_cast<const SoundGroupInfo*>(table.GetReferedItem(index));
}

const SoundArchiveFile::GroupInfo*
SoundArchiveFile::InfoBlockBody::GetGroupInfo(SoundArchive::ItemId itemId) const
{
    if(GetItemTypeEnum(itemId) != ItemType_Group)
    {
        return NULL;
    }

    u32 index = Util::GetItemIndex(itemId);
    const Util::ReferenceTable& table = GetGroupInfoReferenceTable();
    if(index >= table.count)
    {
        return NULL;
    }
    return reinterpret_cast<const GroupInfo*>(table.GetReferedItem(index));
}

const SoundArchiveFile::WaveArchiveInfo*
SoundArchiveFile::InfoBlockBody::GetWaveArchiveInfo(SoundArchive::ItemId itemId) const
{
    if(GetItemTypeEnum(itemId) != ItemType_WaveArchive)
    {
        return NULL;
    }

    u32 index = Util::GetItemIndex(itemId);
    const Util::ReferenceTable& table = GetWaveArchiveInfoReferenceTable();
    if(index >= table.count)
    {
        return NULL;
    }
    return reinterpret_cast<const WaveArchiveInfo*>(table.GetReferedItem(index));
}

const SoundArchiveFile::FileInfo*
SoundArchiveFile::InfoBlockBody::GetFileInfo(SoundArchive::FileId itemId) const
{
    u32 index = Util::GetItemIndex(itemId);
    const Util::ReferenceTable& table = GetFileInfoReferenceTable();
    if(index >= table.count)
    {
        return NULL;
    }
    return reinterpret_cast<const FileInfo*>(table.GetReferedItem(index));
}

SoundArchive::FileId
SoundArchiveFile::InfoBlockBody::GetItemFileId(SoundArchive::ItemId id) const
{
    SoundArchive::FileId fileId = SoundArchive::INVALID_ID;

    switch(Util::GetItemType(id))
    {
    case ItemType_Sound:
        {
            const SoundInfo* info = GetSoundInfo(id);
            if(info != NULL)
            {
                fileId = info->fileId;
            }
        }
        break;
    case ItemType_Bank:
        {
            const BankInfo* info = GetBankInfo(id);
            if(info != NULL)
            {
                fileId = info->fileId;
            }
        }
        break;
    case ItemType_WaveArchive:
        {
            const WaveArchiveInfo* info = GetWaveArchiveInfo(id);
            if(info != NULL)
            {
                fileId = info->fileId;
            }
        }
        break;
    case ItemType_Group:
        {
            const GroupInfo* info = GetGroupInfo(id);
            if(info != NULL)
            {
                fileId = info->fileId;
            }
        }
        break;
    case ItemType_SoundGroup:
        {
            const SoundGroupInfo* info = GetSoundGroupInfo(id);
            if(info != NULL)
            {
                SoundArchive::ItemId soundId = info->startId;
                const SoundInfo* soundInfo = GetSoundInfo(soundId);
                if(soundInfo != NULL)
                {
                    fileId = soundInfo->fileId;
                }
            }
        }
        break;
    case ItemType_Player:
        break;
    }

    return fileId;
}

SoundArchive::StringId
SoundArchiveFile::InfoBlockBody::GetItemStringId(SoundArchive::ItemId id) const
{
    SoundArchive::FileId stringId = SoundArchive::INVALID_ID;

    switch(Util::GetItemType(id))
    {
    case ItemType_Sound:
        {
            const SoundInfo* info = GetSoundInfo(id);
            if(info != NULL)
            {
                stringId = info->GetStringId();
            }
        }
        break;
    case ItemType_Bank:
        {
            const BankInfo* info = GetBankInfo(id);
            if(info != NULL)
            {
                stringId = info->GetStringId();
            }
        }
        break;
    case ItemType_WaveArchive:
        {
            const WaveArchiveInfo* info = GetWaveArchiveInfo(id);
            if(info != NULL)
            {
                stringId = info->GetStringId();
            }
        }
        break;
    case ItemType_SoundGroup:
        {
            const SoundGroupInfo* info = GetSoundGroupInfo(id);
            if(info != NULL)
            {
                stringId = info->GetStringId();
            }
        }
        break;
    case ItemType_Group:
        {
            const GroupInfo* info = GetGroupInfo(id);
            if(info != NULL)
            {
                stringId = info->GetStringId();
            }
        }
        break;
    case ItemType_Player:
        {
            const PlayerInfo* info = GetPlayerInfo(id);
            if(info != NULL)
            {
                stringId = info->GetStringId();
            }
        }
        break;
    }

    return stringId;
}

const SoundArchiveFile::SoundArchivePlayerInfo*
SoundArchiveFile::InfoBlockBody::GetSoundArchivePlayerInfo() const
{
    return reinterpret_cast<const SoundArchivePlayerInfo*>(
        ut::AddOffsetToPtr(this, toSoundArchivePlayerInfo.offset));
}

const Util::ReferenceTable&
SoundArchiveFile::InfoBlockBody::GetSoundInfoReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(
        ut::AddOffsetToPtr(this, toSoundInfoReferenceTable.offset));
}

const Util::ReferenceTable&
SoundArchiveFile::InfoBlockBody::GetBankInfoReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(
        ut::AddOffsetToPtr(this, toBankInfoReferenceTable.offset));
}

const Util::ReferenceTable&
SoundArchiveFile::InfoBlockBody::GetPlayerInfoReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(
        ut::AddOffsetToPtr(this, toPlayerInfoReferenceTable.offset));
}

const Util::ReferenceTable&
SoundArchiveFile::InfoBlockBody::GetSoundGroupInfoReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(
        ut::AddOffsetToPtr(this, toSoundGroupInfoReferenceTable.offset));
}

const Util::ReferenceTable&
SoundArchiveFile::InfoBlockBody::GetGroupInfoReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(
        ut::AddOffsetToPtr(this, toGroupInfoReferenceTable.offset));
}

const Util::ReferenceTable&
SoundArchiveFile::InfoBlockBody::GetWaveArchiveInfoReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(
        ut::AddOffsetToPtr(this, toWaveArchiveInfoReferenceTable.offset));
}

const Util::ReferenceTable&
SoundArchiveFile::InfoBlockBody::GetFileInfoReferenceTable() const
{
    return *reinterpret_cast<const Util::ReferenceTable*>(
        ut::AddOffsetToPtr(this, toFileInfoReferenceTable.offset));
}

// TODO ADD ELEMENT STRUCT DEFINE

SoundArchive::SoundType SoundArchiveFile::SoundInfo::GetSoundType() const
{
    switch(toDetailSoundInfo.typeId)
    {
    case 8705:
        return SoundArchive::SOUND_TYPE_STRM;
    case 8706:
        return SoundArchive::SOUND_TYPE_WAVE;
    case 8707:
        return SoundArchive::SOUND_TYPE_SEQ;
    default:
        return SoundArchive::SOUND_TYPE_INVALID;
    }
}

const SoundArchiveFile::StreamSoundInfo&
SoundArchiveFile::SoundInfo::GetStreamSoundInfo() const
{
    NW_ASSERT(toDetailSoundInfo.typeId == 8705);

    return *reinterpret_cast<const StreamSoundInfo*>(
        ut::AddOffsetToPtr(this, toDetailSoundInfo.offset));
}

const SoundArchiveFile::WaveSoundInfo&
SoundArchiveFile::SoundInfo::GetWaveSoundInfo() const
{
    NW_ASSERT(toDetailSoundInfo.typeId == 8706);

    return *reinterpret_cast<const WaveSoundInfo*>(
        ut::AddOffsetToPtr(this, toDetailSoundInfo.offset));
}

const SoundArchiveFile::SequenceSoundInfo&
SoundArchiveFile::SoundInfo::GetSequenceSoundInfo() const
{
    NW_ASSERT(toDetailSoundInfo.typeId == 8707);

    return *reinterpret_cast<const SequenceSoundInfo*>(
        ut::AddOffsetToPtr(this, toDetailSoundInfo.offset));
}

const SoundArchiveFile::Sound3DInfo*
SoundArchiveFile::SoundInfo::GetSound3DInfo() const
{
    u32 offset;
    bool result = optionParameter.GetValue(&offset, SOUND_INFO_OFFSET_TO_3D_PARAM);
    if(result == false)
    {
        return NULL;
    }

    return reinterpret_cast<const Sound3DInfo*>(ut::AddOffsetToPtr(this, offset));
}

u32 SoundArchiveFile::SoundInfo::GetStringId() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SOUND_INFO_STRING_ID);
    if(result == false)
    {
        return DEFAULT_STRING_ID;
    }
    return value;
}

PanMode SoundArchiveFile::SoundInfo::GetPanMode() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SOUND_INFO_PAN_PARAM);
    if(result == false)
    {
        return DEFAULT_PAN_MODE;
    }
    return static_cast<PanMode>(Util::DevideBy8bit(value, 0));
}

PanCurve SoundArchiveFile::SoundInfo::GetPanCurve() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SOUND_INFO_PAN_PARAM);
    if(result == false)
    {
        return DEFAULT_PAN_CURVE;
    }
    return static_cast<PanCurve>(Util::DevideBy8bit(value, 1));
}

u8 SoundArchiveFile::SoundInfo::GetPlayerPriority() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SOUND_INFO_PLAYER_PARAM);
    if(result == false)
    {
        return DEFAULT_PLAYER_PRIORITY;
    }
    return Util::DevideBy8bit(value, 0);
}

u8 SoundArchiveFile::SoundInfo::GetActorPlayerId() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SOUND_INFO_PLAYER_PARAM);
    if(result == false)
    {
        return DEFAULT_ACTOR_PLAYER_ID;
    }
    return Util::DevideBy8bit(value, 1);
}

u32 SoundArchiveFile::SoundInfo::GetUserParam() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SOUND_INFO_USER_PARAM);
    if(result == false)
    {
        return DEFAULT_USER_PARAM;
    }
    return value;
}
bool SoundArchiveFile::SoundInfo::IsFrontBypass() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SOUND_INFO_OFFSET_TO_CTR_PARAM);
    if(result == false)
    {
        return DEFAULT_IS_FRONT_BYPASS;
    }
    return (value & (1 << 0));
}

u8 SoundArchiveFile::WaveSoundInfo::GetChannelPriority() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, WAVE_SOUND_INFO_PRIORITY);
    if(result == false)
    {
        return DEFAULT_CHANNEL_PRIORITY;
    }
    return Util::DevideBy8bit(value, 0);
}

u8 SoundArchiveFile::WaveSoundInfo::GetIsReleasePriorityFix() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, WAVE_SOUND_INFO_PRIORITY);
    if(result == false)
    {
        return DEFAULT_IS_RELEASE_PRIORITY_FIX;
    }
    return Util::DevideBy8bit(value, 1);
}

const Util::Table<ut::ResU32>&
SoundArchiveFile::SequenceSoundInfo::GetBankIdTable() const
{
    return *reinterpret_cast<const Util::Table<ut::ResU32>*>(
        ut::AddOffsetToPtr(this, toBankIdTable.offset));
}
void SoundArchiveFile::SequenceSoundInfo::GetBankIds(u32* bankIds) const
{
    const Util::Table<ut::ResU32>& table = GetBankIdTable();
    for(u32 i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
    {
        if(i >= table.count)
        {
            bankIds[i] = SoundArchive::INVALID_ID;
        }
        else
        {
            bankIds[i] = table.item[i];
        }
    }
}
u32 SoundArchiveFile::SequenceSoundInfo::GetStartOffset() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SEQ_SOUND_INFO_START_OFFSET);
    if(result == false)
    {
        return DEFAULT_SEQ_START_OFFSET;
    }
    return value;
}
u8 SoundArchiveFile::SequenceSoundInfo::GetChannelPriority() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SEQ_SOUND_INFO_PRIORITY);
    if(result == false)
    {
        return DEFAULT_CHANNEL_PRIORITY;
    }
    return Util::DevideBy8bit(value, 0);
}
bool SoundArchiveFile::SequenceSoundInfo::IsReleasePriorityFix() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SEQ_SOUND_INFO_PRIORITY);
    if(result == false)
    {
        return DEFAULT_CHANNEL_PRIORITY;
    }
    if(Util::DevideBy8bit(value, 1) > 0) return true;
    return false;
}

u32 SoundArchiveFile::BankInfo::GetStringId() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, BANK_INFO_STRING_ID);
    if(result == false)
    {
        return DEFAULT_STRING_ID;
    }
    return value;
}

u32 SoundArchiveFile::PlayerInfo::GetStringId() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, PLAYER_INFO_STRING_ID);
    if(result == false)
    {
        return DEFAULT_STRING_ID;
    }
    return value;
}
u32 SoundArchiveFile::PlayerInfo::GetPlayerHeapSize() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, PLAYER_INFO_HEAP_SIZE);
    if(result == false)
    {
        return DEFAULT_PLAYER_HEAP_SIZE;
    }
    return value;
}

u32 SoundArchiveFile::SoundGroupInfo::GetStringId() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, SOUND_GROUP_INFO_STRING_ID);
    if(result == false)
    {
        return DEFAULT_STRING_ID;
    }
    return value;
}

u32 SoundArchiveFile::GroupInfo::GetStringId() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, GROUP_INFO_STRING_ID);
    if(result == false)
    {
        return DEFAULT_STRING_ID;
    }
    return value;
}

u32 SoundArchiveFile::WaveArchiveInfo::GetStringId() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, WAVE_ARCHIVE_INFO_STRING_ID);
    if(result == false)
    {
        return DEFAULT_STRING_ID;
    }
    return value;
}
u32 SoundArchiveFile::WaveArchiveInfo::GetWaveCount() const
{
    u32 value;
    bool result = optionParameter.GetValue(&value, WAVE_ARCHIVE_INFO_WAVE_COUNT);
    if(result == false)
    {
        return DEFAULT_WARC_WAVE_COUNT;
    }
    return value;
}

SoundArchiveFile::FileLocationType SoundArchiveFile::FileInfo::GetFileLocationType() const
{
    switch(toFileLocation.typeId)
    {
    case 8716:
        return FILE_LOCATION_TYPE_INTERNAL;
    case 8717:
        return FILE_LOCATION_TYPE_EXTERNAL;
    case 0:
        return FILE_LOCATION_TYPE_NONE;
    default:
        NW_ASSERTMSG(false, "invalid file location type");
        return FILE_LOCATION_TYPE_NONE;
    }
}

const SoundArchiveFile::InternalFileInfo*
SoundArchiveFile::FileInfo::GetInternalFileInfo() const
{
    if(GetFileLocationType() != FILE_LOCATION_TYPE_INTERNAL)
    {
        return NULL;
    }

    return reinterpret_cast<const InternalFileInfo*>(
        ut::AddOffsetToPtr(this, toFileLocation.offset));
}

const SoundArchiveFile::ExternalFileInfo*
SoundArchiveFile::FileInfo::GetExternalFileInfo() const
{
    if(GetFileLocationType() != FILE_LOCATION_TYPE_EXTERNAL)
    {
        return NULL;
    }

    return reinterpret_cast<const ExternalFileInfo*>(
        ut::AddOffsetToPtr(this, toFileLocation.offset));
}

} // namespace internal
} // namespace snd
} // namespace nw