#ifndef NW_SND_SOUND_3D_LISTENER_H_
#define NW_SND_SOUND_3D_LISTENER_H_

#include <nw/math/math_Types.h>
#include <nw/ut/ut_LinkList.h>

namespace nw {
namespace snd {

class Sound3DListener
{
public:
    Sound3DListener();

    void SetMatrix(const nw::math::MTX34& mtx);
    void ResetMatrix();
    const nw::math::MTX34& GetMatrix() const { return m_Mtx; }
    void CalcPositionFromMatrix(const nw::math::MTX34& mtx, nw::math::VEC3* pos);

    const nw::math::VEC3& GetPosition() const { return m_Position; }

    void SetVelocity(const nw::math::VEC3& velocity);
    const nw::math::VEC3& GetVelocity() const { return m_Velocity; }

    void SetInteriorSize(f32 interiorSize);
    f32 GetInteriorSize() const { return m_InteriorSize; }

    void SetMaxVolumeDistance(f32 maxVolumeDistance);
    f32 GetMaxVolumeDistance() const { return m_MaxVolumeDistance; }

    void SetUnitDistance(f32 unitDistance);
    f32 GetUnitDistance() const { return m_UnitDistance; }

    void SetUserParam(u32 param) { m_UserParam = param; }
    u32 GetUserParam() const { return m_UserParam; }

    void SetUnitBiquadFilterValue(f32 value);
    f32 GetUnitBiquadFilterValue() const { return m_UnitBiquadFilterValue; }

    void SetMaxBiquadFilterValue(f32 value);
    f32 GetMaxBiquadFilterValue() const { return m_MaxBiquadFilterValue; }
private:
    nw::math::MTX34 m_Mtx;
    nw::math::VEC3 m_Position;
    nw::math::VEC3 m_Velocity;
    f32 m_InteriorSize;
    f32 m_MaxVolumeDistance;
    f32 m_UnitDistance;
    u32 m_UserParam;
    f32 m_UnitBiquadFilterValue;
    f32 m_MaxBiquadFilterValue;
    bool m_ResetMatrixFlag;
public:
    ut::LinkListNode m_LinkNode;
};

} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_3D_LISTENER_H_