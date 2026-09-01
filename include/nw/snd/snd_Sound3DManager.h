#ifndef NW_SND_SOUND_3D_MANAGER_H_
#define NW_SND_SOUND_3D_MANAGER_H_

#include <nw/math/math_Types.h>
#include <nw/ut/ut_LinkList.h>
#include <nw/snd/snd_BasicSound.h>
#include <nw/snd/snd_InstancePool.h>
#include <nw/snd/snd_Sound3DListener.h>

namespace nw {
namespace snd {

class SoundArchive;
class Sound3DManager;
class Sound3DEngine;

struct Sound3DParam
{
    math::VEC3 position;
    math::VEC3 velocity;

    u32 ctrlFlag;
    u32 actorUserParam;
    u32 soundUserParam; 
    f32 decayRatio;
    u8  decayCurve;
    u8  dopplerFactor;

    Sound3DParam();
};

class Sound3DManager : 
    public internal::BasicSound::AmbientParamUpdateCallback, 
    public internal::BasicSound::AmbientArgAllocatorCallback
{
public:
    typedef internal::InstancePool<Sound3DParam> Sound3DParamPool;
    typedef ut::LinkList<Sound3DListener, offsetof(Sound3DListener,m_LinkNode)> ListenerList;
public:
    Sound3DManager();

    size_t GetRequiredMemSize(const SoundArchive* archive);

    bool Initialize(const SoundArchive* archive, void* buffer, size_t size);
    bool Finalize();

    void AddListener(Sound3DListener* listener) { m_ListenerList.PushBack(listener); }
    void RemoveListener(Sound3DListener* listener) { m_ListenerList.Erase(listener); }

    const ListenerList& GetListenerList() const { return m_ListenerList; }

    void SetEngine(Sound3DEngine* pEngine);

    void SetMaxPriorityReduction(int maxPriorityReduction){ m_MaxPriorityReduction = maxPriorityReduction; }
    int GetMaxPriorityReduction() const{ return m_MaxPriorityReduction; }

    void SetPanRange(f32 panRange) { m_PanRange = panRange; }
    f32 GetPanRange() const { return m_PanRange; }

    void SetSonicVelocity(f32 sonicVelocity) { m_SonicVelocity = sonicVelocity; }
    f32 GetSonicVelocity() const { return m_SonicVelocity; }

    void SetBiquadFilterType(int type);
    int GetBiquadFilterType() const { return m_BiquadFilterType; }

private:
    virtual void detail_UpdateAmbientParam(const void* arg, u32 soundId, SoundAmbientParam* param);
    virtual int detail_GetAmbientPriority(const void* arg, u32 soundId);
    virtual void* detail_AllocAmbientArg(size_t argSize);
    virtual void detail_FreeAmbientArg(void* arg, const internal::BasicSound* sound);

    Sound3DParamPool m_ParamPool;
    ListenerList m_ListenerList;
    internal::ISound3DEngine* m_pSound3DEngine;

    s32 m_MaxPriorityReduction;
    f32 m_PanRange;
    f32 m_SonicVelocity;
    s32 m_BiquadFilterType;

    void* m_pSoundParamPoolBuffer;
    size_t m_SoundParamPoolBufferSize;
    s32 m_FreeMemSizeAfterCheking;
    bool m_IsInitialized;
};

namespace internal {

// found from M & L BISDX RTTI

class ISound3DEngine
{
public:
    virtual ~ISound3DEngine() {}
    virtual void UpdateAmbientParam(const Sound3DManager* manager, const Sound3DParam* actorParam, u32 soundId, SoundAmbientParam* param) = 0;
    virtual int GetAmbientPriority(const Sound3DManager* manager, const Sound3DParam* actorParam, u32 soundId) = 0;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_3D_MANAGER_H_