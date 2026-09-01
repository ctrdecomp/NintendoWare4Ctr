#ifndef NW_SND_SEQUENCE_SOUND_H_
#define NW_SND_SEQUENCE_SOUND_H_

#include <nw/snd/snd_BasicSound.h>
#include <nw/snd/snd_BasicSoundPlayer.h>
#include <nw/snd/snd_SequenceSoundPlayer.h>
#include <nw/snd/snd_SoundInstanceManager.h>
#include <nw/snd/snd_Task.h>
#include <nw/snd/snd_PlayerHeapDataManager.h>

namespace nw { 
namespace snd {

class SoundDataManager;
class SequenceSoundHandle;

namespace internal {

namespace driver {

class NoteOnCallback;

} // namespace driver

class SequenceSound;

typedef SoundInstanceManager<SequenceSound> SequenceSoundInstanceManager;

class SequenceSound : public BasicSound
{
    friend class nw::snd::SequenceSoundHandle;

public:
    NW_UT_RUNTIME_TYPEINFO;

public:

    struct StartInfo
    {
        s32 seqOffset;
        driver::SequenceSoundPlayer::OffsetType startOffsetType;
        int startOffset;
    };

    struct LoadInfo
    {
        const SoundArchive* arc;
        const SoundDataManager* mgr;
        const LoadItemInfo* seq;
        const LoadItemInfo* banks;
    };

private:
    typedef void (*NotifyAsyncLoadFinished)(
        bool result,
        const LoadItemInfo* seq,
        const LoadItemInfo banks[],
        const LoadItemInfo warcs[],
        const bool warcIsIndividuals[],
        void* userData
    );

    class DataLoadTask : public Task
    {
    public:
        DataLoadTask();
        virtual void Execute();

        PlayerHeapDataManager* m_pDataManager;
        PlayerHeap* m_pAllocator;
        NotifyAsyncLoadFinished m_Callback;
        void* m_CallbackData;

        const SoundDataManager* m_pSoundDataManager;
        const SoundArchive* m_pSoundArchive;

        LoadItemInfo m_LoadInfoSeq;
        LoadItemInfo m_LoadInfoBanks[SoundArchive::SEQ_BANK_MAX];
    };

public:
    static const int BANK_INDEX_MIN = 0;
    static const int BANK_INDEX_MAX = 3;
    static const s8 TRANSPOSE_MIN = -64;
    static const s8 TRANSPOSE_MAX = 63;
    static const u8 VELOCITY_RANGE_MIN = 0;
    static const u8 VELOCITY_RANGE_MAX = 127;

    explicit SequenceSound(SequenceSoundInstanceManager& manager);

    void Setup(
        driver::SequenceTrackAllocator* trackAllocator,
        u32 allocTracks,
        driver::NoteOnCallback* callback
    );

    struct Resource
    {
        const void* seq;
        const void* banks[SoundArchive::SEQ_BANK_MAX];
        const void* warcs[SoundArchive::SEQ_BANK_MAX];
        bool warcIsIndividuals[SoundArchive::SEQ_BANK_MAX];

        Resource()
        {
            Initialize();
        }

        void Initialize()
        {
            seq = NULL;
            for (u32 i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
            {
                banks[i] = NULL;
                warcs[i] = NULL;
                warcIsIndividuals[i] = false;
            }
        }
    };

    void Prepare(const Resource& res, const StartInfo& startInfo);

    bool RegisterDataLoadTask(const LoadInfo& loadInfo, const StartInfo& startInfo);

    void Initialize() override;
    void Finalize() override;
    bool IsPrepared() const override { return m_PreparedFlag; }

    void SetTempoRatio(f32 tempoRatio);
    void SetChannelPriority(int priority);
    void SetReleasePriorityFix(bool fix);
    void SetSequenceUserprocCallback(SequenceUserprocCallback callback, void* arg);

    u32 GetTick() const;

    void SetTrackMute(u32 trackBitFlag, SeqMute mute);
    void SetTrackMute(u32 trackBitFlag, bool muteFlag);
    void SetTrackSilence(u32 trackBitFlag, bool silenceFlag, int fadeTimes);

    void SetTrackBiquadFilter(u32 trackBitFlag, int type, f32 value);
    void SetTrackBankIndex(u32 trackBitFlag, int bankIndex);

    void SetTrackVolume(u32 trackBitFlag, f32 volume);
    void SetTrackPitch(u32 trackBitFlag, f32 pitch);
    void SetTrackLpfFreq(u32 trackBitFlag, f32 lpfFreq);
    void SetTrackPanRange(u32 trackBitFlag, f32 panRange);
    void SetTrackModDepth(u32 trackBitFlag, f32 depth);
    void SetTrackModSpeed(u32 trackBitFlag, f32 speed);

    void SetTrackTranspose(u32 trackBitFlag, s8 transpose);
    void SetTrackVelocityRange(u32 trackBitFlag, u8 range);
    void SetTrackOutputLine(u32 trackBitFlag, u32 lineFlag);
    void ResetTrackOutputLine(u32 trackBitFlag);

    void SetTrackMainOutVolume(u32 trackBitFlag, f32 volume);
    void SetTrackPan(u32 trackBitFlag, f32 pan);
    void SetTrackSurroundPan(u32 trackBitFlag, f32 surroundPan);
    void SetTrackMainSend(u32 trackBitFlag, f32 send);
    void SetTrackFxSend(u32 trackBitFlag, AuxBus bus, f32 send);

    void SetTrackDrcOutVolume(u32 drcIndex, u32 trackBitFlag, f32 volume);
    void SetTrackDrcPan(u32 drcIndex, u32 trackBitFlag, f32 pan);
    void SetTrackDrcSurroundPan(u32 drcIndex, u32 trackBitFlag, f32 span);
    void SetTrackDrcMainSend(u32 drcIndex, u32 trackBitFlag, f32 send);
    void SetTrackDrcFxSend(u32 drcIndex, u32 trackBitFlag, AuxBus bus, f32 send);

    bool ReadVariable(int varNo, s16* var) const;
    static bool ReadGlobalVariable(int varNo, s16* var);
    bool ReadTrackVariable(int trackNo, int varNo, s16* var) const;
    void WriteVariable(int varNo, s16 var);
    static void WriteGlobalVariable(int varNo, s16 var);
    void WriteTrackVariable(int trackNo, int varNo, s16 var);

protected:
    bool IsAttachedTempSpecialHandle() override;
    void DetachTempSpecialHandle() override;

    driver::BasicSoundPlayer* GetBasicSoundPlayerHandle() override { return &m_SequenceSoundPlayerInstance; }

    void OnUpdatePlayerPriority() override;
    void OnUpdate() override { CheckLoadState(); }

private:
    void Skip(driver::SequenceSoundPlayer::OffsetType offsetType, int offset);

    static void NotifyAsyncLoadFinishedFunc(
        bool result,
        const LoadItemInfo* seq,
        const LoadItemInfo banks[],
        const LoadItemInfo warcs[],
        const bool warcIsIndividuals[],
        void* userData
    );

    void CheckLoadState();

    driver::SequenceSoundPlayer m_SequenceSoundPlayerInstance;
    SequenceSoundHandle* m_pTempSpecialHandle;
    SequenceSoundInstanceManager& m_Manager;

    StartInfo m_StartInfo;

    DataLoadTask m_DataLoadTask;
    PlayerHeapDataManager m_DataManager;

    int m_TaskCommandId;

    Resource m_Res;

    s8 m_LoadState;
    bool m_InitializeFlag;
    bool m_PreparedFlag;
};


} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_SEQUENCE_SOUND_H_
