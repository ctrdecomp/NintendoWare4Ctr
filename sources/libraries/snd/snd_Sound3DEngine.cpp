// Filename: snd_Sound3DEngine.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Sound3DListener.h>

namespace nw {
namespace snd {

Sound3DListener::Sound3DListener(): 
    m_Position(0.0f, 0.0f, 0.0f),
    m_Velocity(0.0f, 0.0f, 0.0f),
    m_InteriorSize(1.0f),
    m_MaxVolumeDistance(1.0f),
    m_UnitDistance(1.0f),
    m_UserParam(0),
    m_UnitBiquadFilterValue(0.5f),
    m_MaxBiquadFilterValue(1.0f),
    m_ResetMatrixFlag(true)
{
    nw::math::MTX34Zero(&m_Mtx);
}

void Sound3DListener::SetMatrix(const nw::math::MTX34& mtx)
{
    nw::math::VEC3 oldPos = m_Position;
    CalcPositionFromMatrix(mtx, &m_Position);
    
    if (m_ResetMatrixFlag) 
    {
        m_Mtx = mtx;
        m_ResetMatrixFlag = false;
    }
    else 
    {
        m_Mtx = mtx;
        nw::math::VEC3Sub(&m_Velocity, &m_Position, &oldPos);
    }
}

void Sound3DListener::CalcPositionFromMatrix(const nw::math::MTX34& mtx, nw::math::VEC3* pos)
{
    pos->x = -(mtx.f._00 * mtx.f._03 + mtx.f._10 * mtx.f._13 + mtx.f._20 * mtx.f._23);
    pos->y = -(mtx.f._01 * mtx.f._03 + mtx.f._11 * mtx.f._13 + mtx.f._21 * mtx.f._23);
    pos->z = -(mtx.f._02 * mtx.f._03 + mtx.f._12 * mtx.f._13 + mtx.f._22 * mtx.f._23);
}

void Sound3DListener::ResetMatrix()
{
    nw::math::MTX34Zero(&m_Mtx);
    m_Velocity = nw::math::VEC3(0.0f,0.0f,0.0f);
    m_ResetMatrixFlag = true;
}

void Sound3DListener::SetVelocity(const nw::math::VEC3& velocity)
{
    m_Velocity = velocity;
}

void Sound3DListener::SetInteriorSize(f32 interiorSize)
{
    NW_ASSERT(interiorSize > 0.0f);
    m_InteriorSize = interiorSize;
}

void Sound3DListener::SetMaxVolumeDistance(f32 maxVolumeDistance)
{
    NW_ASSERT(maxVolumeDistance >= 0.0f);
    m_MaxVolumeDistance = maxVolumeDistance;
}

void Sound3DListener::SetUnitDistance(f32 unitDistance)
{
    NW_ASSERT(unitDistance > 0.0f);
    m_UnitDistance = unitDistance;
}

void Sound3DListener::SetUnitBiquadFilterValue(f32 value)
{
    NW_MINMAX_ASSERT(value, 0.0f, 1.0f);
    m_UnitBiquadFilterValue = value;
}

void Sound3DListener::SetMaxBiquadFilterValue(f32 value)
{
    NW_MINMAX_ASSERT(value, 0.0f, 1.0f);
    m_MaxBiquadFilterValue = value;
}

} // namespace snd
} // namespace nw

