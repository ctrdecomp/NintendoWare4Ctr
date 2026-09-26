// Filename: snd_SequenceSoundPlayer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SequenceSoundPlayer.h>

#include <nw/snd/snd_SequenceTrack.h>
#include <nw/snd/snd_SequenceTrackAllocator.h>
#include <nw/snd/snd_DisposeCallbackManager.h>
#include <nw/snd/snd_HardwareManager.h>
#include <nw/snd/snd_NoteOnCallback.h>
#include <nw/snd/snd_SoundThread.h>

namespace
{
const u32 INTERVAL_MSEC_NUMERATOR   = NN_SND_SAMPLES_PER_FRAME * 1000 * (32 * 256);
const u32 INTERVAL_MSEC_DENOMINATOR = NN_SND_HW_SYSTEM_CLOCK;
}

namespace nw {
namespace snd {
namespace internal {
namespace driver {

vs16 SequenceSoundPlayer::m_GlobalVariable[GLOBAL_VARIABLE_NUM];

SequenceSoundPlayer::SequenceSoundPlayer()
{
    for (int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER ; trackNo++)
    {
        m_pTracks[trackNo] = NULL;
    }
}

SequenceSoundPlayer::~SequenceSoundPlayer()
{
    Finalize();
}

void SequenceSoundPlayer::InitSequenceSoundPlayer()
{
    for(int variableNo = 0 ; variableNo < GLOBAL_VARIABLE_NUM ; variableNo++)
    {
        m_GlobalVariable[variableNo] = VARIABLE_DEFAULT_VALUE;
    }
}

void SequenceSoundPlayer::Initialize()
{
    BasicSoundPlayer::Initialize();

    m_StartedFlag = false;
    m_PauseFlag = false;
    m_ReleasePriorityFixFlag = false;
    
    m_TempoRatio = 1.0f;
    m_TickFraction = 0.0f;
    m_SkipTickCounter = 0;
    m_SkipTimeCounter = 0.0f;
    m_PanRange = 1.0f;
    m_TickCounter = 0;
    m_TickFraction = 0.0f;

    m_SequenceUserprocCallback = NULL;
    m_pSequenceUserprocCallbackArg = NULL;

    m_ParserParam.tempo = DEFAULT_TEMPO;
    m_ParserParam.timebase = DEFAULT_TIMEBASE;
    m_ParserParam.volume = 127;
    m_ParserParam.priority = 64;
    m_ParserParam.callback = NULL;

    for (int varNo = 0; varNo < PLAYER_VARIABLE_NUM ; varNo++)
    {
        m_LocalVariable[varNo] = VARIABLE_DEFAULT_VALUE;
    }

    for (int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER ; trackNo++)
    {
        m_pTracks[trackNo] = NULL;
    }
    
    for (int bankIdx = 0; bankIdx < SoundArchive::SEQ_BANK_MAX; bankIdx++)
    {
        m_pBankFiles[bankIdx] = NULL;
    }
}

void SequenceSoundPlayer::Finalize()
{
    m_FinishFlag = true;

    FinishPlayer();

    if (m_ActiveFlag)
    {
        DisposeCallbackManager::GetInstance().UnregisterDisposeCallback(this);
        m_ActiveFlag = false;
    }

    BasicSoundPlayer::Finalize();
}

void SequenceSoundPlayer::Setup(SequenceTrackAllocator* trackAllocator, u32 allocTracks,
    NoteOnCallback* callback)
{
    m_ParserParam.callback = callback;

    {
        {
            int trackCount = 0;
            for(u32 trackBitMask = allocTracks; trackBitMask != 0; trackBitMask >>= 1)
            {
                if (trackBitMask & 0x01) 
                {
                    trackCount++;
                }
            }
            if (trackCount > trackAllocator->GetAllocatableTrackCount())
            {
                NW_WARNING(false, "Failed to start sequence sound for not enough sequence track instance." );
                Finalize();
                return;
            }
        }

        {
            u32 trackBitMask = allocTracks;
            for(int trackNo = 0; trackBitMask != 0; trackNo++, trackBitMask >>= 1)
            {
                if ((trackBitMask & 0x01) == 0) continue;

                SequenceTrack* track = trackAllocator->AllocTrack(this);
                NW_NULL_ASSERT(track);
                SetPlayerTrack(trackNo, track);
            }
        }
    }

    DisposeCallbackManager::GetInstance().RegisterDisposeCallback(this);

    m_pSequenceTrackAllocator = trackAllocator;
    m_ActiveFlag = true;
}

void SequenceSoundPlayer::SetSeqData(const void* seqBase, s32 seqOffset)
{
    SequenceTrack* seqTrack = GetPlayerTrack(0);
    if (seqTrack == NULL)
    {
        return;
    }
    
    if (seqBase != NULL)
    {
        seqTrack->SetSeqData(seqBase, seqOffset);
        seqTrack->Open();
    }
}

void SequenceSoundPlayer::SetBankData(const void* bankFiles[], u32 bankFileCount)
{
    NW_ASSERT(bankFileCount <= SoundArchive::SEQ_BANK_MAX);

    if (bankFiles != NULL)
    {
        for (u32 i = 0; i < bankFileCount; i++)
        {
            m_pBankFiles[i] = bankFiles[i];
        }
    }
}

void SequenceSoundPlayer::Start()
{
    SoundThread::GetInstance().RegisterPlayerCallback(this);
    m_StartedFlag = true;
}

void SequenceSoundPlayer::Stop()
{
    FinishPlayer();
}

void SequenceSoundPlayer::Pause(bool flag)
{
    m_PauseFlag = flag;

    SequenceTrack* track;
    for(int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER ; trackNo++)
    {
        track = GetPlayerTrack(trackNo);
        if (track == NULL) continue;

        track->PauseAllChannel(flag);
    }
}

void SequenceSoundPlayer::Skip(OffsetType offsetType, int offset)
{
    if (!m_ActiveFlag)
    {
        return;
    }

    switch (offsetType)
    {
    case OFFSET_TYPE_TICK:
        m_SkipTickCounter += offset;
        break;
    case OFFSET_TYPE_MILLISEC:
        m_SkipTimeCounter += static_cast<f32>(offset);
        break;
    }
}

void SequenceSoundPlayer::SetTempoRatio(float tempoRatio)
{
    NW_ASSERT(tempoRatio >= 0.0f);
    m_TempoRatio = tempoRatio;
}

void SequenceSoundPlayer::SetPanRange(float panRange)
{
    m_PanRange = panRange;
}

void SequenceSoundPlayer::SetChannelPriority(int priority)
{
    NW_MINMAX_ASSERT(priority, 0, 127);
    m_ParserParam.priority = static_cast<u8>(priority);
}

void SequenceSoundPlayer::SetReleasePriorityFix(bool fix)
{
    m_ReleasePriorityFixFlag = fix;
}

void SequenceSoundPlayer::SetSequenceUserprocCallback(SequenceUserprocCallback callback, void* arg)
{
    m_SequenceUserprocCallback = callback;
    m_pSequenceUserprocCallbackArg = arg;
}

void SequenceSoundPlayer::CallSequenceUserprocCallback(u16 procId, SequenceTrack* track)
{
    if (m_SequenceUserprocCallback == NULL)
    {
        return;
    }

    NW_NULL_ASSERT(track);

    SequenceTrack::ParserTrackParam& trackParam = track->GetParserTrackParam();

    SequenceUserprocCallbackParam param;
    param.localVariable = GetVariablePtr(0);
    param.globalVariable = GetVariablePtr(16);
    param.trackVariable = track->GetVariablePtr(0);
    param.cmpFlag = trackParam.cmpFlag;

    m_SequenceUserprocCallback(procId, &param, m_pSequenceUserprocCallbackArg);

    trackParam.cmpFlag = param.cmpFlag;
}

s16 SequenceSoundPlayer::GetLocalVariable(int varNo) const
{
    NW_MINMAXLT_ASSERT(varNo, 0, PLAYER_VARIABLE_NUM);

    return m_LocalVariable[varNo];
}

s16 SequenceSoundPlayer::GetGlobalVariable(int varNo)
{
    NW_MINMAXLT_ASSERT(varNo, 0, GLOBAL_VARIABLE_NUM);

    return m_GlobalVariable[varNo];
}

void SequenceSoundPlayer::SetLocalVariable(int varNo, s16 var)
{
    NW_MINMAXLT_ASSERT(varNo, 0, PLAYER_VARIABLE_NUM);

    m_LocalVariable[varNo] = var;
}

void SequenceSoundPlayer::SetGlobalVariable(int varNo, s16 var)
{
    NW_MINMAXLT_ASSERT(varNo, 0, GLOBAL_VARIABLE_NUM);

    m_GlobalVariable[varNo] = var;
}

void SequenceSoundPlayer::SetTrackMute(u32 trackBitFlag, SeqMute mute)
{
    SetTrackParam(trackBitFlag, &SequenceTrack::SetMute, mute);
}

void SequenceSoundPlayer::SetTrackSilence(unsigned long trackBitFlag, bool silenceFlag, int fadeTimes)
{
    SetTrackParam(trackBitFlag, &SequenceTrack::SetSilence, silenceFlag, fadeTimes);
}

void SequenceSoundPlayer::SetTrackVolume(u32 trackBitFlag, float volume)
{
    NW_ASSERT(volume >= 0.0f);
    SetTrackParam(trackBitFlag, &SequenceTrack::SetVolume, volume);
}

void SequenceSoundPlayer::SetTrackPitch(u32 trackBitFlag, float pitch)
{
    NW_ASSERT(pitch >= 0.0f);
    SetTrackParam(trackBitFlag, &SequenceTrack::SetPitch, pitch);
}

void SequenceSoundPlayer::SetTrackPan(u32 trackBitFlag, float pan)
{
    SetTrackParam(trackBitFlag, &SequenceTrack::SetPan, pan);
}

void SequenceSoundPlayer::SetTrackSurroundPan(u32 trackBitFlag, float surroundPan)
{
    SetTrackParam(trackBitFlag, &SequenceTrack::SetSurroundPan, surroundPan);
}

void SequenceSoundPlayer::SetTrackLpfFreq(u32 trackBitFlag, float lpfFreq)
{
    SetTrackParam(trackBitFlag, &SequenceTrack::SetLpfFreq, lpfFreq);
}

void SequenceSoundPlayer::SetTrackBiquadFilter(u32 trackBitFlag, int type, float value)
{
    SetTrackParam(trackBitFlag, &SequenceTrack::SetBiquadFilter, type, value);
}

void SequenceSoundPlayer::SetTrackPanRange(u32 trackBitFlag, float panRange)
{
    SetTrackParam(trackBitFlag, &SequenceTrack::SetPanRange, panRange);
}

void SequenceSoundPlayer::SetTrackModDepth(u32 trackBitFlag, float depth)
{
    SetTrackParam(trackBitFlag, &SequenceTrack::SetModDepth, depth);
}

void SequenceSoundPlayer::SetTrackModSpeed(u32 trackBitFlag, float speed)
{
    SetTrackParam(trackBitFlag, &SequenceTrack::SetModSpeed, speed);
}

bool SequenceSoundPlayer::SetTrackBankIndex(u32 trackBitFlag, int bankIndex)
{
    NW_MINMAXLT_ASSERT(bankIndex, 0, SoundArchive::SEQ_BANK_MAX);
    if (m_pBankFiles[bankIndex] == NULL)
    {
        return false;
    }
    
    SetTrackParam(trackBitFlag, &SequenceTrack::SetBankIndex, bankIndex);
    return true;
}

void SequenceSoundPlayer::InvalidateData(const void* start, const void* end)
{
    if (m_ActiveFlag)
    {
        for(int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER ; trackNo++)
        {
            SequenceTrack* track = GetPlayerTrack(trackNo);
            if (track == NULL) continue;

            const u8* cur = track->GetParserTrackParam().baseAddr;
            if (start <= cur && cur <= end)
            {
                Finalize();
                break;
            }
        }
        for (int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
        {
            const void* cur = m_pBankFiles[i];
            if (start <= cur && cur <= end)
            {
                m_pBankFiles[i] = NULL;
            }
        }
    }
}

SequenceTrack* SequenceSoundPlayer::GetPlayerTrack(int trackNo)
{
    if (trackNo > TRACK_NUM_PER_PLAYER - 1)
    {
        return NULL;
    }

    return m_pTracks[trackNo];
}

const SequenceTrack* SequenceSoundPlayer::GetPlayerTrack(int trackNo) const
{
    if (trackNo > TRACK_NUM_PER_PLAYER - 1)
    {
        return NULL;
    }

    return m_pTracks[trackNo];
}

void SequenceSoundPlayer::CloseTrack(int trackNo)
{
    NW_MINMAXLT_ASSERT(trackNo, 0, TRACK_NUM_PER_PLAYER);

    SequenceTrack* track = GetPlayerTrack(trackNo);
    if (track == NULL)
    {
        return;
    }

    track->Close();

    m_pSequenceTrackAllocator->FreeTrack(m_pTracks[trackNo]);
    
    m_pTracks[trackNo] = NULL;
}

void SequenceSoundPlayer::SetPlayerTrack(int trackNo, SequenceTrack* track)
{
    if (trackNo > TRACK_NUM_PER_PLAYER - 1)
    {
        return;
    }
    m_pTracks[trackNo] = track;

    track->SetPlayerTrackNo(trackNo);
    track->GetParserTrackParam().frontBypassFlag = IsFrontBypass();
}

void SequenceSoundPlayer::FinishPlayer()
{
    if (m_StartedFlag)
    {
        SoundThread::GetInstance().UnregisterPlayerCallback(this);
        m_StartedFlag = false;
    }

    for(int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER ; trackNo++)
    {
        CloseTrack( trackNo );
    }
}

void SequenceSoundPlayer::UpdateChannelParam()
{
    SequenceTrack* track;
    for(int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER ; trackNo++)
    {
        track = GetPlayerTrack(trackNo);
        if (track == NULL) continue;
        track->UpdateChannelParam();
    }
}

int SequenceSoundPlayer::ParseNextTick( bool doNoteOn )
{
    bool activeFlag = false;

    for(int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER ; trackNo++)
    {
        SequenceTrack* track = GetPlayerTrack(trackNo);
        if (track == NULL) continue;

        track->UpdateChannelLength();

        if (track->ParseNextTick(doNoteOn) < 0)
        {
            CloseTrack(trackNo);
        }

        if (track->IsOpened())
        {
            activeFlag = true;
        }
    }

    if (!activeFlag)
    {
        return 1;
    }

    return 0;
}

vs16* SequenceSoundPlayer::GetVariablePtr(int varNo)
{
    NW_MINMAX_ASSERT(varNo, 0, PLAYER_VARIABLE_NUM + GLOBAL_VARIABLE_NUM);

    if (varNo < PLAYER_VARIABLE_NUM)
    {
        return & m_LocalVariable[varNo];
    }
    else if (varNo < PLAYER_VARIABLE_NUM + GLOBAL_VARIABLE_NUM)
    {
        return & m_GlobalVariable[varNo - PLAYER_VARIABLE_NUM];
    }
    else
    {
        return NULL;
    }
}

void SequenceSoundPlayer::Update()
{
    if (!m_ActiveFlag)
    {
        return;
    }
    if (!m_StartedFlag)
    {
        return;
    }

    if ((m_SkipTickCounter > 0) || (m_SkipTimeCounter > 0.0f))
    {
        SkipTick();
    }
    else if (!m_PauseFlag)
    {
        UpdateTick();
    }
    
    UpdateChannelParam();
}

void SequenceSoundPlayer::UpdateTick()
{
    f32 tickPerMsec = CalcTickPerMsec();
    if (tickPerMsec == 0.0f)
    {
        return;
    }

    u64 restMsec = INTERVAL_MSEC_NUMERATOR;
    u64 nextMsec = static_cast<u64>(INTERVAL_MSEC_DENOMINATOR * m_TickFraction / tickPerMsec);

    while (nextMsec < restMsec)
    {
        restMsec -= nextMsec;

        bool result = (ParseNextTick(true) != 0);

        if (result)
        {
            FinishPlayer();
            m_FinishFlag = true;
            return;
        }
        ++m_TickCounter;

        tickPerMsec = CalcTickPerMsec();
        if (tickPerMsec == 0.0f)
        {
            return;
        }
        nextMsec = static_cast<u64>(INTERVAL_MSEC_DENOMINATOR / tickPerMsec);
    }
    nextMsec -= restMsec;
    m_TickFraction = nextMsec * tickPerMsec / INTERVAL_MSEC_DENOMINATOR;
}

void SequenceSoundPlayer::SkipTick()
{
    for(int trackNo = 0; trackNo < TRACK_NUM_PER_PLAYER ; trackNo++ )
    {
        SequenceTrack* track = GetPlayerTrack(trackNo);
        if (track == NULL) continue;

        track->ReleaseAllChannel( SequenceTrack::PAUSE_RELEASE_VALUE );
        track->FreeAllChannel();
    }

    int skipCount = 0;
    while (m_SkipTickCounter > 0 || m_SkipTimeCounter * CalcTickPerMsec() >= 1.0f)
    {
        if (skipCount >= MAX_SKIP_TICK_PER_FRAME)
        {
            return;
        }

        if (m_SkipTickCounter > 0)
        {
            --m_SkipTickCounter;
        }
        else
        {
            f32 tickPerMsec = CalcTickPerMsec();
            NW_ASSERT(tickPerMsec > 0.0f);
            f32 msecPerTick = 1.0f / tickPerMsec;
            m_SkipTimeCounter -= msecPerTick;
        }

        if (ParseNextTick(false) != 0)
        {
            FinishPlayer();
            m_FinishFlag = true;
            return;
        }
        ++skipCount;
        ++m_TickCounter;
    }

    m_SkipTimeCounter = 0.0f;
}

Channel* SequenceSoundPlayer::NoteOn(u8 bankIndex, const NoteOnInfo& noteOnInfo)
{
    Channel* channel = m_ParserParam.callback->NoteOn(this, bankIndex, noteOnInfo);
    return channel;
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw