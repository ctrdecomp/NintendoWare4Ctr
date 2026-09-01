#ifndef NW_SND_SOUND_HEAP_H_
#define NW_SND_SOUND_HEAP_H_

#include <nn/os.h>
#include <nw/snd/snd_FrameHeap.h>
#include <nw/snd/snd_SoundMemoryAllocatable.h>

namespace nw {
namespace snd {

class SoundArchive;
class SoundDataManager;

class SoundHeap : public SoundMemoryAllocatable
{
public:
    typedef void (*DisposeCallback)(void* mem, unsigned long size, void* userArg);

public:
    SoundHeap();
    virtual ~SoundHeap();
    virtual void* Alloc(size_t size);

    void* Alloc(size_t size, DisposeCallback callback, void* callbackArg);
    bool Create(void* startAddress, size_t size);
    void Destroy();
    void Clear();

    bool IsValid() const { return m_FrameHeap.IsValid(); }

    int SaveState();
    void LoadState(int level);

    int GetCurrentLevel() const
    {
        nn::os::CriticalSection::ScopedLock locker(m_CriticalSection);
        return m_FrameHeap.GetCurrentLevel();
    }

    size_t GetSize() const
    {
        nn::os::CriticalSection::ScopedLock locker(m_CriticalSection);
        return m_FrameHeap.GetSize();
    }

    size_t GetFreeSize() const
    {
        nn::os::CriticalSection::ScopedLock locker(m_CriticalSection);
        return m_FrameHeap.GetFreeSize();
    }

    void Dump(nw::snd::SoundDataManager& mgr, nw::snd::SoundArchive& arc) const
    {
        nn::os::CriticalSection::ScopedLock locker(m_CriticalSection);
        m_FrameHeap.Dump(mgr, arc);
    }

private:
    static void DisposeCallbackFunc(void* mem, unsigned long size, void* arg);

    mutable nn::os::CriticalSection m_CriticalSection;
    internal::FrameHeap m_FrameHeap;
};


} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_HEAP_H_
