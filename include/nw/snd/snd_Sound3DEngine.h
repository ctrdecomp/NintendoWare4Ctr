#ifndef NW_SND_SOUND_3D_ENGINE_H_
#define NW_SND_SOUND_3D_ENGINE_H_

#include <nw/snd/snd_Sound3DManager.h>
#include <nw/snd/snd_Sound3DCalculator.h>

namespace nw {
namespace snd {

class Sound3DEngine : public internal::ISound3DEngine
{
public:
    Sound3DEngine();
    virtual ~Sound3DEngine() {}
    virtual void UpdateAmbientParam(const Sound3DManager* sound3DManager, const Sound3DParam* sound3DParam, u32 soundId, u32 updateFlag, SoundAmbientParam* ambientParam);
    virtual int GetAmbientPriority(const Sound3DManager* sound3DManager, const Sound3DParam* sound3DParam, u32 soundId);
    virtual void UpdateAmbientParam(const Sound3DManager* sound3DManager, const Sound3DParam* sound3DParam, u32 soundId, SoundAmbientParam* ambientParam);
private:
    Sound3DCalculator::CalcPanParam m_CalcPanParam;
};

} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_3D_ENGINE_H_