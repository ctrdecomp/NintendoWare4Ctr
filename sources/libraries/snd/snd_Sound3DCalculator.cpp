// Filename: snd_Sound3DCalculator.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Sound3DCalculator.h>

#include <nw/snd/snd_Sound3DManager.h>
#include <nw/snd/snd_Sound3DListener.h>

namespace nw {
namespace snd {

namespace 
{

inline f32 SolveLinerFunction(f32 x, f32 x1, f32 x2, f32 y1, f32 y2)
{
    if (x1 == x2) 
    {
        return (y1 + y2) / 2.0f;
    }
    f32 divider = x1 - x2;
    return x * (y1 - y2) / divider + (x1 * y2 - x2 * y1) / divider;
}

} // namespace

void Sound3DCalculator::CalcVolumeAndPriority(
    const Sound3DManager& manager, const Sound3DListener& listener,
    const Sound3DParam& actorParam, f32* volumePtr, int* priorityPtr)
{
    nw::math::VEC3 pos;
    nw::math::VEC3Sub(&pos, &actorParam.position, &listener.GetPosition());
    const f32 actorDistance = nw::math::VEC3Len(&pos);

    CalcVolumeAndPriorityImpl(actorDistance,
        static_cast<SoundArchive::Sound3DInfo::DecayCurve>(actorParam.decayCurve), actorParam.decayRatio,
        manager.GetMaxPriorityReduction(), listener.GetMaxVolumeDistance(), listener.GetUnitDistance(),
        volumePtr, priorityPtr);
}

void Sound3DCalculator::CalcPan(
    const Sound3DManager& manager, const Sound3DListener& listener,
    const Sound3DParam& actorParam, const CalcPanParam& calcPanParam,
    f32* panPtr, f32* spanPtr)
{

    const math::MTX34& listenerMtx = listener.GetMatrix();
    nw::math::VEC3 pos;
    nw::math::VEC3Transform(&pos, &listenerMtx, &actorParam.position);
    const f32 actorDistance = nw::math::VEC3Len(&pos);

    CalcPanImpl(pos, listener.GetInteriorSize(), actorDistance, manager.GetPanRange(),
        calcPanParam.stereoSpeakerAngle,
        calcPanParam.surroundSpeakerFrontAngle,
        calcPanParam.surroundSpeakerRearAngle,
        calcPanParam.surroundPanOffset,
        panPtr,
        spanPtr
    );
}

void Sound3DCalculator::CalcPitch(
    const Sound3DManager& manager, const Sound3DListener& listener,
    const Sound3DParam& actorParam, f32* pitchPtr)
{
    NW_NULL_ASSERT(pitchPtr);

    const f32 sonicVelocity = manager.GetSonicVelocity();
    if (sonicVelocity == 0.0f) 
    {
        *pitchPtr = 1.0f;
        return;
    }

    nw::math::VEC3 relPos;
    nw::math::VEC3Sub(&relPos, &actorParam.position, &listener.GetPosition());
    const f32 distance = nw::math::VEC3Len(&relPos);
    if (distance > 0.0f) 
    {
        relPos /= distance;
    }

    const f32 dopplerFactor = actorParam.dopplerFactor / 32.0f;

    f32 actorVelocity;
    f32 listenerVelocity;
    if (distance > 0.0f)
    {
        actorVelocity = - nw::math::VEC3Dot(&relPos, &actorParam.velocity);
        listenerVelocity = - nw::math::VEC3Dot(&relPos, &listener.GetVelocity());
    }
    else 
    {
        actorVelocity = - nw::math::VEC3Len(&actorParam.velocity);
        listenerVelocity = nw::math::VEC3Len(&listener.GetVelocity());
    }
    actorVelocity *= dopplerFactor;
    listenerVelocity *= dopplerFactor;

    f32 pitch;
    if (listenerVelocity > sonicVelocity) 
    {
        pitch = 0.0f;
    }
    else if (actorVelocity >= sonicVelocity) 
    {
        pitch = 65535.0f;
    }
    else{
        pitch = (sonicVelocity - listenerVelocity) / (sonicVelocity - actorVelocity);
    }

    *pitchPtr = pitch;
}

void Sound3DCalculator::CalcBiquadFilterValue(
    const Sound3DManager& manager, const Sound3DListener& listener,
    const Sound3DParam& actorParam, f32* biquadFilterValuePtr)
{
    NW_NULL_ASSERT(biquadFilterValuePtr);

    NW_UNUSED_VARIABLE(manager);

    nw::math::VEC3 pos;
    nw::math::VEC3Sub(&pos, &actorParam.position, &listener.GetPosition());
    const f32 actorDistance = nw::math::VEC3Len(&pos);

    f32 biquadFilterValue = 0.0f;
    f32 maxVolumeDistance = listener.GetMaxVolumeDistance();
    f32 maxBiquadFilterValue = listener.GetMaxBiquadFilterValue();

    if(actorDistance > maxVolumeDistance)
    {
        biquadFilterValue =
            (actorDistance - maxVolumeDistance) /
            listener.GetUnitDistance() *
            listener.GetUnitBiquadFilterValue();

        if (biquadFilterValue > maxBiquadFilterValue)
        {
            biquadFilterValue = maxBiquadFilterValue;
        }
    }

    *biquadFilterValuePtr = biquadFilterValue;
}

void Sound3DCalculator::CalcVolumeAndPriorityImpl(f32 actorDistance,
    SoundArchive::Sound3DInfo::DecayCurve decayCurve,
    f32 decayRatio, int maxPriorityReduction, f32 maxVolumeDistance,
    f32 unitDistance, f32* volumePtr, int* priorityPtr)
{
    NW_NULL_ASSERT(volumePtr);
    NW_NULL_ASSERT(priorityPtr);

    static const f32 MAX_VOLUME = 1.0f;
    f32 volume = MAX_VOLUME;

    if (actorDistance > maxVolumeDistance)
    {
        switch (decayCurve)
        {
        case DECAY_CURVE_LOG:
            volume = std::powf(decayRatio, (actorDistance - maxVolumeDistance) / unitDistance);
            break;
        case DECAY_CURVE_LINEAR:
            volume = 1.0f - (actorDistance - maxVolumeDistance) / unitDistance * (1.0f - decayRatio);
            if (volume < 0.0f) 
            {
                volume = 0.0f;
            }
            break;
        }
    }

    *volumePtr = volume;
    *priorityPtr = - static_cast<int>((1.0f - volume) * maxPriorityReduction);
}

void Sound3DCalculator::CalcPanImpl(
    const nw::math::VEC3& pos,
    f32 interiorSize, f32 actorDistance, f32 panRange,
    f32 stereoSpeakerAngle, f32 surroundSpeakerFrontAngle,
    f32 surroundSpeakerRearAngle, f32 surroundPanOffset,
    f32* panPtr, f32* spanPtr)
{
    NW_UNUSED_VARIABLE(stereoSpeakerAngle);
    CalcPanSurround(pos, interiorSize, actorDistance, panRange,
        surroundSpeakerFrontAngle, surroundSpeakerRearAngle, surroundPanOffset, 
        panPtr, spanPtr);
}

void Sound3DCalculator::CalcPanSurround(
    const nw::math::VEC3& pos, f32 interiorSize, f32 actorDistance,
    f32 panRange, f32 surroundSpeakerFrontAngle, f32 surroundSpeakerRearAngle, 
    f32 surroundPanOffset, f32* panPtr, f32* surroundPanPtr)
{
    NW_NULL_ASSERT(panPtr);
    NW_NULL_ASSERT(surroundPanPtr);

    f32 angle;
    f32 distance;
    CalcAngleAndDistance(pos, actorDistance, interiorSize, &angle, &distance);

    f32 x = 0.0f;
    f32 z = 0.0f;

    const f32 angleRearLeft = -surroundSpeakerRearAngle;
    const f32 angleFrontLeft = -surroundSpeakerFrontAngle;
    const f32 angleFrontRight = surroundSpeakerFrontAngle;
    const f32 angleRearRight = surroundSpeakerRearAngle;

    if (angle < angleRearLeft)
    {
        x = SolveLinerFunction(angle, -nw::math::PI, angleRearLeft, 0.0f, -1.0f);
        z = 1.0f;
    }
    else if (angle < -nw::math::PI / 2.0f)
    {
        x = -1.0f;
        z = SolveLinerFunction(angle, angleRearLeft, -nw::math::PI / 2.0f, 1.0f, 0.0f);
    }
    else if (angle < angleFrontLeft)
    {
        x = -1.0f;
        z = SolveLinerFunction(angle, -nw::math::PI / 2.0f, angleFrontLeft, 0.0f, -1.0f);
    }
    else if (angle < angleFrontRight)
    {
        x = SolveLinerFunction(angle, angleFrontLeft, angleFrontRight, -1.0f, 1.0f);
        z = -1.0f;
    }
    else if (angle < nw::math::PI / 2.0f)
    {
        x = 1.0f;
        z = SolveLinerFunction(angle, angleFrontRight, nw::math::PI / 2.0f, -1.0f, 0.0f);
    }
    else if (angle < angleRearRight)
    {
        x = 1.0f;
        z = SolveLinerFunction(angle, nw::math::PI / 2.0f, angleRearRight, 0.0f, 1.0f);
    }
    else
    {
        x = SolveLinerFunction(angle, angleRearRight, nw::math::PI, 1.0f, 0.0f);
        z = 1.0f;
    }

    f32 a = (std::cosf(surroundSpeakerFrontAngle) + std::cosf(surroundSpeakerRearAngle)) / 2.0f;
    f32 speakerOffset = a / (a + (-std::cosf(surroundSpeakerRearAngle)));

    const f32 center_x = 0.0f;
    const f32 center_z = speakerOffset;

    x *= panRange;
    z *= panRange;
    *panPtr = x * distance + center_x * (1.0f - distance);
    *surroundPanPtr = z * distance + center_z * (1.0f - distance) + 1.0f + surroundPanOffset;
}

void Sound3DCalculator::CalcAngleAndDistance(
    const nw::math::VEC3& pos, f32 actorDistance,
    f32 interiorSize, f32* anglePtr, f32* distancePtr)
{
    NW_NULL_ASSERT(anglePtr);
    NW_NULL_ASSERT(distancePtr);

    nw::math::VEC3 interiorPos;

    if (actorDistance == 0.0f)
    {
        interiorPos.x = interiorPos.y = interiorPos.z = 0.0f;
    }
    else
    {
        nw::math::VEC3 yPlanePoint(pos.x, 0, pos.z);
        f32 d = nw::math::VEC3Len(&yPlanePoint);
        if (d > interiorSize)
        {
            yPlanePoint.x *= interiorSize / d;
            yPlanePoint.z *= interiorSize / d;
        }

        f32 yPlaneDistance = nw::math::VEC3Len(&yPlanePoint);
        interiorPos.x = pos.x * yPlaneDistance / actorDistance;
        interiorPos.y = 0;
        interiorPos.z = pos.z * yPlaneDistance / actorDistance;
    }

    *anglePtr = std::atan2f(interiorPos.x, -interiorPos.z);
    *distancePtr = nw::math::VEC3Len(&interiorPos) / interiorSize;
}

} // namespace snd
} // namespace nw