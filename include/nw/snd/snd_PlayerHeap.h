#ifndef NW_SND_PLAYER_HEAP_H_
#define NW_SND_PLAYER_HEAP_H_

#include <nw/snd/snd_SoundMemoryAllocatable.h>
#include <nw/ut/ut_LinkList.h>

namespace nw {
namespace snd {

class SoundPlayer; // im lazy

namespace internal {

class BasicSound; // im lazy

class PlayerHeap : public SoundMemoryAllocatable
{
public:
    PlayerHeap();
    virtual ~PlayerHeap();
    virtual void* Alloc(size_t size);

    void AttachSoundPlayer(SoundPlayer* player) { m_pPlayer = player; }

    void AttachSound(BasicSound* sound);
    void DetachSound(BasicSound* sound);

    bool Create(void* startAddress, size_t size);
    void Destroy();

    void Clear();

    bool IsValid() const { return m_pAllocAddress != NULL; }

    size_t GetFreeSize() const;

private:
    BasicSound* m_pSound;
    SoundPlayer* m_pPlayer;

    void* m_pStartAddress;
    void* m_pEndAddress;
    void* m_pAllocAddress;

public:
    ut::LinkListNode m_Link;
};


} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_PLAYER_HEAP_H_