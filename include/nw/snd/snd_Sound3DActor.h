#ifndef NW_SND_SOUND_3D_ACTOR_H_
#define NW_SND_SOUND_3D_ACTOR_H_

#include <nw/snd/snd_SoundActor.h>
#include <nw/snd/snd_BasicSound.h>
#include <nw/math/math_Types.h>

namespace nw {
namespace snd {

class Sound3DManager;
class SoundArchivePlayer;
class SoundHandle;

class Sound3DActor : public SoundActor, public internal::BasicSound::AmbientArgUpdateCallback
{
public:
    Sound3DActor();
    virtual ~Sound3DActor();
    virtual SoundStartable::StartResult SetupSound(SoundHandle* handle, u32 soundId, const StartInfo* startInfo, void* setupArg);
    virtual void detail_UpdateAmbientArg(void* arg, const internal::BasicSound* sound);

    void Initialize(SoundArchivePlayer& player, Sound3DManager& manager);
    void Finalize();

    const nw::math::VEC3& GetPosition() const { return m_Position; }
    const nw::math::VEC3& GetVelocity() const { return m_Velocity; }

    void SetUserParam( u32 param ) { m_UserParam = param; }
    u32 GetUserParam() const { return m_UserParam; }

private:
    static void ClearUpdateCallback(SoundHandle& handle);
    
    Sound3DManager* m_p3dManager;
    SoundArchivePlayer* m_pArchivePlayer;
    u32 m_UserParam;
    nw::math::VEC3 m_Position;
    nw::math::VEC3 m_Velocity;
    bool m_ResetPositionFlag;
    bool m_IsInitialized;
    bool m_IsFinalized;
};

} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_3D_ACTOR_H_