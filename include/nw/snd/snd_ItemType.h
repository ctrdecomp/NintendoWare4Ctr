#ifndef NW_SND_ITEM_TYPE_H_
#define NW_SND_ITEM_TYPE_H_

namespace nw { 
namespace snd { 
namespace internal {

enum ItemType
{
    ItemType_Sound = 1,
    ItemType_SoundGroup,
    ItemType_Bank,
    ItemType_Player,
    ItemType_WaveArchive,
    ItemType_Group
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_ITEM_TYPE_H_
