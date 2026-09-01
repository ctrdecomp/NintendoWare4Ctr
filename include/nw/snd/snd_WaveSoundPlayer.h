#ifndef NW_SND_WAVE_SOUND_PLAYER_H_
#define NW_SND_WAVE_SOUND_PLAYER_H_

#include <nw/snd/snd_BasicSoundPlayer.h>
#include <nw/snd/snd_DisposeCallback.h>
#include <nw/snd/snd_CurveLfo.h>
#include <nw/snd/snd_WaveSoundFileReader.h>
#include <nw/snd/snd_Channel.h>
#include <nw/snd/snd_SoundThread.h>

namespace nw {
namespace snd {

struct WaveSoundDataInfo
{
    bool loopFlag;
    int sampleRate;
    u32 loopStart;
    u32 loopEnd;
};

namespace internal {
namespace driver {

class WaveSoundPlayer : public BasicSoundPlayer, public DisposeCallback, public SoundThread::PlayerCallback
{
public:
    static const int PAUSE_RELEASE_VALUE    = 127;
    static const int MUTE_RELEASE_VALUE     = 127;
    static const int DEFAULT_PRIORITY       = 64;

    enum StartOffsetType
    {
        START_OFFSET_TYPE_SAMPLE,
        START_OFFSET_TYPE_MILLISEC
    };


    struct WaveSoundCallbackArg
    {
        const void* wsdFile;
        int wsdIndex;
        int noteIndex;
        u32 callbackData;

        const PlayerHeapDataManager* dataMgr;
    };

    class WaveSoundCallback
    {
      public:
        virtual ~WaveSoundCallback() {}

        virtual bool GetWaveSoundData(WaveSoundInfo* info, WaveSoundNoteInfo* noteInfo, WaveInfo* waveData, const WaveSoundCallbackArg& arg) const = 0;
    };

public:
    WaveSoundPlayer();

    virtual void Initialize();
    virtual void Finalize();
    virtual void Start();
    virtual void Stop();
    virtual void Pause(bool flag);
    virtual void InvalidateData(const void* start, const void* end);
    virtual void OnUpdateFrameSoundThread() { Update(); }
    virtual void OnShutdownSoundThread() { Stop(); }


    bool Prepare(const void* waveSoundBase, int index, StartOffsetType startOffsetType, int startOffset, const WaveSoundCallback* callback,u32 callbackData);

    void SetPanRange(float panRange);
    void SetChannelPriority(int priority);
    void SetReleasePriorityFix(bool fix);

    float GetPanRange() const { return m_PanRange; }
    int GetChannelPriority() const { return m_Priority; }

    bool ReadWaveSoundDataInfo( WaveSoundDataInfo* info ) const;
    s32 GetPlaySamplePosition() const;

public:
    void DebugUpdate() { if (m_ActiveFlag) { Update(); } }

private:
    bool m_WavePlayFlag;
    bool m_ReleasePriorityFixFlag;

    f32 m_PanRange;
    u8 m_Priority;

    const WaveSoundCallback* m_pCallback;
    u32 m_CallbackData;

    const void* m_pWaveSoundData;
    int m_WaveSoundIndex;
    StartOffsetType m_StartOffsetType;
    int m_StartOffset;

    CurveLfoParam m_LfoParam;

    WaveSoundInfo m_WaveSoundInfo;

    Channel* m_pChannel;

    void FinishPlayer();
    void Update();
    bool IsChannelActive() const { return ( m_pChannel != NULL ) && m_pChannel->IsActive(); }
    bool StartChannel( const WaveSoundCallback* callback, u32 callbackData );
    void CloseChannel();
    void UpdateChannel();

    const void* GetWaveSoundDataAddress() const { return m_pWaveSoundData; }
    static void ChannelCallbackFunc(Channel* dropChannel,Channel::ChannelCallbackStatus status,u32 userData);
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_WAVE_SOUND_PLAYER_H_