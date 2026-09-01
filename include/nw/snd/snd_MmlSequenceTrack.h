#ifndef NW_SND_MML_SEQUENCE_TRACK_H_
#define NW_SND_MML_SEQUENCE_TRACK_H_

#include <nw/snd/snd_SequenceTrack.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

class MmlSequenceTrack : public SequenceTrack
{
public:
    MmlSequenceTrack();

    void SetMmlParser(const MmlParser* parser) { m_pParser = parser; }
    const MmlParser* GetMmlParser() const { return m_pParser; }

protected:
    virtual SequenceTrack::ParseResult Parse(bool doNoteOn);
    
private:
    const MmlParser* m_pParser;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_MML_SEQUENCE_TRACK_H_
