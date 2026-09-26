#ifndef NW_SND_STREAM_SOUND_PLAYER_H_
#define NW_SND_STREAM_SOUND_PLAYER_H_

#include <nw/io/io_FileStream.h>
#include <nw/snd/snd_SoundThread.h>
#include <nw/snd/snd_BasicSoundPlayer.h>
#include <nw/snd/snd_StreamSoundFileReader.h>
#include <nw/snd/snd_StreamTrack.h>
#include <nw/snd/snd_InstancePool.h>
#include <nw/snd/snd_Task.h>
#include <nw/snd/snd_Util.h>

namespace nw {
namespace snd {

struct StreamDataInfo
{
    bool loopFlag;
    int sampleRate;
    u32 loopStart;
    u32 loopEnd;
};

namespace internal {
namespace driver {

class StreamBufferPool;

class StreamSoundPlayer : public BasicSoundPlayer, public SoundThread::PlayerCallback
{
public:
    static const u32 DATA_BLOCK_SIZE_MAX = 8 * 1024;
    static const u32 STRM_TRACK_NUM = 4;

    enum SetupResult
    {
        SETUP_SUCCESS = 0,
        SETUP_ERR_CANNOT_ALLOCATE_BUFFER,
        SETUP_ERR_UNKNOWN
    };

    enum StartOffsetType
    {
        START_OFFSET_TYPE_SAMPLE,
        START_OFFSET_TYPE_MILLISEC
    };

    StreamSoundPlayer();
    virtual ~StreamSoundPlayer();
    virtual void Initialize();
    virtual void Finalize();
    virtual void Start();
    virtual void Stop();
    virtual void Pause(bool flag);

    SetupResult Setup(StreamBufferPool* pBufferPool,u32 allocChannelCount,u16 allocTrackFlag);


    bool Prepare(io::FileStream* pFileStream, StartOffsetType startOffsetType, int startOffset);

    bool IsSuspendByLoadingDelay() const { return m_LoadWaitFlag; }

    bool IsPrepared() const { return m_IsPrepared; }

    void SetTrackVolume(u32 trackBitFlag, float volume);
    void SetTrackPan(u32 trackBitFlag, float pan);
    void SetTrackSurroundPan(u32 trackBitFlag, float span);

    bool ReadStreamDataInfo(StreamDataInfo* info) const;
    long GetPlayLoopCount() const { return m_ActiveFlag ? m_LoopCounter : -1; }
    long GetPlaySamplePosition() const;
    float GetFilledBufferPercentage() const;

    StreamTrack* GetPlayerTrack(int trackNo);
    const StreamTrack* GetPlayerTrack(int trackNo) const;

    bool LoadHeader(const StreamSoundFile::StreamSoundInfo& streamInfo,const StreamSoundFileReader::TrackInfo trackInfos[],const DspAdpcmParam dspAdpcmParam[],
        const DspAdpcmLoopParam dspAdpcmLoopParam[],u32 dataBlockOffset,u32 trackCount,u32 channelCount);

    bool LoadStreamData(int bufferBlockIndex, int dataBlockIndex, u32 blockSamples, bool isDataLoopBlock, bool lastBlockFlag);

protected:
    virtual void OnUpdateFrameSoundThread() { Update(); }
    virtual void OnUpdateVoiceSoundThread() { UpdateBuffer(); }
    virtual void OnShutdownSoundThread() { Stop(); }

private:
    static const u32 STRM_CHANNEL_NUM = 8;
    static const u32 STRM_CHANNEL_NUM_PER_TRACK = 2;

    static const u32 BUFFER_BLOCK_COUNT_MAX = 32;
    static const u32 LOAD_BUFFER_CHANNEL_NUM = 2;
    static const u32 LOAD_BUFFER_SIZE = DATA_BLOCK_SIZE_MAX * LOAD_BUFFER_CHANNEL_NUM;

    class StreamHeaderLoadTask : public Task
    {
      public:
        StreamHeaderLoadTask();
        virtual void Execute();

      public:
        StreamSoundPlayer* m_PlayerHandle;
        io::FileStream* m_pFileStream;
        StreamSoundPlayer::StartOffsetType m_StartOffsetType;
        s32 m_StartOffset;
        
      private:
        bool LoadHeader();
    };

    friend class StreamHeaderLoadTask;


    class StreamDataLoadTask : public Task
    {
      public:
        StreamDataLoadTask();
        virtual void Execute();

      public:
        StreamSoundPlayer* m_PlayerHandle;
        io::FileStream* m_pFileStream;

        void*   m_BufferAddress[ STRM_CHANNEL_NUM ];
        u32     m_ChannelCount;
        s32     m_Offset;
        size_t  m_BlockBytes;
        s32     m_BufferBlockIndex;
        u32     m_BlockSamples;
        u32     m_DataBlockSize;
        u32     m_LoadingDataBlockIndex;
        bool    m_IsDataLoopBlock;
        bool    m_LastBlockFlag;

        ut::LinkListNode m_Link;

      private:
        bool LoadStreamData();
    };

    friend class StreamDataLoadTask;

    typedef ut::LinkList<StreamDataLoadTask, offsetof(StreamDataLoadTask,m_Link)>StreamDataLoadTaskList;

    bool SetupPlayer();
    
    void Update();
    void UpdateTask();
    void UpdateBuffer();
    void UpdateVoiceParams(StreamTrack* track);

    bool AllocVoices();
    void FreeVoices();
    bool AllocStreamBuffers();
    void FreeStreamBuffers();

    void UpdatePlayingBlockIndex();
    void UpdateLoadingBlockIndex();
    void UpdatePauseStatus();

    void SetAdpcmLoopContext(int channelNum, u16 predScale[]);

    void* GetBuffer(int channelNum);

    void SetTaskErrorFlag() { m_IsTaskError = true; }


    bool CheckDiskDriveError() const;
    
    int CalcLoadingBufferBlockCount() const;
    bool CalcStartOffset(s32* pStartBlockIndex,u32* pStartBlockOffset,s32* pLoopCount);

    static void VoiceCallbackFunc(Voice* voice,Voice::VoiceCallbackStatus status,void* arg);

    StreamChannel* GetTrackChannel(const StreamTrack& track, int channelIndex);

    s32 CalcLoadOffset() const;

    bool IsDspAdpcm() const;
    
    StreamSoundFile::StreamSoundInfo m_StreamInfo;

    bool m_IsInitialized;
    bool m_IsPrepared;
    bool m_IsTaskError;
    bool m_IsLoadingDelay;
    bool m_PauseStatus;
    bool m_LoadWaitFlag;
    bool m_IsNoRealtimeLoad;
    bool m_SkipUpdateAdpcmLoop;
    bool m_ValidAdpcmLoop;

    bool m_PlayFinishFlag;
    bool m_LoadFinishFlag;

    s32 m_LoopCounter;

    int m_PrepareCounter;
    int m_ChangeNumBlocks;

    int m_DataBlockSize;

    u32 m_BufferBlockCount;
    int m_BufferBlockCountBase;

    int m_LoadingBufferBlockCount;
    int m_LoadingBufferBlockIndex;
    int m_LoadingDataBlockIndex;

    int m_PlayingBufferBlockCount;

    int m_PlayingBufferBlockIndex;
    int m_PlayingDataBlockIndex;

    int m_LoopStartBlockIndex;
    int m_LastBlockIndex;

    u32 m_DataOffsetFromFileHead;

    volatile int m_LoadWaitCount;
    
    StartOffsetType m_StartOffsetType;
    int m_StartOffset;

    StreamHeaderLoadTask m_StreamHeaderLoadTask;
    StreamDataLoadTaskList m_StreamDataLoadTaskList;
    InstancePool<StreamDataLoadTask> m_StreamDataLoadTaskPool;
    StreamDataLoadTask m_StreamDataLoadTaskArea[BUFFER_BLOCK_COUNT_MAX];

    StreamBufferPool* m_pBufferPool;
    io::FileStream* m_pFileStream;

    s32 m_TrackCount;
    s32 m_ChannelCount;
    StreamChannel m_Channels[STRM_CHANNEL_NUM];
    StreamTrack m_Tracks[STRM_TRACK_NUM];

    static u8 s_LoadBuffer[LOAD_BUFFER_SIZE];
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_STREAM_SOUND_PLAYER_H_