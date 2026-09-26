// Filename: snd_MmlSequenceTrack.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_MmlSequenceTrack.h>
#include <nw/snd/snd_MmlParser.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

MmlSequenceTrack::MmlSequenceTrack()
{
}

SequenceTrack::ParseResult MmlSequenceTrack::Parse(bool doNoteOn)
{
    return m_pParser->Parse(this, doNoteOn);
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

