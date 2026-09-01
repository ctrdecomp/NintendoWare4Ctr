#ifndef NW_SND_SOUND_ARCHIVE_PLAYER_H_
#define NW_SND_SOUND_ARCHIVE_PLAYER_H_

#include <nw/snd/snd_SoundStartable.h>
#include <nw/snd/snd_NoteOnCallback.h>
#include <nw/snd/snd_SequenceSoundPlayer.h>
#include <nw/snd/snd_SequenceSound.h>
#include <nw/snd/snd_WaveSound.h>
#include <nw/snd/snd_StreamSound.h>
#include <nw/snd/snd_MmlSequenceTrackAllocator.h>
#include <nw/snd/snd_MmlParser.h>

namespace nw { 
namespace snd {

class SoundPlayer;
class SoundDataManager;

namespace internal {

class PlayerHeap;
class SoundArchiveFilesHook;

} // namespace internal

class SoundArchivePlayer : public SoundStartable
{
private:
    class SequenceNoteOnCallback : public internal::driver::NoteOnCallback
    {
        NW_DISALLOW_COPY_AND_ASSIGN(SequenceNoteOnCallback);

    public:
        SequenceNoteOnCallback(): 
            m_pSoundArchivePlayer(NULL)
        {
        }

        void Initialize(const SoundArchivePlayer& player)
        {
            m_pSoundArchivePlayer = &player;
        }

        virtual internal::driver::Channel* NoteOn(internal::driver::SequenceSoundPlayer* seqPlayer, u8 bankIndex, const internal::driver::NoteOnInfo& noteOnInfo);
    private:
        const SoundArchivePlayer* m_pSoundArchivePlayer;
    };

    friend class SoundArchivePlayer::SequenceNoteOnCallback;

public:
    SoundArchivePlayer();
    virtual ~SoundArchivePlayer();
    virtual SoundArchive::ItemId detail_GetItemId(const char* pString){ return m_pSoundArchive->GetItemId(pString); }

    size_t GetRequiredMemSize(const SoundArchive* arc, size_t userParamSizePerSound = 0) const;
    size_t GetRequiredStreamBufferSize(const SoundArchive* arc) const;

    bool Initialize(const SoundArchive* arc, const SoundDataManager* manager, void* buffer, u32 size, void* strmBuffer, u32 strmBufferSize, size_t userParamSizePerSound = 0);

    size_t GetRequiredStreamCacheSize(const SoundArchive* arc, size_t cacheSizePerSound) const;


    void Finalize();
    bool IsAvailable() const;
    void Update();

    const SoundArchive& GetSoundArchive() const;

    SoundPlayer& GetSoundPlayer(SoundArchive::ItemId playerId);
    const SoundPlayer& GetSoundPlayer(SoundArchive::ItemId playerId) const;

    StartResult detail_SetupSoundImpl(SoundHandle* handle, u32 soundId, internal::BasicSound::AmbientInfo* ambientArgInfo,
        SoundActor* actor, bool holdFlag, const StartInfo* startInfo);

protected:
    virtual StartResult detail_SetupSound(SoundHandle* handle, u32 soundId, bool holdFlag, const StartInfo* startInfo);
private:
    StartResult PrepareSequenceSoundImpl(
        internal::SequenceSound* sound, const SoundArchive::SoundInfo* commonInfo,
        const SoundArchive::SequenceSoundInfo* info, SoundStartable::StartInfo::StartOffsetType startOffsetType,
        int startOffset, const StartInfo::SeqSoundInfo* externalSeqInfo
    );

    StartResult PrepareStreamSoundImpl(internal::StreamSound* sound, const SoundArchive::SoundInfo* commonInfo, const SoundArchive::StreamSoundInfo* info,
        SoundStartable::StartInfo::StartOffsetType startOffsetType, int startOffset);

    StartResult PrepareWaveSoundImpl(internal::WaveSound* sound, const SoundArchive::SoundInfo* commonInfo, const SoundArchive::WaveSoundInfo* info,
        SoundStartable::StartInfo::StartOffsetType startOffsetType, int startOffset);

    internal::PlayerHeap* CreatePlayerHeap(void** ppBuffer, const void* pEndAddress, size_t heapSize);
    bool SetupMram(const SoundArchive* arc, void* buffer, size_t size, size_t userParamSizePerSound);
    bool SetupSoundPlayer(const SoundArchive* arc, void** buffer, const void* endp);
    bool SetupSequenceSound(int numSounds, void** buffer, const void* endp);
    bool SetupSequenceTrack(int numTracks, void** buffer, const void* endp);
    bool SetupWaveSound(int numSounds, void** buffer, const void* endp);
    bool SetupStreamSound(int numSounds, void** buffer, const void* endp);
    bool SetupStreamBuffer(const SoundArchive* arc, void* buffer, size_t size);
    bool SetupUserParamForBasicSound(const SoundArchive::SoundArchivePlayerInfo& info, void** buffer, const void* endp, size_t userParamSizePerSound);
    void SetCommonSoundParam(internal::BasicSound* sound, const SoundArchive::SoundInfo* commonInfo);
private:

    const SoundArchive* m_pSoundArchive;
    SequenceNoteOnCallback m_SequenceCallback;
    SequenceUserprocCallback m_SequenceUserprocCallback;
    void* m_pSequenceUserprocCallbackArg;
    u32 m_SoundPlayerCount;
    SoundPlayer* m_pSoundPlayers;
    internal::SequenceSoundInstanceManager m_SequenceSoundInstanceManager;
    internal::WaveSoundInstanceManager m_WaveSoundInstanceManager;
    internal::StreamSoundInstanceManager m_StreamSoundInstanceManager;
    internal::driver::SequenceTrackAllocator* m_pSequenceTrackAllocator;
    internal::driver::MmlSequenceTrackAllocator m_MmlSequenceTrackAllocator;
    internal::driver::StreamBufferPool m_StreamBufferPool;
    internal::driver::MmlParser m_MmlParser;
    void* m_pSetupBufferAddress;
    u32 m_SetupBufferSize;
    size_t m_SoundUserParamSize;
    const SoundDataManager* m_pSoundDataManager;
    internal::SoundArchiveFilesHook* m_pSoundArchiveFilesHook;
    bool m_IsEnableWarningPrint;
};


} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_ARCHIVE_PLAYER_H_
