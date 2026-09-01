#ifndef NW_SND_NOTE_ON_CALLBACK_H_
#define NW_SND_NOTE_ON_CALLBACK_H_

#include <nw/snd/snd_Channel.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

class SequenceSoundPlayer;

struct NoteOnInfo
{
    int prgNo;
    int key;
    int velocity;
    int length;
    int initPan;
    int priority;
    Channel::ChannelCallback channelCallback;
    u32 channelCallbackData;
};


class NoteOnCallback
{
public:
    virtual ~NoteOnCallback() {}

    virtual Channel* NoteOn(SequenceSoundPlayer* sequenceSoundPlayer, u8 bankIndex, const NoteOnInfo& noteOnInfo) = 0;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_NOTE_ON_CALLBACK_H_
