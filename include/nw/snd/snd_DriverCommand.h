#ifndef NW_SND_DRIVER_COMMAND_H_
#define NW_SND_DRIVER_COMMAND_H_

#include <nw/snd/snd_StreamSoundFile.h>
#include <nw/snd/snd_StreamSoundFileReader.h>
#include <nw/snd/snd_SoundArchive.h>

namespace nw {

namespace io {
class FileStream;
}

namespace snd {

class FxBase;

namespace internal {

namespace driver {
class BasicSoundPlayer;
class SequenceSoundPlayer;
class SequenceTrackAllocator;
class NoteOnCallback;
class WaveSoundPlayer;
class StreamSoundPlayer;
class StreamBufferPool;
class DisposeCallback;
}

enum DriverCommandId
{
    DRIVER_COMMAND_DUMMY,
    DRIVER_COMMAND_DEBUG,
    DRIVER_COMMAND_REPLY,
    DRIVER_COMMAND_PLAYER_INIT,
    DRIVER_COMMAND_PLAYER_FINALIZE,
    DRIVER_COMMAND_PLAYER_START,
    DRIVER_COMMAND_PLAYER_STOP,
    DRIVER_COMMAND_PLAYER_PAUSE,
    DRIVER_COMMAND_PLAYER_PARAM,
    DRIVER_COMMAND_PLAYER_PANMODE,
    DRIVER_COMMAND_PLAYER_PANCURVE,
    DRIVER_COMMAND_PLAYER_FRONTBYPASS,
    DRIVER_COMMAND_SEQ_INIT,
    DRIVER_COMMAND_SEQ_SEQDATA,
    DRIVER_COMMAND_SEQ_BANKDATA,
    DRIVER_COMMAND_SEQ_SKIP,
    DRIVER_COMMAND_SEQ_TEMPORATIO,
    DRIVER_COMMAND_SEQ_CHANNELPRIO,
    DRIVER_COMMAND_SEQ_PRIOFIX,
    DRIVER_COMMAND_SEQ_USERPROC,
    DRIVER_COMMAND_SEQ_SETVAR,
    DRIVER_COMMAND_SEQ_SETGVAR,
    DRIVER_COMMAND_SEQ_SETTVAR,
    DRIVER_COMMAND_SEQTRACK_MUTE,
    DRIVER_COMMAND_SEQTRACK_SILENCE,
    DRIVER_COMMAND_SEQTRACK_VOLUME,
    DRIVER_COMMAND_SEQTRACK_PITCH,
    DRIVER_COMMAND_SEQTRACK_PAN,
    DRIVER_COMMAND_SEQTRACK_SPAN,
    DRIVER_COMMAND_SEQTRACK_LPF,
    DRIVER_COMMAND_SEQTRACK_PANRANGE,
    DRIVER_COMMAND_SEQTRACK_MODDEPTH,
    DRIVER_COMMAND_SEQTRACK_MODSPEED,
    DRIVER_COMMAND_SEQTRACK_BIQUAD,
    DRIVER_COMMAND_SEQTRACK_BANKINDEX,
    DRIVER_COMMAND_WAVESND_SETUP,
    DRIVER_COMMAND_WAVESND_CHANNELPRIO,
    DRIVER_COMMAND_WAVESND_PRIOFIX,
    DRIVER_COMMAND_STRM_INIT,
    DRIVER_COMMAND_STRM_SETUP,
    DRIVER_COMMAND_STRM_STOP,
    DRIVER_COMMAND_STRM_LOADHEADER,
    DRIVER_COMMAND_STRM_LOADDATA,
    DRIVER_COMMAND_STRM_TRACK_VOLUME,
    DRIVER_COMMAND_STRM_TRACK_PAN,
    DRIVER_COMMAND_STRM_TRACK_SPAN,
    DRIVER_COMMAND_INVALIDATE_DATA,
    DRIVER_COMMAND_REGIST_DISPOSE_CALLBACK,
    DRIVER_COMMAND_UNREGIST_DISPOSE_CALLBACK,
    DRIVER_COMMAND_APPEND_EFFECT,
    DRIVER_COMMAND_APPEND_EFFECT_SDK_DELAY,
    DRIVER_COMMAND_APPEND_EFFECT_SDK_REVERB,
    DRIVER_COMMAND_CLEAR_EFFECT,
    DRIVER_COMMAND_ALLVOICES_SYNC,
    DRIVER_COMMAND_SET_MASTER_VOLUME
};

struct DriverCommand
{
    struct DriverCommand* next;
    DriverCommandId id;
    u32 tag;
    u32 memory_next;
    
    static void ProcessCommandList(DriverCommand* commandList);
};

struct DriverCommandDebug : public DriverCommand
{
    char str[256];
};

struct DriverCommandReply : public DriverCommand
{
    bool* ptr;
};

struct DriverCommandPlayer : public DriverCommand
{
    driver::BasicSoundPlayer* player;
    bool flag;
};

struct DriverCommandPlayerInit : public DriverCommand
{
    driver::BasicSoundPlayer* player;
    bool* availableFlagPtr;
};

struct DriverCommandPlayerPanParam : public DriverCommand
{
    driver::BasicSoundPlayer* player;
    PanMode panMode;
    PanCurve panCurve;
};

struct DriverCommandPlayerParam : public DriverCommand
{
    driver::BasicSoundPlayer* player;
    float volume;
    float pitch;
    float pan;
    float surroundPan;
    float lpfFreq;
    int biquadFilterType;
    float biquadFilterValue;
    float mainSend;
    float fxSend[AUX_BUS_NUM];
};

struct DriverCommandSeqInit : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    driver::SequenceTrackAllocator* trackAllocator;
    u32 allocTracks;
    driver::NoteOnCallback* noteOnCallback;
};

struct DriverCommandSeqSetData : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    const void* seqBase;
    s32 seqOffset;
};
struct DriverCommandSeqSetBankData : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    const void* bankFiles[SoundArchive::SEQ_BANK_MAX];
};

struct DriverCommandSeqSkip : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    int offsetType;
    int offset;
};

struct DriverCommandSeqTempoRatio : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    f32 tempoRatio;
};

struct DriverCommandSeqChannelPrio : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    int priority;
};

struct DriverCommandSeqPrioFix : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    bool priorityFix;
};

struct DriverCommandSeqUserProc : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    uptr callback;
    void* arg;
};

struct DriverCommandSeqSetVar : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    int trackNo;
    int varNo;
    s32 var;
};

struct DriverCommandSeqTrack : public DriverCommand
{
    driver::SequenceSoundPlayer* player;
    u32 trackBitFlag;
};

struct DriverCommandSeqTrackMute : public DriverCommandSeqTrack
{
    int mute;
};

struct DriverCommandSeqTrackSilence : public DriverCommandSeqTrack
{
    bool silenceFlag;
    int fadeFrames;
};

struct DriverCommandSeqTrackParam : public DriverCommandSeqTrack
{
    f32 value;
};

struct DriverCommandSeqTrackBiquad : public DriverCommandSeqTrack
{
    int type;
    f32 value;
};

struct DriverCommandSeqTrackBankIndex : public DriverCommandSeqTrack
{
    int bankIndex;
};

struct DriverCommandWaveSoundSetup : public DriverCommand
{
    driver::WaveSoundPlayer* player;
    const void* wsdFile;
    s32 waveSoundOffset;
    int startOffsetType;
    s32 offset;
    const void* callback;
    u32 callbackData;
};

struct DriverCommandWaveSoundChannelPrio : public DriverCommand
{
    driver::WaveSoundPlayer* player;
    int priority;
};

struct DriverCommandWaveSoundPrioFix : public DriverCommand
{
    driver::WaveSoundPlayer* player;
    bool priorityFix;
};

struct DriverCommandStreamSoundInit : public DriverCommand
{
    driver::StreamSoundPlayer* player;
    driver::StreamBufferPool* pBufferPool;
    int allocChannelCount;
    u16 allocTrackFlag;
};

struct DriverCommandStreamSoundSetup : public DriverCommand
{
    driver::StreamSoundPlayer* player;
    io::FileStream* pFileStream;
    int startOffsetType;
    s32 offset;
};

struct DriverCommandStreamSoundLoadHeader : public DriverCommand
{
    static const int TRACK_MAX = 4;
    static const int CHANNEL_MAX = 8;
    
    driver::StreamSoundPlayer* player;
    StreamSoundFile::StreamSoundInfo streamInfo;
    StreamSoundFileReader::TrackInfo trackInfoArray[TRACK_MAX];
    DspAdpcmParam dspAdpcmParam[CHANNEL_MAX];
    DspAdpcmLoopParam dspAdpcmLoopParam[CHANNEL_MAX];
    u32 dataBlockOffset;
    u32 trackCount;
    u32 channelCount;
};

struct DriverCommandStreamSoundLoadData : public DriverCommand
{
    driver::StreamSoundPlayer* player;
    u32 bufferBlockIndex;
    u32 dataBlockIndex;
    u32 blockSamples;
    bool isDataLoopBlock;
    bool lastBlockFlag;
};

struct DriverCommandStreamSoundTrackParam : public DriverCommand
{
    driver::StreamSoundPlayer* player;
    unsigned long trackBitFlag;
    float value;
};

struct DriverCommandInvalidateData : public DriverCommand
{
    void* mem;
    unsigned long size;
};

struct DriverCommandDisposeCallback : public DriverCommand
{
    driver::DisposeCallback* callback;
};

struct DriverCommandEffect : public DriverCommand
{
    AuxBus bus;
    FxBase* effect;
    int fadeTimes;
};

struct DriverCommandEffectSdkDelay : public DriverCommand
{
    AuxBus bus;
    nn::snd::CTR::FxDelay* effect;
};

struct DriverCommandEffectSdkReverb : public DriverCommand
{
    AuxBus bus;
    nn::snd::CTR::FxReverb* effect;
};

struct DriverCommandAllVoicesSync : public DriverCommand
{
    u32 syncFlag;
};

} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_DRIVER_COMMAND_H_