#ifndef NW_SND_MML_PARSER_H_
#define NW_SND_MML_PARSER_H_

#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_SequenceTrack.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

class MmlSequenceTrack;

class MmlParser
{
public:
    virtual ~MmlParser() {}

    static u32 ParseAllocTrack(const void* baseAddress, u32 seqOffset, u32* allocTrack);
    
    static void EnablePrintVar(bool enble) { mPrintVarEnabledFlag = enble; }
    static bool IsEnabledPrintVar() { return mPrintVarEnabledFlag; }
protected:
    virtual void CommandProc(MmlSequenceTrack* track, u32 command, s32 commandArg1, s32 commandArg2) const;
    virtual void NoteOnCommandProc(MmlSequenceTrack* track, int key, int velocity, s32 length, bool tieFlag) const;

private:
    static bool mPrintVarEnabledFlag;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_MML_PARSER_H_
