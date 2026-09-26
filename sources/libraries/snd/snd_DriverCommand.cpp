// Filename: snd_DriverCommand.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_DriverCommand.h>
#include <nw/snd/snd_BasicSoundPlayer.h>
#include <nw/snd/snd_DisposeCallbackManager.h>
#include <nw/snd/snd_SequenceSoundPlayer.h>
#include <nw/snd/snd_SequenceTrack.h>
#include <nw/snd/snd_StreamSoundPlayer.h>
#include <nw/snd/snd_WaveSoundPlayer.h>
#include <nw/snd/snd_VoiceManager.h>

namespace nw {
namespace snd {
namespace internal {

void DriverCommand::ProcessCommandList(DriverCommand *commandList)
{
    for(DriverCommand* command = commandList; command != NULL; command = command->next)
    {
        switch(command->id)
        {
        case DRIVER_COMMAND_DEBUG:
            NN_TLOG_("[CMD] %s\n", command->id);
            break;
        case DRIVER_COMMAND_REPLY:
        {
            DriverCommandReply* c = reinterpret_cast<DriverCommandReply*>(command);
            *(c->ptr) = true;
            break;
        }
        case DRIVER_COMMAND_PLAYER_INIT:
        {
            DriverCommandPlayerInit* c = reinterpret_cast<DriverCommandPlayerInit*>(command);
            c->player->Initialize();
            *(c->availableFlagPtr) = true;
            break;
        }
        case DRIVER_COMMAND_PLAYER_FINALIZE:
        {
            DriverCommandPlayer* c = reinterpret_cast<DriverCommandPlayer*>(command);
            c->player->Finalize();
            break;
        }
        case DRIVER_COMMAND_PLAYER_START:
        {
            DriverCommandPlayer* c = reinterpret_cast<DriverCommandPlayer*>(command);
            c->player->Start();
            break;
        }
        case DRIVER_COMMAND_PLAYER_STOP:
        {
            DriverCommandPlayer* c = reinterpret_cast<DriverCommandPlayer*>(command);
            if (c->flag) 
            {
                c->player->SetVolume(0.0f);
            }
            c->player->Stop();
            break;
        }
        case DRIVER_COMMAND_PLAYER_PAUSE:
        {
            DriverCommandPlayer* c = reinterpret_cast<DriverCommandPlayer*>(command);
            c->player->Pause(c->flag);
            break;
        }
        case DRIVER_COMMAND_PLAYER_PARAM:
        {
            DriverCommandPlayerParam* c = reinterpret_cast<DriverCommandPlayerParam*>(command);
            c->player->SetVolume(c->volume);
            c->player->SetPitch(c->pitch);
            c->player->SetPan(c->pan);
            c->player->SetSurroundPan(c->surroundPan );
            c->player->SetLpfFreq(c->lpfFreq );
            c->player->SetBiquadFilter(c->biquadFilterType, c->biquadFilterValue);
            c->player->SetMainSend(c->mainSend);
            for (int i = 0; i < AUX_BUS_NUM; i++) 
            {
                c->player->SetFxSend(AuxBus(i), c->fxSend[i]);
            }
            break;
        }
        case DRIVER_COMMAND_PLAYER_PANMODE:
        {
            DriverCommandPlayerPanParam* c = reinterpret_cast<DriverCommandPlayerPanParam*>(command);
            c->player->SetPanMode(c->panMode);
            break;
        }
        case DRIVER_COMMAND_PLAYER_PANCURVE:
        {
            DriverCommandPlayerPanParam* c = reinterpret_cast<DriverCommandPlayerPanParam*>(command);
            c->player->SetPanCurve(c->panCurve);
            break;
        }
        case DRIVER_COMMAND_PLAYER_FRONTBYPASS:
        {
            DriverCommandPlayer* c = reinterpret_cast<DriverCommandPlayer*>(command);
            c->player->SetFrontBypass(c->flag);
            break;
        }
        case DRIVER_COMMAND_SEQ_INIT:
        {
            DriverCommandSeqInit* c = reinterpret_cast<DriverCommandSeqInit*>(command);
            c->player->Setup(c->trackAllocator, c->allocTracks, c->noteOnCallback);
            break;
        }
        case DRIVER_COMMAND_SEQ_SEQDATA:
        {
            DriverCommandSeqSetData* c = reinterpret_cast<DriverCommandSeqSetData*>(command);
            c->player->SetSeqData(c->seqBase, c->seqOffset);
            break;
        }
        case DRIVER_COMMAND_SEQ_BANKDATA:
        {
            DriverCommandSeqSetBankData* c = reinterpret_cast<DriverCommandSeqSetBankData*>(command);
            c->player->SetBankData(c->bankFiles, SoundArchive::SEQ_BANK_MAX);
            break;
        }
        case DRIVER_COMMAND_SEQ_SKIP:
        {
            DriverCommandSeqSkip* c = reinterpret_cast<DriverCommandSeqSkip*>(command);
            c->player->Skip(driver::SequenceSoundPlayer::OffsetType(c->offsetType), c->offset);
            break;
        }
        case DRIVER_COMMAND_SEQ_TEMPORATIO:
        {
            DriverCommandSeqTempoRatio* c = reinterpret_cast<DriverCommandSeqTempoRatio*>(command);
            c->player->SetTempoRatio(c->tempoRatio);
            break;
        }
        case DRIVER_COMMAND_SEQ_CHANNELPRIO:
        {
            DriverCommandSeqChannelPrio* c = reinterpret_cast<DriverCommandSeqChannelPrio*>(command);
            c->player->SetChannelPriority(c->priority);
            break;
        }
        case DRIVER_COMMAND_SEQ_PRIOFIX:
        {
            DriverCommandSeqPrioFix* c = reinterpret_cast<DriverCommandSeqPrioFix*>(command);
            c->player->SetReleasePriorityFix(c->priorityFix);
            break;
        }
        case DRIVER_COMMAND_SEQ_USERPROC:
        {
            DriverCommandSeqUserProc* c = reinterpret_cast<DriverCommandSeqUserProc*>(command);
            c->player->SetSequenceUserprocCallback(reinterpret_cast<SequenceUserprocCallback>(c->callback), c->arg);
            break;
        }
        case DRIVER_COMMAND_SEQ_SETVAR:
        {
            DriverCommandSeqSetVar* c = reinterpret_cast<DriverCommandSeqSetVar*>(command);
            c->player->SetLocalVariable(c->varNo, c->var);
            break;
        }
        case DRIVER_COMMAND_SEQ_SETGVAR:
        {
            DriverCommandSeqSetVar* c = reinterpret_cast<DriverCommandSeqSetVar*>(command);
            driver::SequenceSoundPlayer::SetGlobalVariable(c->varNo, c->var);
            break;
        }
        case DRIVER_COMMAND_SEQ_SETTVAR:
        {
            DriverCommandSeqSetVar* c = reinterpret_cast<DriverCommandSeqSetVar*>(command);
            driver::SequenceTrack* track = c->player->GetPlayerTrack(c->trackNo);
            if (track != NULL) 
            {
                track->SetTrackVariable(c->varNo, c->var);
            }
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_MUTE:
        {
            DriverCommandSeqTrackMute* c = reinterpret_cast<DriverCommandSeqTrackMute*>(command);
            c->player->SetTrackMute(c->trackBitFlag, SeqMute(c->mute));
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_SILENCE:
        {
            DriverCommandSeqTrackSilence* c = reinterpret_cast<DriverCommandSeqTrackSilence*>(command);
            c->player->SetTrackSilence(c->trackBitFlag, c->silenceFlag, c->fadeFrames);
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_VOLUME:
        {
            DriverCommandSeqTrackParam* c = reinterpret_cast<DriverCommandSeqTrackParam*>(command);
            c->player->SetTrackVolume(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_PITCH:
        {
            DriverCommandSeqTrackParam* c = reinterpret_cast<DriverCommandSeqTrackParam*>(command);
            c->player->SetTrackPitch(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_PAN:
        {
            DriverCommandSeqTrackParam* c = reinterpret_cast<DriverCommandSeqTrackParam*>(command);
            c->player->SetTrackPan(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_SPAN:
        {
            DriverCommandSeqTrackParam* c = reinterpret_cast<DriverCommandSeqTrackParam*>(command);
            c->player->SetTrackSurroundPan(c->trackBitFlag, c->value);
            break;
        }
        
        case DRIVER_COMMAND_SEQTRACK_LPF:
        {
            DriverCommandSeqTrackParam* c = reinterpret_cast<DriverCommandSeqTrackParam*>(command);
            c->player->SetTrackLpfFreq(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_PANRANGE:
        {
            DriverCommandSeqTrackParam* c = reinterpret_cast<DriverCommandSeqTrackParam*>(command);
            c->player->SetTrackPanRange(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_MODDEPTH:
        {
            DriverCommandSeqTrackParam* c = reinterpret_cast<DriverCommandSeqTrackParam*>(command);
            c->player->SetTrackModDepth(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_MODSPEED:
        {
            DriverCommandSeqTrackParam* c = reinterpret_cast<DriverCommandSeqTrackParam*>(command);
            c->player->SetTrackModSpeed(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_BIQUAD:
        {
            DriverCommandSeqTrackBiquad* c = reinterpret_cast<DriverCommandSeqTrackBiquad*>(command);
            c->player->SetTrackBiquadFilter(c->trackBitFlag, c->type, c->value);
            break;
        }
        case DRIVER_COMMAND_SEQTRACK_BANKINDEX:
        {
            DriverCommandSeqTrackBankIndex* c = reinterpret_cast<DriverCommandSeqTrackBankIndex*>(command);
            c->player->SetTrackBankIndex(c->trackBitFlag, c->bankIndex);
            break;
        }
        case DRIVER_COMMAND_WAVESND_SETUP:
        {
            DriverCommandWaveSoundSetup* c = reinterpret_cast<DriverCommandWaveSoundSetup*>(command);
            c->player->Prepare(c->wsdFile, c->waveSoundOffset,
                driver::WaveSoundPlayer::StartOffsetType(c->startOffsetType), 
                c->offset,
                reinterpret_cast<const driver::WaveSoundPlayer::WaveSoundCallback*>(c->callback), 
                c->callbackData);
            break;
        }
        case DRIVER_COMMAND_WAVESND_CHANNELPRIO:
        {
            DriverCommandWaveSoundChannelPrio* c = reinterpret_cast<DriverCommandWaveSoundChannelPrio*>(command);
            c->player->SetChannelPriority(c->priority);
            break;
        }
        case DRIVER_COMMAND_WAVESND_PRIOFIX:
        {
            DriverCommandWaveSoundPrioFix* c = reinterpret_cast<DriverCommandWaveSoundPrioFix*>(command);
            c->player->SetReleasePriorityFix(c->priorityFix);
            break;
        }
        case DRIVER_COMMAND_STRM_INIT:
        {
            DriverCommandStreamSoundInit* c = reinterpret_cast<DriverCommandStreamSoundInit*>(command);
            c->player->Setup(c->pBufferPool, c->allocChannelCount, c->allocTrackFlag);
            break;
        }
        case DRIVER_COMMAND_STRM_SETUP:
        {
            DriverCommandStreamSoundSetup* c = reinterpret_cast<DriverCommandStreamSoundSetup*>(command);
            c->player->Prepare(c->pFileStream,
                driver::StreamSoundPlayer::StartOffsetType(c->startOffsetType), c->offset);
            break;
        }
        case DRIVER_COMMAND_STRM_LOADHEADER:
        {
            DriverCommandStreamSoundLoadHeader* c = reinterpret_cast<DriverCommandStreamSoundLoadHeader*>(command);
            c->player->LoadHeader(
                c->streamInfo,
                c->trackInfoArray,
                c->dspAdpcmParam,
                c->dspAdpcmLoopParam,
                c->dataBlockOffset,
                c->trackCount,
                c->channelCount
            );
            break;
        }
        case DRIVER_COMMAND_STRM_LOADDATA:
        {
            DriverCommandStreamSoundLoadData* c = reinterpret_cast<DriverCommandStreamSoundLoadData*>(command);
            c->player->LoadStreamData(
                c->bufferBlockIndex,
                c->dataBlockIndex,
                c->blockSamples,
                c->isDataLoopBlock,
                c->lastBlockFlag
            );
            break;
        }
        case DRIVER_COMMAND_STRM_TRACK_VOLUME:
        {
            DriverCommandStreamSoundTrackParam* c = reinterpret_cast<DriverCommandStreamSoundTrackParam*>(command);
            c->player->SetTrackVolume(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_STRM_TRACK_PAN:
        {
            DriverCommandStreamSoundTrackParam* c = reinterpret_cast<DriverCommandStreamSoundTrackParam*>(command);
            c->player->SetTrackPan(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_STRM_TRACK_SPAN:
        {
            DriverCommandStreamSoundTrackParam* c = reinterpret_cast<DriverCommandStreamSoundTrackParam*>(command);
            c->player->SetTrackSurroundPan(c->trackBitFlag, c->value);
            break;
        }
        case DRIVER_COMMAND_INVALIDATE_DATA:
        {
            DriverCommandInvalidateData* c = reinterpret_cast<DriverCommandInvalidateData*>(command);
            driver::DisposeCallbackManager::GetInstance().Dispose(c->mem, c->size);
            break;
        }
        case DRIVER_COMMAND_REGIST_DISPOSE_CALLBACK:
        {
            DriverCommandDisposeCallback* c = reinterpret_cast<DriverCommandDisposeCallback*>(command);
            driver::DisposeCallbackManager::GetInstance().RegisterDisposeCallback(c->callback);
            break;
        }
        case DRIVER_COMMAND_UNREGIST_DISPOSE_CALLBACK:
        {
            DriverCommandDisposeCallback* c = reinterpret_cast<DriverCommandDisposeCallback*>(command);
            driver::DisposeCallbackManager::GetInstance().UnregisterDisposeCallback(c->callback);
            break;
        }
        case DRIVER_COMMAND_APPEND_EFFECT:
        {
            DriverCommandEffect* c = reinterpret_cast<DriverCommandEffect*>(command);
            internal::driver::HardwareManager::GetInstance().AppendEffect(c->bus, c->effect);
            break;
        }
        case DRIVER_COMMAND_APPEND_EFFECT_SDK_DELAY:
        {
            DriverCommandEffectSdkDelay* c = reinterpret_cast<DriverCommandEffectSdkDelay*>(command);
            internal::driver::HardwareManager::GetInstance().AppendEffect(c->bus, c->effect);
            break;
        }
        case DRIVER_COMMAND_APPEND_EFFECT_SDK_REVERB:
        {
            DriverCommandEffectSdkReverb* c = reinterpret_cast<DriverCommandEffectSdkReverb*>(command);
            internal::driver::HardwareManager::GetInstance().AppendEffect(c->bus, c->effect);
            break;
        } 
        case DRIVER_COMMAND_CLEAR_EFFECT:
        {
            DriverCommandEffect* c = reinterpret_cast<DriverCommandEffect*>(command);
            internal::driver::HardwareManager::GetInstance().ClearEffect(c->bus, c->fadeTimes);
            break;
        }
        case DRIVER_COMMAND_ALLVOICES_SYNC:
        {
            DriverCommandAllVoicesSync* c = reinterpret_cast<DriverCommandAllVoicesSync*>(command);
            driver::VoiceManager::GetInstance().UpdateAllVoicesSync( c->syncFlag );
            break;
        }
        }
    }
}

} // namespace internal
} // namespace snd
} // namespace nw