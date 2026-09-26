// Filename: snd_SequenceTrack.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SequenceTrack.h>

#include <nw/snd/snd_SequenceSoundPlayer.h>
#include <nw/snd/snd_HardwareManager.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

void SequenceTrack::SetPlayerTrackNo(int playerTrackNo)
{
    NW_MINMAX_ASSERT(playerTrackNo, 0, SequenceSoundPlayer::TRACK_NUM_PER_PLAYER );
    m_PlayerTrackNo = static_cast<u8>(playerTrackNo);
}

SequenceTrack::SequenceTrack(): 
    m_OpenFlag(false),
    m_pSequenceSoundPlayer(NULL),
    m_pChannelList(NULL)
  
{
    InitParam();
}

SequenceTrack::~SequenceTrack()
{
    Close();
}

void SequenceTrack::InitParam()
{
    m_ExtVolume = 1.0f;
    m_ExtPitch = 1.0f;
    m_ExtPan = 0.0f;
    m_ExtSurroundPan = 0.0f;
    m_PanRange = 1.0f;

    m_ParserTrackParam.baseAddr = NULL;
    m_ParserTrackParam.currentAddr = NULL;

    m_ParserTrackParam.cmpFlag = true;
    m_ParserTrackParam.noteWaitFlag = true;
    m_ParserTrackParam.tieFlag = false;
    m_ParserTrackParam.monophonicFlag = false;

    m_ParserTrackParam.callStackDepth = 0;

    m_ParserTrackParam.wait = 0;

    m_ParserTrackParam.muteFlag = false;
    m_ParserTrackParam.silenceFlag = false;
    m_ParserTrackParam.noteFinishWait = false;
    m_ParserTrackParam.portaFlag = false;
    m_ParserTrackParam.damperFlag = false;

    m_ParserTrackParam.bankIndex = 0;
    m_ParserTrackParam.prgNo = 0;

    m_ParserTrackParam.lfoParam.Initialize();
    m_ParserTrackParam.lfoTarget = Channel::LFO_TARGET_PITCH;
    m_ParserTrackParam.sweepPitch = 0.0f;

    m_ParserTrackParam.volume.InitValue(127);
    m_ParserTrackParam.pan.InitValue(0);
    m_ParserTrackParam.surroundPan.InitValue(0);
    m_ParserTrackParam.volume2 = 127;
    m_ParserTrackParam.velocityRange = 127;
    m_ParserTrackParam.pitchBend.InitValue(0);
    m_ParserTrackParam.bendRange = DEFAULT_BENDRANGE;
    m_ParserTrackParam.initPan = 0;
    m_ParserTrackParam.transpose = 0;
    m_ParserTrackParam.priority = DEFAULT_PRIORITY;
    m_ParserTrackParam.portaKey = DEFAULT_PORTA_KEY;
    m_ParserTrackParam.portaTime = 0;

    m_ParserTrackParam.attack = INVALID_ENVELOPE;
    m_ParserTrackParam.decay = INVALID_ENVELOPE;
    m_ParserTrackParam.sustain = INVALID_ENVELOPE;
    m_ParserTrackParam.release = INVALID_ENVELOPE;
    m_ParserTrackParam.envHold = INVALID_ENVELOPE;

    m_ParserTrackParam.mainSend = 127;
    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        m_ParserTrackParam.fxSend[i] = 0;
    }

    m_ParserTrackParam.lpfFreq = 0.0f;
    m_ParserTrackParam.biquadType = 0;
    m_ParserTrackParam.biquadValue = 0;

    for(int varNo = 0; varNo < TRACK_VARIABLE_NUM ; varNo++)
    {
        m_TrackVariable[varNo] = SequenceSoundPlayer::VARIABLE_DEFAULT_VALUE;
    }
}

void SequenceTrack::SetSeqData(const void* seqBase, s32 seqOffset)
{
    m_ParserTrackParam.baseAddr = static_cast<const u8*>(seqBase);
    m_ParserTrackParam.currentAddr = m_ParserTrackParam.baseAddr + seqOffset;
}

void SequenceTrack::Open()
{
    m_ParserTrackParam.noteFinishWait = false;
    m_ParserTrackParam.callStackDepth = 0;
    m_ParserTrackParam.wait = 0;

    m_OpenFlag = true;
}

void SequenceTrack::Close()
{
    ReleaseAllChannel(-1);
    FreeAllChannel();

    m_OpenFlag = false;
}

void SequenceTrack::UpdateChannelLength()
{
    if (!m_OpenFlag) 
    {
        return;
    }

    Channel* channel = m_pChannelList;
    while(channel != NULL)
    {
        if (channel->GetLength() > 0)
        {
            channel->SetLength(channel->GetLength() - 1);
        }

        UpdateChannelRelease(channel);

        if (!channel->IsAutoUpdateSweep())
        {
            channel->UpdateSweep(1);
        }

        channel = channel->GetNextTrackChannel();
    }
}

void SequenceTrack::UpdateChannelRelease(Channel* channel)
{
    if ((channel->GetLength() == 0) && (!channel->IsRelease()))
    {
        if (!m_ParserTrackParam.damperFlag)
        {
            channel->NoteOff();
        }
    }
}

int SequenceTrack::ParseNextTick(bool doNoteOn)
{
    if (!m_OpenFlag) 
    {
        return 0;
    }

    m_ParserTrackParam.volume.Update();
    m_ParserTrackParam.pan.Update();
    m_ParserTrackParam.surroundPan.Update();
    m_ParserTrackParam.pitchBend.Update();

    if (m_ParserTrackParam.noteFinishWait) 
    {
        if (m_pChannelList != NULL) 
        {
            return 1;
        }
        m_ParserTrackParam.noteFinishWait = false;
    }
    if (m_ParserTrackParam.wait > 0) 
    {
        m_ParserTrackParam.wait--;
        if (m_ParserTrackParam.wait > 0) 
        {
            return 1;
        }
    }

    if (m_ParserTrackParam.currentAddr != NULL)
    {
        while(m_ParserTrackParam.wait == 0 && ! m_ParserTrackParam.noteFinishWait)
        {
            ParseResult result = Parse(doNoteOn);
            if (result == PARSE_RESULT_FINISH) 
            {
                return -1;
            }
        }
    }

    return 1;
}

void SequenceTrack::StopAllChannel()
{
    Channel* channel = m_pChannelList;

    while(channel != NULL)
    {
        Channel::FreeChannel(channel);
        channel->Stop();
        channel = channel->GetNextTrackChannel();
    }

    m_pChannelList = NULL;
}

void SequenceTrack::ReleaseAllChannel(int release)
{
    UpdateChannelParam();

    Channel* channel = m_pChannelList;
    while(channel != NULL)
    {
        if (channel->IsActive())
        {
            if (release >= 0)
            {
                NW_MINMAX_ASSERT(release, 0, 127);
                channel->SetRelease(static_cast<u8>(release));
            }
            channel->Release();
        }
        channel = channel->GetNextTrackChannel();
    }
}

void SequenceTrack::PauseAllChannel(bool flag)
{
    Channel* channel = m_pChannelList;

    while(channel != NULL)
    {
        if (channel->IsActive() && (channel->IsPause() != flag))
        {
            channel->Pause(flag);
        }
        channel = channel->GetNextTrackChannel();
    }
}

void SequenceTrack::AddChannel(Channel* channel)
{
    channel->SetNextTrackChannel(m_pChannelList);
    m_pChannelList = channel;
}

void SequenceTrack::UpdateChannelParam()
{
    if (!m_OpenFlag) 
    {
        return;
    }

    if (m_pChannelList == NULL) 
    {
        return;
    }

    // volume
    register float volume
        = m_ParserTrackParam.volume.GetValue()
        * m_ParserTrackParam.volume2
        * m_pSequenceSoundPlayer->GetParserPlayerParam().volume
        / (127.0f * 127.0f * 127.0f);
    volume = volume * volume * m_ExtVolume * m_pSequenceSoundPlayer->GetVolume();
    
    // pitch
    f32 pitch = (m_ParserTrackParam.pitchBend.GetValue() / 128.0f) * m_ParserTrackParam.bendRange;
    
    // pitch ratio
    float pitchRatio = m_pSequenceSoundPlayer->GetPitch() * m_ExtPitch;

    // pan
    register float pan
        = ut::Clamp(static_cast<float>(m_ParserTrackParam.pan.GetValue()) / 63.0f, -1.0f, 1.0f)
        * m_PanRange
        * m_pSequenceSoundPlayer->GetPanRange()
        + m_ExtPan
        + m_pSequenceSoundPlayer->GetPan();
    
    // surround pan
    float surroundPan
        = ut::Clamp( static_cast<float>( m_ParserTrackParam.surroundPan.GetValue()) / 63.0f, 0.0f, 2.0f)
        + m_ExtSurroundPan
        + m_pSequenceSoundPlayer->GetSurroundPan();
    
    // lpf freq
    float lpfFreq
        = m_ParserTrackParam.lpfFreq
        + m_pSequenceSoundPlayer->GetLpfFreq();
    
    // biquad filter
    int biquadType = m_ParserTrackParam.biquadType;
    float biquadValue = m_ParserTrackParam.biquadValue;
    if (m_pSequenceSoundPlayer->GetBiquadFilterType() != -1)
    {
        biquadType = m_pSequenceSoundPlayer->GetBiquadFilterType();
        biquadValue = m_pSequenceSoundPlayer->GetBiquadFilterValue();
    }

    // main send
    float mainSend
        = static_cast<float>(m_ParserTrackParam.mainSend) / 127.0f - 1.0f
        + m_pSequenceSoundPlayer->GetMainSend();
    
    float fxSend[AUX_BUS_NUM];
    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        AuxBus bus = static_cast<AuxBus>(i);
        fxSend[i] = static_cast<float>(m_ParserTrackParam.fxSend[i]) / 127.0f
            + m_pSequenceSoundPlayer->GetFxSend(bus);
    }

    // set channel params of track
    register Channel* channel = m_pChannelList;
    while(channel != NULL)
    {
        channel->SetUserVolume(volume);
        channel->SetUserPitch(pitch);
        channel->SetUserPitchRatio(pitchRatio);
        channel->SetUserPan(pan);
        channel->SetUserSurroundPan(surroundPan);
        channel->SetUserLpfFreq(lpfFreq);
        channel->SetBiquadFilter(biquadType, biquadValue);
        channel->SetMainSend(mainSend);
        for (int i = 0; i < AUX_BUS_NUM; i++)
        {
            AuxBus bus = static_cast<AuxBus>(i);
            channel->SetFxSend(bus, fxSend[i]);
        }

        channel->SetLfoParam(m_ParserTrackParam.lfoParam);
        channel->SetLfoTarget(static_cast<Channel::LfoTarget>(m_ParserTrackParam.lfoTarget));

        channel = channel->GetNextTrackChannel();
    }
}

void SequenceTrack::FreeAllChannel()
{
    Channel* channel = m_pChannelList;

    while(channel != NULL) 
    {
        Channel::FreeChannel(channel);
        channel = channel->GetNextTrackChannel();
    }
    m_pChannelList = NULL;
}


int SequenceTrack::GetChannelCount() const
{
    Channel* channel = m_pChannelList;
    int count = 0;
    while(channel != NULL) 
    {
        ++count;
        channel = channel->GetNextTrackChannel();
    }

    return count;
}

void SequenceTrack::ChannelCallbackFunc(Channel* dropChannel, 
    Channel::ChannelCallbackStatus status, u32 userData)
{
    SequenceTrack* track = reinterpret_cast<SequenceTrack*>(userData);

    NW_NULL_ASSERT(dropChannel);
    NW_NULL_ASSERT(track);

    switch (status)
    {
    case Channel::CALLBACK_STATUS_STOPPED:
    case Channel::CALLBACK_STATUS_FINISH:
        Channel::FreeChannel(dropChannel);
        break;
    }

    if (track->m_pSequenceSoundPlayer != NULL) 
    {
        track->m_pSequenceSoundPlayer->ChannelCallback(dropChannel);
    }

    if (track->m_pChannelList == dropChannel) 
    {
        track->m_pChannelList = dropChannel->GetNextTrackChannel();
        return;
    }

    Channel* channel = track->m_pChannelList;
    NW_NULL_ASSERT(channel);
    while(channel->GetNextTrackChannel() != NULL) 
    {
        if (channel->GetNextTrackChannel() == dropChannel) 
        {
            channel->SetNextTrackChannel(dropChannel->GetNextTrackChannel());
            return;
        }
        channel = channel->GetNextTrackChannel();
    }

    NW_ASSERT(false);
}

void SequenceTrack::SetMute(SeqMute mute)
{
    switch (mute)
    {
    case SEQ_MUTE_OFF:
        m_ParserTrackParam.muteFlag = false;
        break;
    case SEQ_MUTE_STOP:
        StopAllChannel();
        m_ParserTrackParam.muteFlag = true;
        break;
    case SEQ_MUTE_RELEASE:
        ReleaseAllChannel(-1);
        FreeAllChannel();
        m_ParserTrackParam.muteFlag = true;
        break;
    case SEQ_MUTE_NO_STOP:
        m_ParserTrackParam.muteFlag = true;
        break;
    }
}

void SequenceTrack::SetSilence(bool silenceFlag, int fadeTimes)
{
    m_ParserTrackParam.silenceFlag = silenceFlag;

    Channel* channel = m_pChannelList;
    while(channel != NULL)
    {
        channel->SetSilence(silenceFlag,
            (fadeTimes + HardwareManager::SOUND_FRAME_INTERVAL_MSEC - 1) /
              HardwareManager::SOUND_FRAME_INTERVAL_MSEC);
        channel = channel->GetNextTrackChannel();
    }
}

void SequenceTrack::SetBiquadFilter(int type, float value)
{
    m_ParserTrackParam.biquadType = static_cast<u8>( type );
    m_ParserTrackParam.biquadValue = value;
}

void SequenceTrack::SetBankIndex(int bankIndex)
{
    NW_MINMAXLT_ASSERT(bankIndex, 0, SoundArchive::SEQ_BANK_MAX);
    m_ParserTrackParam.bankIndex = static_cast<u8>(bankIndex);
}

s16 SequenceTrack::GetTrackVariable(int varNo) const
{
    NW_MINMAXLT_ASSERT(varNo, 0, TRACK_VARIABLE_NUM);

    return m_TrackVariable[varNo];
}

void SequenceTrack::SetTrackVariable(int varNo, s16 var)
{
    NW_MINMAXLT_ASSERT(varNo, 0, TRACK_VARIABLE_NUM);

    m_TrackVariable[varNo] = var;
}

vs16* SequenceTrack::GetVariablePtr(int varNo)
{
    NW_MINMAX_ASSERT(varNo, 0, TRACK_VARIABLE_NUM);

    if (varNo < TRACK_VARIABLE_NUM)
    {
        return & m_TrackVariable[varNo];
    }
    else
    {
        return NULL;
    }
}

Channel* SequenceTrack::NoteOn(int key, int velocity, s32 length, bool tieFlag)
{  
    NW_MINMAX_ASSERT(key, 0, 127);
    NW_MINMAX_ASSERT(velocity, 0, 127);

    const SequenceSoundPlayer::ParserPlayerParam& playerParam = m_pSequenceSoundPlayer->GetParserPlayerParam();

    Channel* channel = NULL;

    velocity = velocity * m_ParserTrackParam.velocityRange / 127;

    if (tieFlag)
    {
        channel = GetLastChannel();
        if (channel != NULL)
        {
            channel->SetKey(static_cast<u8>(key));
            float initVolume = static_cast<float>(velocity) / 127.0f;
            channel->SetInitVolume(initVolume * initVolume);
        }
    }

    if (GetParserTrackParam().monophonicFlag)
    {
        channel = GetLastChannel();
        if (channel != NULL)
        {
            if (channel->IsRelease())
            {
                channel->Stop();
                channel = NULL;
            }
            else
            {
                channel->SetKey(static_cast<u8>(key));
                float initVolume = static_cast<float>(velocity) / 127.0f;
                channel->SetInitVolume(initVolume * initVolume);
                channel->SetLength(length);
            }
        }
    }

    if (channel == NULL)
    {
        NoteOnInfo noteOnInfo = 
        {
            m_ParserTrackParam.prgNo,
            key,
            velocity,
            tieFlag ? -1 : length,
            m_ParserTrackParam.initPan,
            playerParam.priority + m_ParserTrackParam.priority,
            SequenceTrack::ChannelCallbackFunc,
            reinterpret_cast<u32>(this)
        };
        channel = m_pSequenceSoundPlayer->NoteOn(m_ParserTrackParam.bankIndex, noteOnInfo);
        if (channel == NULL) 
        {
            return NULL;
        }

        if (channel->GetKeyGroupId() > 0)
        {
            Channel* itr = m_pChannelList;
            while(itr != NULL)
            {
                if (itr->GetKeyGroupId() == channel->GetKeyGroupId())
                {
                    itr->SetRelease(126);
                    itr->Release();
                }
                itr = itr->GetNextTrackChannel();
            }
        }

        AddChannel(channel);
    }

    if (m_ParserTrackParam.attack <= SequenceTrack::MAX_ENVELOPE_VALUE)
    {
        channel->SetAttack(m_ParserTrackParam.attack);
    }

    if (m_ParserTrackParam.decay <= SequenceTrack::MAX_ENVELOPE_VALUE)
    {
        channel->SetDecay(m_ParserTrackParam.decay);
    }

    if (m_ParserTrackParam.sustain <= SequenceTrack::MAX_ENVELOPE_VALUE)
    {
        channel->SetSustain(m_ParserTrackParam.sustain);
    }

    if (m_ParserTrackParam.release <= SequenceTrack::MAX_ENVELOPE_VALUE)
    {
        channel->SetRelease(m_ParserTrackParam.release);
    }
    if (m_ParserTrackParam.envHold <= SequenceTrack::MAX_ENVELOPE_VALUE)
    {
        channel->SetHold(m_ParserTrackParam.envHold);
    }

    f32 sweepPitch = m_ParserTrackParam.sweepPitch;

    if (m_ParserTrackParam.portaFlag)
    {
        sweepPitch += m_ParserTrackParam.portaKey - key;
    }

    if (m_ParserTrackParam.portaTime == 0)
    {
        NW_WARNING(length != 0, "portatime zero is invalid.");
        channel->SetSweepParam(sweepPitch, length, false);
    }
    else
    {
        int sweepTime = m_ParserTrackParam.portaTime;
        sweepTime *= sweepTime;
        sweepTime = static_cast<int>(sweepTime * (sweepPitch >= 0 ? sweepPitch : -sweepPitch));
        sweepTime >>= 5;
        sweepTime *= 5;

        channel->SetSweepParam(sweepPitch, sweepTime, true);
    }

    m_ParserTrackParam.portaKey = static_cast<u8>(key);

    channel->SetSilence(m_ParserTrackParam.silenceFlag != 0, 0);
    channel->SetReleasePriorityFix(m_pSequenceSoundPlayer->IsReleasePriorityFix());
    channel->SetPanMode(m_pSequenceSoundPlayer->GetPanMode());
    channel->SetPanCurve(m_pSequenceSoundPlayer->GetPanCurve());
    channel->SetFrontBypass(m_ParserTrackParam.frontBypassFlag);
    
    return channel;
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw