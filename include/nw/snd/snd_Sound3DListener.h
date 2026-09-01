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
    const nw::math::MTX34& GetMatrix() const { return m_Mtx; }

    void ResetMatrix();
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