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
    static const int PAN_CENTER = 64;
    static const int SURROUND_PAN_CENTER = 64;
    static const int CALL_STACK_DEPTH   = 3;
    static const int TEMPO_MIN = 0;
    static const int TEMPO_MAX = 1023;
private:
    enum SeqArgType
    {
        SEQ_ARG_NONE,
        SEQ_ARG_U8,
        SEQ_ARG_S16,
        SEQ_ARG_VMIDI,
        SEQ_ARG_RANDOM,
        SEQ_ARG_VARIABLE
    };
public:
    SequenceTrack::ParseResult Parse(MmlSequenceTrack* track, bool doNoteOn) const;

    virtual ~MmlParser() {}

    static u32 ParseAllocTrack(const void* baseAddress, u32 seqOffset, u32* allocTrack);
    
    static void EnablePrintVar(bool enble) { mPrintVarEnabledFlag = enble; }
    static bool IsEnabledPrintVar() { return mPrintVarEnabledFlag; }
protected:
    virtual void CommandProc(MmlSequenceTrack* track, u32 command, s32 commandArg1, s32 commandArg2) const;
    virtual void NoteOnCommandProc(MmlSequenceTrack* track, int key, int velocity, s32 length, bool tieFlag) const;

private:
    static bool mPrintVarEnabledFlag;

    u8 ReadByte(const u8** ptr) const { return *(*ptr)++; }
    void UnreadByte( const u8** ptr) const { --(*ptr); }
    u16 Read16(const u8** ptr) const;
    u32 Read24(const u8** ptr) const;
    s32 ReadVar(const u8** ptr) const;
    s32 ReadArg(const u8** ptr, SequenceSoundPlayer* player, SequenceTrack* track, SeqArgType argType) const;
    vs16* GetVariablePtr(SequenceSoundPlayer* player, SequenceTrack* track, int varNo) const;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_MML_PARSER_H_
