// Filename: anim_AnimFrameController.cpp
//
// Project: NintendoWare4Ctr

#include <nw/math.h>
#include <nw/anim/anim_AnimFrameController.h>

using namespace nw::math;

namespace nw {
namespace anim {

f32 PlayPolicy_Onetime(f32 startFrame, f32 endFrame, f32 inputFrame, void* /*pUserData*/)
{
    if (inputFrame > endFrame)
    {
        return endFrame;
    }

    if (inputFrame < startFrame)
    {
        return startFrame;
    }

    return inputFrame;
}

f32 PlayPolicy_Loop(f32 startFrame, f32 endFrame, f32 inputFrame, void* /*pUserData*/)
{
    NW_ASSERT(startFrame <= endFrame);
    f32 length = endFrame - startFrame;
    if (length == 0.0f)
    {
        return startFrame;
    }
    else
    {
        f32 offset = inputFrame - startFrame;
        if (offset < 0.0f)
        {
            offset += (FFloor(-offset / length) + 1) * length;
        }
        return FMod(offset, length) + startFrame;
    }
}

} // namespace anim
} // namespace nw