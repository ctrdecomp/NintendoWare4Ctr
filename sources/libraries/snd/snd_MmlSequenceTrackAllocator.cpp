// Filename: snd_MmlSequenceTrackAllocator.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_MmlSequenceTrackAllocator.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

SequenceTrack* MmlSequenceTrackAllocator::AllocTrack(SequenceSoundPlayer* player)
{
    MmlSequenceTrack* track = m_TrackPool.Alloc();
    if (track != NULL)
    {
        track->SetSequenceSoundPlayer(player);
        track->SetMmlParser(m_pParser);
    }
    return track;
}

void MmlSequenceTrackAllocator::FreeTrack(SequenceTrack* track)
{
    NW_NULL_ASSERT(track);

    track->SetSequenceSoundPlayer(NULL);

    m_TrackPool.Free(reinterpret_cast<MmlSequenceTrack*>(track));
}

unsigned long MmlSequenceTrackAllocator::Create(void* buffer, unsigned long size)
{
    return m_TrackPool.Create(buffer, size);
}

void MmlSequenceTrackAllocator::Destroy(void* buffer, unsigned long size)
{
    m_TrackPool.Destroy(buffer, size);
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw