#ifndef NW_SND_SEQUENCE_TRACK_H_
#define NW_SND_SEQUENCE_TRACK_H_

#include <nw/snd/snd_Channel.h>
#include <nw/snd/snd_CurveLfo.h>
#include <nw/snd/snd_MoveValue.h>
#include <nw/snd/snd_Global.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {


class SequenceSoundPlayer;


class SequenceTrack
{
public:
    static const int CALL_STACK_DEPTH = 3;

    struct ParserTrackParam
    {
        const u8* baseAddr;
        const u8* currentAddr;

        bool cmpFlag;
        bool noteWaitFlag;
        bool tieFlag;
        bool monophonicFlag;

        struct CallStack
        {
            u8 loopFlag;
            u8 loopCount;
            const u8* address;
        };
        CallStack callStack[ CALL_STACK_DEPTH ];
        u8 callStackDepth;

        bool frontBypassFlag;

        s32 wait;

        bool muteFlag;
        bool silenceFlag;
        bool noteFinishWait;
        bool portaFlag;
        bool damperFlag;

        u8  bankIndex;
        int prgNo;

        CurveLfoParam lfoParam;
        u8 lfoTarget;
        f32 sweepPitch;

        MoveValue<u8,s16> volume;
        MoveValue<s8,s16> pan;
        MoveValue<s8,s16> surroundPan;
        MoveValue<s8,s16> pitchBend;
        u8 volume2;
        u8 velocityRange;
        u8 bendRange;
        s8 initPan;
        s8 transpose;
        u8 priority;
        u8 portaKey;
        u8 portaTime;
        u8 attack;
        u8 decay;
        u8 sustain;
        u8 release;
        s16 envHold;
        u8 mainSend;
        u8 fxSend[AUX_BUS_NUM];
        u8 biquadType;
        f32 lpfFreq;
        f32 biquadValue;
    };

    enum ParseResult
    {
        PARSE_RESULT_CONTINUE,
        PARSE_RESULT_FINISH
    };

public:
    static const int DEFAULT_PRIORITY   = 64;
    static const int DEFAULT_BENDRANGE  = 2;
    static const int DEFAULT_PORTA_KEY  = 60; /* cn4 */
    static const int INVALID_ENVELOPE   = 0xff;
    static const int MAX_ENVELOPE_VALUE = 0x7f;
    static const int PARSER_PARAM_SIZE  = 32;
    static const int TRACK_VARIABLE_NUM = 16;

    static const int PAUSE_RELEASE_VALUE    = 127;
    static const int MUTE_RELEASE_VALUE     = 127;

public:
    static void ChannelCallbackFunc(Channel* dropChannel, Channel::ChannelCallbackStatus status, u32 userData);

public:
    SequenceTrack();
    virtual ~SequenceTrack();

    void InitParam();
    void SetSeqData(const void* seqBase, s32 seqOffset);
    void Open();
    void Close();
    bool IsOpened() const { return m_OpenFlag; }

    int ParseNextTick( bool doNoteOn );
    void UpdateChannelLength();
    void UpdateChannelParam();

    Channel* NoteOn(int key, int velocity, s32 length, bool tieFlag);

    void StopAllChannel();
    void ReleaseAllChannel( int release );
    void FreeAllChannel();
    void PauseAllChannel( bool flag );

    int GetChannelCount() const;
    const ParserTrackParam& GetParserTrackParam() const { return m_ParserTrackParam; }
    ParserTrackParam& GetParserTrackParam() { return m_ParserTrackParam; }

    void SetMute(SeqMute mute);
    void SetSilence(bool silenceFlag, int fadeTimes);
    void SetVolume(float volume) { m_ExtVolume = volume; }
    void SetPitch(float pitch) { m_ExtPitch = pitch; }
    void SetPan(float pan) { m_ExtPan = pan; }
    void SetPanRange(float panRange) { m_PanRange = panRange; }
    void SetSurroundPan(float surroundPan) { m_ExtSurroundPan = surroundPan; }
    void SetLpfFreq(float lpfFreq) { m_ParserTrackParam.lpfFreq = lpfFreq; }
    void SetBiquadFilter(int type, float value);
    void SetModDepth(float depth) { m_ParserTrackParam.lfoParam.depth = depth; }
    void SetModSpeed(float speed) { m_ParserTrackParam.lfoParam.speed = speed; }
    void SetBankIndex(int bankIdex);

    float GetVolume() const { return m_ExtVolume; }
    float GetPitch() const { return m_ExtPitch; }
    float GetPan() const { return m_ExtPan; }
    float GetPanRange() const { return m_PanRange; }
    float GetLpfFreq() const { return m_ParserTrackParam.lpfFreq; }
    int GetBiquadType() const { return m_ParserTrackParam.biquadType; }
    float GetBiquadValue() const { return m_ParserTrackParam.biquadValue; }
    float GetSurroundPan() const { return m_ExtSurroundPan; }

    s16 GetTrackVariable(int varNo) const;
    void SetTrackVariable(int varNo, s16 var);
    vs16* GetVariablePtr(int varNo);

    void SetSequenceSoundPlayer(SequenceSoundPlayer* player) { m_pSequenceSoundPlayer = player; }
    const SequenceSoundPlayer* GetSequenceSoundPlayer() const { return m_pSequenceSoundPlayer; }
    SequenceSoundPlayer* GetSequenceSoundPlayer() { return m_pSequenceSoundPlayer; }

    void SetPlayerTrackNo(int playerTrackNo);
    u8 GetPlayerTrackNo() const { return m_PlayerTrackNo; }

    void UpdateChannelRelease(Channel* channel);

protected:
    virtual ParseResult Parse(bool doNoteOn) = 0;

private:
    Channel* GetLastChannel() const { return m_pChannelList; }
    void AddChannel(Channel* channel);

    u8 m_PlayerTrackNo;
    bool m_OpenFlag;
    f32 m_ExtVolume;
    f32 m_ExtPitch;
    f32 m_ExtPan;
    f32 m_ExtSurroundPan;
    f32 m_PanRange;

    ParserTrackParam m_ParserTrackParam;
    vs16 m_TrackVariable[ TRACK_VARIABLE_NUM ];

    SequenceSoundPlayer* m_pSequenceSoundPlayer;
    Channel* m_pChannelList;
};

} // namespace driver
} // namespace ninternal
} // namespace snd
} // namespace nw


#endif // NW_SND_SEQUENCE_TRACK_H_