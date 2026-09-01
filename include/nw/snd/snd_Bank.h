#ifndef NW_SND_BANK_H_
#define NW_SND_BANK_H_

#include <nw/snd/snd_NoteOnCallback.h>
#include <nw/snd/snd_Channel.h>
#include <nw/snd/snd_PlayerHeapDataManager.h>

namespace nw {
namespace snd {

class SoundArchive;
class SoundArchivePlayer;

namespace internal {
namespace driver {

struct Bank
{
    Bank()
    {
    }
    ~Bank()
    {
    }
    Channel* NoteOn(const void* bankFile, const NoteOnInfo& noteOnInfo, const SoundArchive& archive,
        const SoundArchivePlayer& player, const PlayerHeapDataManager* dataMgr) const;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_BANK_H_