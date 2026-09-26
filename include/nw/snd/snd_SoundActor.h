#ifndef NW_SND_SOUND_ACTOR_H_
#define NW_SND_SOUND_ACTOR_H_

#include <nw/snd/snd_SoundStartable.h>
#include <nw/snd/snd_ExternalSoundPlayer.h>
#include <nw/snd/snd_BasicSound.h>

namespace nw { 
namespace snd {

class SoundArchivePlayer;

class SoundActor : public SoundStartable
{
    typedef internal::ExternalSoundPlayer ActorPlayer;

public:
    static const int ACTOR_PLAYER_COUNT = 4;

    SoundActor(SoundArchivePlayer& soundArchivePlayer);
    SoundActor();
    virtual ~SoundActor();
    virtual SoundStartable::StartResult SetupSound(SoundHandle* handle, u32 soundId, const SoundStartable::StartInfo* startInfo, void* setupArg);

    void Initialize(SoundArchivePlayer& soundArchivePlayer);
    void Finalize();

public:
    void StopAllSound(int fadeFrames);
    void PauseAllSound(bool flag, int fadeFrames);

public:
    void SetVolume(f32 volume)
    {
        m_ActorParam.volume = volume;
    }

    f32 GetVolume() const
    {
        return m_ActorParam.volume;
    }

    void SetPitch(f32 pitch)
    {
        m_ActorParam.pitch = pitch;
    }

    f32 GetPitch() const
    {
        return m_ActorParam.pitch;
    }

    int GetPlayingSoundCount(int actorPlayerId) const;

    void SetPlayableSoundCount(int actorPlayerId, int count);
    int GetPlayableSoundCount(int actorPlayerId) const;

    template <class Function>
    void ForEachSoundRef(Function& function, bool reverse = false);

    template <class Function>
    Function ForEachSound(Function function, bool reverse = false);

    ActorPlayer* detail_GetActorPlayer(int actorPlayerId)
    {
        if (actorPlayerId < 0 || ACTOR_PLAYER_COUNT <= actorPlayerId)
            return NULL;

        return &m_ActorPlayer[actorPlayerId];
    }

    const internal::SoundActorParam& detail_GetActorParam() const
    {
        return m_ActorParam;
    }

    virtual SoundStartable::StartResult detail_SetupSoundWithAmbientInfo(
        SoundHandle* handle,
        u32 soundId,
        const SoundStartable::StartInfo* startInfo,
        internal::BasicSound::AmbientInfo* ambientInfo,
        void* setupArg
    );

private:
    SoundStartable::StartResult detail_SetupSound(
        SoundHandle* handle,
        u32 soundId,
        bool holdFlag,
        const SoundStartable::StartInfo* startInfo
    ) override;
    SoundArchive::ItemId detail_GetItemId(const char* pString) override;

    struct SetupInfo
    {
        bool holdFlag;
    };

    SoundArchivePlayer* m_pSoundArchivePlayer;
    ActorPlayer m_ActorPlayer[ACTOR_PLAYER_COUNT];
    internal::SoundActorParam m_ActorParam;

    bool m_IsInitialized;
    bool m_IsFinalized;
};

template <class Function>
inline void SoundActor::ForEachSoundRef(Function& function, bool reverse)
{
    for (int actorPlayerIndex = 0; actorPlayerIndex < ACTOR_PLAYER_COUNT; actorPlayerIndex++)
        m_ActorPlayer[actorPlayerIndex].ForEachSoundRef<Function>(function, reverse);
}

template <class Function>
inline Function SoundActor::ForEachSound(Function function, bool reverse)
{
    for (int actorPlayerIndex = 0; actorPlayerIndex < ACTOR_PLAYER_COUNT; actorPlayerIndex++)
        m_ActorPlayer[actorPlayerIndex].ForEachSound<Function>(function, reverse);

    return function;
}

} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_ACTOR_H_
