#ifndef NW_SND_MML_SEQUENCE_TRACK_ALLOCATOR_H_
#define NW_SND_MML_SEQUENCE_TRACK_ALLOCATOR_H_

#include <nw/snd/snd_SequenceTrackAllocator.h>
#include <nw/snd/snd_MmlSequenceTrack.h>
#include <nw/snd/snd_InstancePool.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

class MmlParser;

class MmlSequenceTrackAllocator : public SequenceTrackAllocator
{
    typedef InstancePool<MmlSequenceTrack> MmlSequenceTrackPool;

public:
    explicit MmlSequenceTrackAllocator(MmlParser* parser) : m_pParser(parser) {}
    void SetMmlParser(MmlParser* parser) { m_pParser = parser; }
    MmlParser* GetMmlParser() { return m_pParser; }
    virtual SequenceTrack* AllocTrack(SequenceSoundPlayer* player);
    virtual void FreeTrack(SequenceTrack* track);
    virtual int GetAllocatableTrackCount() const { return m_TrackPool.Count(); }

    unsigned long Create(void* buffer, unsigned long size);
    void Destroy();

private:
    MmlParser* m_pParser;
    MmlSequenceTrackPool m_TrackPool;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_MML_SEQUENCE_TRACK_ALLOCATOR_H_
