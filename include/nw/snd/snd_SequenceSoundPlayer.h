#ifndef NW_SND_SEQUENCE_SOUND_PLAYER_H_
#define NW_SND_SEQUENCE_SOUND_PLAYER_H_

#include <nw/snd/snd_BasicSoundPlayer.h>
#include <nw/snd/snd_DisposeCallback.h>
#include <nw/snd/snd_SoundThread.h>
#include <nw/snd/snd_NoteOnCallback.h>
#include <nw/snd/snd_SoundArchive.h>
#include <nw/snd/snd_BankFileReader.h>
#include <nw/snd/snd_WaveArchiveFileReader.h>

namespace nw { 
namespace snd {

struct SequenceUserprocCallbackParam
{
    vs16* localVariable;
    vs16* globalVariable;
    vs16* trackVariable;
    bool cmpFlag;
};

typedef void (*SequenceUserprocCallback)(u16 procId, SequenceUserprocCallbackParam* param, void* arg);

namespace internal { 
namespace driver {

class SequenceTrack;
class SequenceTrackAllocator;

class SequenceSoundPlayer : public BasicSoundPlayer, public DisposeCallback, public SoundThread::PlayerCallback
{
public:
    static const int PLAYER_VARIABLE_NUM    = 16;
    static const int GLOBAL_VARIABLE_NUM    = 16;
    static const int TRACK_NUM_PER_PLAYER   = 16;
    static const int VARIABLE_DEFAULT_VALUE = -1;

    static const int DEFAULT_TIMEBASE       = 48;
    static const int DEFAULT_TEMPO          = 120;

    static const int MAX_SKIP_TICK_PER_FRAME = 48*4*4;

public:
    struct ParserPlayerParam
    {
        u8 volume;
        u8 priority;
        u8 timebase;
        u16 tempo;

        NoteOnCallback* callback;
    };

    enum OffsetType
    {
        OFFSET_TYPE_TICK,
        OFFSET_TYPE_MILLISEC
    };

public:
    static void InitSequenceSoundPlayer();

    static void SetSkipIntervalTick(s32 intervalTick);
    static s32 GetSkipIntervalTick();

    SequenceSoundPlayer();
    virtual ~SequenceSoundPlayer();
    
    virtual void Initialize();
    virtual void Finalize();
    
    void Setup(
        SequenceTrackAllocator* trackAllocator,
        u32 allocTracks,
        NoteOnCallback* callback
    );
    void Prepare(
        const void* seqBase,
        const void* bankFiles[],
        const void* warcFiles[],
        const bool warcIsIndividuals[],
        s32 seqOffset
    );

    virtual void Start();
    virtual void Stop();
    virtual void Pause(bool flag);
    void Skip(OffsetType offsetType, int offset);

    Channel* NoteOn(
        u8 bankIndex,
        const NoteOnInfo& noteOnInfo
    );

    void SetSequenceUserprocCallback(SequenceUserprocCallback callback, void* arg);
    void CallSequenceUserprocCallback(u16 procId, SequenceTrack* track);

    void SetTempoRatio(f32 tempoRatio);
    void SetPanRange(f32 panRange);
    void SetChannelPriority(int priority);
    void SetReleasePriorityFix(bool fix);

    f32 GetTempoRatio() const { return m_TempoRatio; }
    f32 GetPanRange() const { return m_PanRange; }
    int GetChannelPriority() const { return m_ParserParam.priority; }

    bool IsReleasePriorityFix() const { return m_ReleasePriorityFixFlag; }

    void SetTrackMute(u32 trackBitFlag, SeqMute mute);
    void SetTrackSilence(unsigned long trackBitFlag, bool silenceFlag, int fadeTimes);
    void SetTrackVolume(u32 trackBitFlag, f32 volume);
    void SetTrackPitch(u32 trackBitFlag, f32 pitch);
    void SetTrackPanRange(u32 trackBitFlag, f32 panRange);
    void SetTrackLpfFreq(u32 trackBitFlag, f32 lpfFreq);
    void SetTrackBiquadFilter(u32 trackBitFlag, int type, f32 value);
    void SetTrackModDepth(u32 trackBitFlag, f32 depth);
    void SetTrackModSpeed(u32 trackBitFlag, f32 speed);
    bool SetTrackBankIndex(u32 trackBitFlag, int bankIndex);
    void SetTrackTranspose(u32 trackBitFlag, s8 transpose);
    void SetTrackVelocityRange(u32 trackBitFlag, u8 range);
    void SetTrackOutputLine(u32 trackBitFlag, u32 outputLine);
    void SetSeqData(const void* seqBase, s32 seqOffset);
    void SetBankData(const void* bankFiles[], u32 bankFileCount);
    void SetTrackPan(u32 trackBitFlag, float pan);
    void SetTrackSurroundPan(u32 trackBitFlag, float span);
    void ResetTrackOutputLine(u32 trackBitFlag);

    s16 GetLocalVariable(int varNo) const;
    static s16 GetGlobalVariable(int varNo);
    void SetLocalVariable(int varNo, s16 var);
    static void SetGlobalVariable(int varNo, s16 var);

    vs16* GetVariablePtr(int varNo);

    virtual void InvalidateData(const void* start, const void* end);

    const ParserPlayerParam& GetParserPlayerParam() const { return m_ParserParam; }
    ParserPlayerParam& GetParserPlayerParam() { return m_ParserParam; }
    u32 GetTickCounter() const { return m_TickCounter; }

    SequenceTrack* GetPlayerTrack(int trackNo);
    const SequenceTrack* GetPlayerTrack(int trackNo) const;
    void SetPlayerTrack(int trackNo, SequenceTrack* track);

    const SequenceTrackAllocator* GetTrackAllocator() { return m_pSequenceTrackAllocator; }

    const void* GetBankFile(u8 bankIndex) const { return m_pBankFiles[bankIndex]; }

    void Update();

    virtual void ChannelCallback(Channel* channel) { (void)channel; }

protected:
    virtual void OnUpdateFrameSoundThread() { Update(); }
    virtual void OnShutdownSoundThread() { Stop(); }

private:
    template <typename T>
    void SetTrackParam(u32 trackBitFlag, void (SequenceTrack::*func)(T), T param);
    template <typename T, typename U>
    void SetTrackParam(u32 trackBitFlag, void (SequenceTrack::*func)(T, U), T param1, U param2);

    int  ParseNextTick(bool doNoteOn);

    void UpdateChannelParam();
    void UpdateTick();
    void SkipTick();

    void CloseTrack(int trackNo);
    void FinishPlayer();
    
    f32 CalcTickPerMinute() const { return m_ParserParam.timebase * m_ParserParam.tempo * m_TempoRatio; }
    f32 CalcTickPerMsec() const { return CalcTickPerMinute() / (60 * 1000.0f); }
    
    static vs16 m_GlobalVariable[GLOBAL_VARIABLE_NUM];

    bool m_ReleasePriorityFixFlag;

    f32 m_PanRange;
    f32 m_TempoRatio;
    f32 m_TickFraction;
    u32 m_SkipTickCounter;
    f32 m_SkipTimeCounter;

    ParserPlayerParam m_ParserParam;
    SequenceTrackAllocator* m_pSequenceTrackAllocator;

    SequenceUserprocCallback m_SequenceUserprocCallback;
    void* m_pSequenceUserprocCallbackArg;

    SequenceTrack* m_pTracks[TRACK_NUM_PER_PLAYER];

    vs16 m_LocalVariable[PLAYER_VARIABLE_NUM];
    vu32 m_TickCounter;

    const void* m_pBankFiles[SoundArchive::SEQ_BANK_MAX];
};

template <typename T>
void SequenceSoundPlayer::SetTrackParam(u32 trackBitFlag, void (SequenceTrack::*func)(T), T param)
{
    for (int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER && trackBitFlag != 0 ; trackNo++, trackBitFlag >>= 1)
    {
        if ((trackBitFlag & 1) == 0)
        {
            continue;
        }
        SequenceTrack* track = GetPlayerTrack(trackNo);
        if (track != NULL)
        {
            (track->*func)(param);
        }
    }
}

template <typename T, typename U>
void SequenceSoundPlayer::SetTrackParam(u32 trackBitFlag, void (SequenceTrack::*func)(T, U), T param1, U param2)
{
    for (int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER && trackBitFlag != 0; trackNo++, trackBitFlag >>= 1)
    {
        if ((trackBitFlag & 1) == 0)
        {
            continue;
        }
        SequenceTrack* track = GetPlayerTrack(trackNo);
        if (track != NULL)
        {
            (track->*func)(param1, param2);
        }
    }
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_SEQUENCE_SOUND_PLAYER_H_
