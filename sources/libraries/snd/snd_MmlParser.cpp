// Filename: snd_MmlParser.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_MmlParser.h>
#include <nw/snd/snd_MmlCommand.h>
#include <nw/snd/snd_MmlSequenceTrack.h>
#include <nw/snd/snd_SequenceSoundPlayer.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

SequenceTrack::ParseResult MmlParser::Parse(MmlSequenceTrack* pTrack, bool doNoteOn) const
{
    NW_NULL_ASSERT(track);
    SequenceSoundPlayer* pPlayer = track->GetSequenceSoundPlayer();
    NW_NULL_ASSERT(player);

    SequenceTrack::ParserTrackParam& rTrackParam = pTrack->GetParserTrackParam();
    SequenceSoundPlayer::ParserPlayerParam& rPlayerParam = pPlayer->GetParserPlayerParam();

    SeqArgType argType = SEQ_ARG_NONE;
    SeqArgType argType2 = SEQ_ARG_NONE;
    bool useArgType = false;
    bool doExecCommand = true;

    u32 cmd = ReadByte(&rTrackParam.currentAddr);

    if(cmd == MmlCommand::MML_IF) 
    {
        cmd = ReadByte(&rTrackParam.currentAddr);
        doExecCommand = rTrackParam.cmpFlag != false;
    }

    if(cmd == MmlCommand::MML_TIME)
    {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType2 = SEQ_ARG_S16;
    }
    else if(cmd == MmlCommand::MML_TIME_RANDOM)
    {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType2 = SEQ_ARG_RANDOM;
    }
    else if(cmd == MmlCommand::MML_TIME_VARIABLE)
    {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType2 = SEQ_ARG_VARIABLE;
    }

    if(cmd == MmlCommand::MML_RANDOM)
    {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType = SEQ_ARG_RANDOM;
        useArgType = true;
    }
    else if(cmd == MmlCommand::MML_VARIABLE)
    {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType = SEQ_ARG_VARIABLE;
        useArgType = true;
    }

    // MML note data, not a command
    if (!(cmd & 0x80)) 
    {
        const u8 velocity = ReadByte(&rTrackParam.currentAddr);

        const s32 length = ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                             useArgType ? argType : SEQ_ARG_VMIDI);

        int key = cmd + rTrackParam.transpose;

        if (!doExecCommand) 
        {
            return SequenceTrack::PARSE_RESULT_CONTINUE;
        }

        key = ut::Clamp(key, 0, 127);

        if (!rTrackParam.muteFlag && doNoteOn) 
        {
            NoteOnCommandProc(pTrack, key, velocity, length > 0 ? length : -1,
                              rTrackParam.tieFlag);
        }

        if (rTrackParam.noteWaitFlag) 
        {
            rTrackParam.wait = length;

            if (length == 0) 
            {
                rTrackParam.noteFinishWait = true;
            }
        }
    }
    else 
    {
        s32 arg1 = 0;
        s32 arg2 = 0;

        switch (cmd & 0xf0) 
        {
        case 0x80: 
        {
            switch (cmd) 
            {
            case MmlCommand::MML_WAIT: 
            {
                s32 time = ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                                   useArgType ? argType : SEQ_ARG_VMIDI);

                if (doExecCommand) 
                {
                    rTrackParam.wait = time;
                }
                break;
            }

            case MmlCommand::MML_PRG: 
            {
                arg1 = ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                               useArgType ? argType : SEQ_ARG_VMIDI);

                if (doExecCommand) 
                {
                    CommandProc(pTrack, cmd, arg1, arg2);
                }
                break;
            }

            case MmlCommand::MML_OPEN_TRACK: 
            {
                u8 trackNo = ReadByte(&rTrackParam.currentAddr);
                u32 offset = Read24(&rTrackParam.currentAddr);

                if (doExecCommand) 
                {
                    arg1 = trackNo;
                    arg2 = offset;
                    CommandProc(pTrack, cmd, arg1, arg2);
                }
                break;
            }

            case MmlCommand::MML_JUMP: 
            {
                u32 offset = Read24(&rTrackParam.currentAddr);

                if (doExecCommand) 
                {
                    arg1 = offset;
                    CommandProc(pTrack, cmd, arg1, arg2);
                }
                break;
            }

            case MmlCommand::MML_CALL: 
            {
                u32 offset = Read24(&rTrackParam.currentAddr);

                if (doExecCommand) 
                {
                    arg1 = offset;
                    CommandProc(pTrack, cmd, arg1, arg2);
                }
                break;
            }
            }

            break;
        }

        case 0xB0: 
        case 0xC0: 
        case 0xD0: 
        {
            u8 arg = ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                             useArgType ? argType : SEQ_ARG_U8);

            if (!doExecCommand) 
            {
                break;
            }

            switch (cmd) {
            case MmlCommand::MML_TRANSPOSE:
            case MmlCommand::MML_PITCH_BEND: 
            {
                arg1 = *reinterpret_cast<s8*>(&arg);
                break;
            }

            default: 
            {
                arg1 = arg;
                break;
            }
            }

            CommandProc(pTrack, cmd, arg1, arg2);
            break;
        }

        case 0x90: 
        {
            if (doExecCommand) 
            {
                CommandProc(pTrack, cmd, arg1, arg2);
            }
            break;
        }

        case 0xE0: 
        {
            arg1 = static_cast<s16>(
                ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                        useArgType ? argType : SEQ_ARG_S16));

            if (doExecCommand) 
            {
                CommandProc(pTrack, cmd, arg1, arg2);
            }
            break;
        }

        case 0xF0: {
            switch (cmd) {
            case MmlCommand::MML_ALLOC_TRACK: 
            {
                (void)Read16(&rTrackParam.currentAddr);
                break;
            }

            case MmlCommand::MML_FIN: 
            {
                return SequenceTrack::PARSE_RESULT_FINISH;
            }

            case MmlCommand::MML_EX_COMMAND: 
            {
                u32 cmdex = ReadByte(&rTrackParam.currentAddr);

                switch (cmdex & 0xf0) 
                {
                case 0xe0: 
                {
                    arg1 = static_cast<s16>(
                        ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                                useArgType ? argType : SEQ_ARG_S16));

                    if (doExecCommand) {
                        CommandProc(pTrack, (cmd << 8) + cmdex, arg1, arg2);
                    }
                    break;
                }

                case 0x80:
                case 0x90: 
                {
                    arg1 = ReadByte(&rTrackParam.currentAddr);
                    arg2 = static_cast<s16>(
                        ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                                useArgType ? argType : SEQ_ARG_S16));

                    if (doExecCommand) 
                    {
                        CommandProc(pTrack, (cmd << 8) + cmdex, arg1, arg2);
                    }
                    break;
                }
                }

                // FALLTHROUGH (assume no arguments?)
            }

            default: {
                if (doExecCommand) 
                {
                    CommandProc(pTrack, cmd, arg1, arg2);
                }
                break;
            }
            }

            break;
        }
        }
    }

    return SequenceTrack::PARSE_RESULT_CONTINUE;
}

void MmlParser::CommandProc(MmlSequenceTrack* track, u32 command, s32 commandArg1, s32 commandArg2) const
{
    NW_NULL_ASSERT(track);
    SequenceSoundPlayer* player = track->GetSequenceSoundPlayer();
    NW_NULL_ASSERT(player);
    SequenceTrack::ParserTrackParam& trackParam = track->GetParserTrackParam();
    SequenceSoundPlayer::ParserPlayerParam& playerParam = player->GetParserPlayerParam();

    if (command <= 0xff)
    {
        switch (command)
        {
        case MmlCommand::MML_TEMPO:
            playerParam.tempo = static_cast<u16>(ut::Clamp(static_cast<int>(commandArg1), TEMPO_MIN, TEMPO_MAX));
            break;

        case MmlCommand::MML_TIMEBASE:
            playerParam.timebase = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_PRG:
            if (commandArg1 < 0x10000)
            {
                trackParam.prgNo = static_cast<u16>(commandArg1);
            }
            else
            {
                NW_WARNING(false, "nw::snd::MmlParser: too large prg No. %d", commandArg1);
            }
            break;

        case MmlCommand::MML_MUTE:
            track->SetMute(static_cast<SeqMute>(commandArg1));
            break;

        case MmlCommand::MML_VOLUME:
            trackParam.volume.SetTarget(static_cast<u8>(commandArg1),
                static_cast<s16>(commandArg2));
            break;

        case MmlCommand::MML_VOLUME2:
            trackParam.volume2 = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_VELOCITY_RANGE:
            trackParam.velocityRange = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_MAIN_VOLUME:
            playerParam.volume = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_TRANSPOSE:
            trackParam.transpose = static_cast<s8>(commandArg1);
            break;

        case MmlCommand::MML_PITCH_BEND:
            trackParam.pitchBend.SetTarget(static_cast<s8>(commandArg1),
                static_cast<s16>(commandArg2));
            break;

        case MmlCommand::MML_BEND_RANGE:
            trackParam.bendRange = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_PAN:
            trackParam.pan.SetTarget(static_cast<s8>(commandArg1 - PAN_CENTER),
                static_cast<s16>(commandArg2));
            break;

        case MmlCommand::MML_INIT_PAN:
            trackParam.initPan = static_cast<s8>(commandArg1 - PAN_CENTER);
            break;

        case MmlCommand::MML_SURROUND_PAN:
            trackParam.surroundPan.SetTarget(static_cast<s8>(commandArg1),
                static_cast<s16>(commandArg2));
            break;

        case MmlCommand::MML_PRIO:
            trackParam.priority = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_NOTE_WAIT:
            trackParam.noteWaitFlag = (commandArg1 != 0);
            break;

        case MmlCommand::MML_FRONT_BYPASS:
            trackParam.frontBypassFlag = (commandArg1 != 0);
            break;

        case MmlCommand::MML_PORTA_TIME:
            trackParam.portaTime = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_MOD_DEPTH:
            trackParam.lfoParam.depth = static_cast<u8>(commandArg1) / 128.0f;
            break;

        case MmlCommand::MML_MOD_SPEED:
            trackParam.lfoParam.speed = static_cast<u8>(commandArg1) * 0.390625f;
            break;

        case MmlCommand::MML_MOD_TYPE:
            trackParam.lfoTarget = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_MOD_RANGE:
            trackParam.lfoParam.range = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_MOD_DELAY:
            trackParam.lfoParam.delay = static_cast<u32>(commandArg1 * 5);
            break;

        case MmlCommand::MML_SWEEP_PITCH:
            trackParam.sweepPitch = static_cast<f32>(commandArg1) / 64.0f;
            break;

        case MmlCommand::MML_ATTACK:
            trackParam.attack = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_DECAY:
            trackParam.decay = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_SUSTAIN:
            trackParam.sustain = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_RELEASE:
            trackParam.release = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_ENV_HOLD:
            trackParam.envHold = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_ENV_RESET:
            trackParam.attack = SequenceTrack::INVALID_ENVELOPE;
            trackParam.decay = SequenceTrack::INVALID_ENVELOPE;
            trackParam.sustain = SequenceTrack::INVALID_ENVELOPE;
            trackParam.release = SequenceTrack::INVALID_ENVELOPE;
            trackParam.envHold = SequenceTrack::INVALID_ENVELOPE;
            break;

        case MmlCommand::MML_DAMPER:
            trackParam.damperFlag = (static_cast<u8>(commandArg1) >= 64);
            break;

        case MmlCommand::MML_TIE:
            trackParam.tieFlag = (commandArg1 != 0);
            track->ReleaseAllChannel(-1);
            track->FreeAllChannel();
            break;

        case MmlCommand::MML_MONOPHONIC:
            trackParam.monophonicFlag = (commandArg1 != 0);
            if (trackParam.monophonicFlag)
            {
                track->ReleaseAllChannel(-1);
                track->FreeAllChannel();
            }
            break;

        case MmlCommand::MML_PORTA:
            trackParam.portaKey = static_cast<u8>(commandArg1 + trackParam.transpose);
            trackParam.portaFlag = true;
            break;

        case MmlCommand::MML_PORTA_SW:
            trackParam.portaFlag = (commandArg1 != 0);
            break;

        case MmlCommand::MML_LPF_CUTOFF:
            trackParam.lpfFreq = static_cast<f32>(commandArg1 - 64) / 64.0f;
            break;

        case MmlCommand::MML_BIQUAD_TYPE:
            trackParam.biquadType = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_BIQUAD_VALUE:
            trackParam.biquadValue = static_cast<f32>(commandArg1) / 127.0f;
            break;

        case MmlCommand::MML_BANK_SELECT:
            trackParam.bankIndex = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_FXSEND_A:
            trackParam.fxSend[AUX_BUS_A] = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_FXSEND_B:
            trackParam.fxSend[AUX_BUS_B] = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_MAINSEND:
            trackParam.mainSend = static_cast<u8>(commandArg1);
            break;

        case MmlCommand::MML_PRINTVAR:
            if (mPrintVarEnabledFlag) {
                const vs16* const varPtr = GetVariablePtr( player, track, commandArg1 );
                NN_LOG_( "#%08x[%d]: printvar %sVAR_%d(%d) = %d\n",
                    player,
                    track->GetPlayerTrackNo(),
                    (commandArg1 >= 32 )? "T": (commandArg1 >= 16)? "G": "",
                    (commandArg1 >= 32 )? commandArg1-32 : (commandArg1 >= 16) ? commandArg1-16: commandArg1,
                    commandArg1,
                    *varPtr);
            }
            break;

        case MmlCommand::MML_OPEN_TRACK:
        {
            SequenceTrack* newTrack = player->GetPlayerTrack( commandArg1 );
            if (newTrack == NULL)
            {
                NW_WARNING(false, "nw::snd::MmlParser: opentrack for not allocated track");
                break;
            }
            if (newTrack == track)
            {
                NW_WARNING(false, "nw::snd::MmlParser: opentrack for self track");
                break;
            }
            newTrack->Close();
            newTrack->SetSeqData(trackParam.baseAddr, commandArg2);
            newTrack->Open();
            break;
        }

        case MmlCommand::MML_JUMP:
            trackParam.currentAddr = trackParam.baseAddr + commandArg1;
            break;

        case MmlCommand::MML_CALL:
        {
            if (trackParam.callStackDepth >= SequenceTrack::CALL_STACK_DEPTH) 
            {
                NW_WARNING(false, "nw::snd::MmlParser: cannot 'call' because already too deep");
                break;
            }

            SequenceTrack::ParserTrackParam::CallStack* callStack = &trackParam.callStack[ trackParam.callStackDepth ];
            callStack->address = trackParam.currentAddr;
            callStack->loopFlag = false;
            trackParam.callStackDepth++;
            trackParam.currentAddr = trackParam.baseAddr + commandArg1;
            break;
        }

        case MmlCommand::MML_RET:
        {
            SequenceTrack::ParserTrackParam::CallStack* callStack = NULL;
            while(trackParam.callStackDepth > 0) 
            {
                trackParam.callStackDepth--;
                if (!trackParam.callStack[trackParam.callStackDepth].loopFlag) 
                {
                    callStack = &trackParam.callStack[trackParam.callStackDepth];
                    break;
                }
            }
            if (callStack == NULL) 
            {
                NW_WARNING(false, "nw::snd::MmlParser: unmatched sequence command 'ret'");
                break;
            }
            trackParam.currentAddr = callStack->address;
            break;
        }

        case MmlCommand::MML_LOOP_START:
        {
            if (trackParam.callStackDepth >= SequenceTrack::CALL_STACK_DEPTH) 
            {
                NW_WARNING(false, "nw::snd::MmlParser: cannot 'loop_start' because already too deep");
                break;
            }

            SequenceTrack::ParserTrackParam::CallStack* callStack = &trackParam.callStack[trackParam.callStackDepth];
            callStack->address = trackParam.currentAddr;
            callStack->loopCount = static_cast<u8>(commandArg1);
            callStack->loopFlag = true;
            trackParam.callStackDepth++;
            break;
        }

        case MmlCommand::MML_LOOP_END:
        {
            if (trackParam.callStackDepth == 0) 
            {
                NW_WARNING(false, "nw::snd::MmlParser: unmatched sequence command 'loop_end'");
                break;
            }

            SequenceTrack::ParserTrackParam::CallStack* callStack = & trackParam.callStack[trackParam.callStackDepth - 1];
            if (!callStack->loopFlag) 
            {
                NW_WARNING(false, "nw::snd::MmlParser: unmatched sequence command 'loop_end'");
                break;
            }

            u8 loop_count = callStack->loopCount;
            if (loop_count > 0) 
            {
                loop_count--;
                if (loop_count == 0) 
                {
                    trackParam.callStackDepth--;
                    break;
                }
            }

            callStack->loopCount = loop_count;
            trackParam.currentAddr = callStack->address;
            break;
        }
        }
    }
    else if(command <= 0xffff)
    {
        u32 cmd = command >> 8;
        u32 cmdex = command & 0xff;
        NW_ASSERT(cmd == MmlCommand::MML_EX_COMMAND);

        vs16* varPtr = NULL;
        if (((cmdex & 0xf0 ) == 0x80 ) || ((cmdex & 0xf0 ) == 0x90))
        {
            varPtr = GetVariablePtr(player, track, commandArg1);
            if (varPtr == NULL)
            {
                return;
            }
        }

        switch (cmdex)
        {
        case MmlCommand::MML_SETVAR:
            *varPtr = static_cast<s16>(commandArg2);
            break;

        case MmlCommand::MML_ADDVAR:
            *varPtr += static_cast<s16>(commandArg2);
            break;

        case MmlCommand::MML_SUBVAR:
            *varPtr -= static_cast<s16>(commandArg2);
            break;

        case MmlCommand::MML_MULVAR:
            *varPtr *= static_cast<s16>(commandArg2);
            break;

        case MmlCommand::MML_DIVVAR:
            if (commandArg2 != 0)
            {
                *varPtr /= static_cast<s16>(commandArg2);
            }
            break;

        case MmlCommand::MML_SHIFTVAR:
            if (commandArg2 >= 0)
            {
                *varPtr <<= commandArg2;
            }
            else
            {
                *varPtr >>= -commandArg2;
            }
            break;

        case MmlCommand::MML_RANDVAR:
        {
            bool minus_flag = false;
            s32 rand;

            if (commandArg2 < 0) 
            {
                minus_flag = true;
                commandArg2 = static_cast<s16>(-commandArg2);
            }

            rand = Util::CalcRandom();
            rand *= commandArg2 + 1;
            rand >>= 16;
            if (minus_flag) rand = -rand;
            *varPtr = static_cast<s16>(rand);
            break;
        }

        case MmlCommand::MML_ANDVAR:
            *varPtr &= commandArg2;
            break;

        case MmlCommand::MML_ORVAR:
            *varPtr |= commandArg2;
            break;

        case MmlCommand::MML_XORVAR:
            *varPtr ^= commandArg2;
            break;

        case MmlCommand::MML_NOTVAR:
            *varPtr = static_cast<s16>(~static_cast<u16>(commandArg2));
            break;

        case MmlCommand::MML_MODVAR:
            if (commandArg2 != 0) *varPtr %= commandArg2;
            break;

        case MmlCommand::MML_CMP_EQ:
            trackParam.cmpFlag = (*varPtr == commandArg2);
            break;

        case MmlCommand::MML_CMP_GE:
            trackParam.cmpFlag = (*varPtr >= commandArg2);
            break;

        case MmlCommand::MML_CMP_GT:
            trackParam.cmpFlag = (*varPtr > commandArg2);
            break;

        case MmlCommand::MML_CMP_LE:
            trackParam.cmpFlag = (*varPtr <= commandArg2);
            break;

        case MmlCommand::MML_CMP_LT:
            trackParam.cmpFlag = (*varPtr < commandArg2);
            break;

        case MmlCommand::MML_CMP_NE:
            trackParam.cmpFlag = (*varPtr != commandArg2);
            break;

        case MmlCommand::MML_USERPROC:
            player->CallSequenceUserprocCallback(static_cast<u16>(commandArg1), track);
            break;
        }
    }
}

void MmlParser::NoteOnCommandProc(MmlSequenceTrack* track, int key, int velocity, s32 length, bool tieFlag) const
{
    track->NoteOn(key, velocity, length, tieFlag);
}

u16 MmlParser::Read16(const u8** ptr) const
{
    u16 ret = ReadByte(ptr);
    ret <<= 8;
    ret |= ReadByte(ptr);
    return ret;
}

u32 MmlParser::Read24(const u8** ptr) const
{
    u32 ret = ReadByte(ptr);
    ret <<= 8;
    ret |= ReadByte(ptr);
    ret <<= 8;
    ret |= ReadByte(ptr);
    return ret;
}

s32 MmlParser::ReadVar(const u8** ptr) const
{
    s32 ret = 0;
    u8 b;
    int i;

    for(i = 0 ;; ++i) 
    {
        NW_ASSERT(i < 4);
        b = ReadByte(ptr);
        ret <<= 7;
        ret |= b & 0x7f;
        if (!(b & 0x80)) break;
    }

    return ret;
}

s32 MmlParser::ReadArg(const u8** ptr, SequenceSoundPlayer* player, SequenceTrack* track, SeqArgType argType) const
{
    s32 var = 0;

    switch (argType) 
    {
    case SEQ_ARG_U8:
        var = ReadByte(ptr);
        break;

    case SEQ_ARG_S16:
        var = Read16(ptr);
        break;

    case SEQ_ARG_VMIDI:
        var = ReadVar(ptr);
        break;

    case SEQ_ARG_VARIABLE: 
    {
        u8 varNo = ReadByte(ptr);
        const vs16* varPtr = GetVariablePtr(player, track, varNo);
        if (varPtr != NULL) 
        {
            var = *varPtr;
        }
        break;
    }

    case SEQ_ARG_RANDOM: 
    {
        s32 rand;
        s16 min;
        s16 max;

        min = static_cast<s16>(Read16(ptr));
        max = static_cast<s16>(Read16(ptr));

        rand = Util::CalcRandom();
        rand *= (max - min) + 1;
        rand >>= 16;
        rand += min;
        var = rand;
        break;
    }

    }

    return var;
}

vs16* MmlParser::GetVariablePtr(SequenceSoundPlayer* player, SequenceTrack* track, int varNo) const
{
    NW_MINMAX_ASSERT(varNo, 0, SequenceSoundPlayer::PLAYER_VARIABLE_NUM + SequenceSoundPlayer::GLOBAL_VARIABLE_NUM + SequenceTrack::TRACK_VARIABLE_NUM);

    if (varNo < SequenceSoundPlayer::PLAYER_VARIABLE_NUM
        + SequenceSoundPlayer::GLOBAL_VARIABLE_NUM)
    {
        return player->GetVariablePtr(varNo);
    }
    else if (varNo < SequenceSoundPlayer::PLAYER_VARIABLE_NUM
        + SequenceSoundPlayer::GLOBAL_VARIABLE_NUM + SequenceTrack::TRACK_VARIABLE_NUM)
    {
        return track->GetVariablePtr(varNo - SequenceSoundPlayer::PLAYER_VARIABLE_NUM - SequenceSoundPlayer::GLOBAL_VARIABLE_NUM);
    }
    else
    {
        return NULL;
    }
}

u32 MmlParser::ParseAllocTrack(const void* baseAddress, u32 seqOffset, u32* allocTrack)
{
    NW_NULL_ASSERT(baseAddress);
    NW_NULL_ASSERT(allocTrack);

    const u8* ptr = static_cast<const u8*>(ut::AddOffsetToPtr(baseAddress, seqOffset));
    if (*ptr != MmlCommand::MML_ALLOC_TRACK) 
    {
        *allocTrack = (1 << 0);
        return seqOffset;
    }
    else 
    {
        ++ptr;
        u32 tracks = *ptr;
        tracks <<= 8;
        ++ptr;
        tracks |= *ptr;
        *allocTrack = tracks;
        return seqOffset + 3;
    }
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw