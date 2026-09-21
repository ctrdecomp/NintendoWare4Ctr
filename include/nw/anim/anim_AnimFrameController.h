#pragma once

#include <nw/types.h>

namespace nw   {
namespace anim {

class AnimFrame
{
public:
    AnimFrame() { ResetFrame(0.0f); }
    AnimFrame(f32 frame) { ResetFrame(frame); }
    AnimFrame(f32 frame, f32 lastFrame) { Set(frame, lastFrame); }
    AnimFrame(const AnimFrame& animFrame) { Set(animFrame); }
    virtual ~AnimFrame() {}

    f32 GetFrame() const { return m_Frame; }

    void SetFrame(f32 frame)
    {
        m_LastFrame = m_Frame;
        m_Frame = frame;
    }

    f32 GetLastFrame() const { return m_LastFrame; }

    void SetLastFrame(f32 lastFrame) { m_LastFrame = lastFrame; }

    void Set(const AnimFrame& animFrame)
    {
        m_Frame = animFrame.GetFrame();
        m_LastFrame = animFrame.GetLastFrame();
    }

    void Set(f32 frame, f32 lastFrame)
    {
        m_Frame = frame;
        m_LastFrame = lastFrame;
    }

    void ResetFrame(f32 frame)
    {
        m_Frame = m_LastFrame = frame;
    }

    f32 GetDelta() const { return m_Frame - m_LastFrame; }

    bool IsOrder() const { return m_Frame >= m_LastFrame; }

    bool operator==(const AnimFrame& rhs) const
    {
        return m_Frame == rhs.m_Frame && m_LastFrame == rhs.m_LastFrame;
    }
    bool operator!=(const AnimFrame& rhs) const
    {
        return m_Frame != rhs.m_Frame || m_LastFrame != rhs.m_LastFrame;
    }

private:
    f32 m_Frame;
    f32 m_LastFrame;
};

f32 PlayPolicy_Onetime(f32 startFrame, f32 endFrame, f32 inputFrame, void* pUserData);
f32 PlayPolicy_Loop(f32 startFrame, f32 endFrame, f32 inputFrame, void* pUserData);

class AnimFrameController
{
public:
    typedef f32 (*PlayPolicy)(f32 startFrame, f32 endFrame, f32 inputFrame, void* pUserData);

    AnimFrameController(f32 startFrame = 0.0f,f32 endFrame = 1.0f,PlayPolicy playPolicy = PlayPolicy_Onetime,void* userData = NULL): 
        m_AnimFrame(startFrame),
        m_StepFrame(1.0f),
        m_StartFrame(startFrame),
        m_EndFrame(endFrame),
        m_PlayPolicy(playPolicy),
        m_UserData(userData) {}


    virtual ~AnimFrameController() {}


    const AnimFrame& GetAnimFrame() const { return m_AnimFrame; }
    AnimFrame& GetAnimFrame() { return m_AnimFrame; }
    void SetAnimFrame(const AnimFrame& animFrame) { m_AnimFrame = animFrame; }

    f32 GetFrame() const { return this->m_AnimFrame.GetFrame(); }
    void SetFrame(f32 frame)
    {
        this->m_AnimFrame.SetFrame(m_PlayPolicy(this->m_StartFrame, this->m_EndFrame, frame, this->m_UserData));
    }

    f32 GetStepFrame() const { return m_StepFrame; }
    void SetStepFrame(f32 stepFrame) { m_StepFrame = stepFrame; }

    f32 GetStartFrame() const { return m_StartFrame; }
    void SetStartFrame(f32 startFrame) { m_StartFrame = startFrame; }

    f32 GetEndFrame() const { return m_EndFrame; }
    void SetEndFrame(f32 endFrame) { m_EndFrame = endFrame; }

    PlayPolicy GetPlayPolicy() const { return m_PlayPolicy; }
    void SetPlayPolicy(PlayPolicy playPolicy) { m_PlayPolicy = playPolicy; }

    const void* GetUserData() const { return m_UserData; }
    void* GetUserData() { return m_UserData; }
    void SetUserData(void* userData) { m_UserData = userData; }

    void UpdateFrame()
    {
        SetFrame(GetFrame() + this->m_StepFrame);
    }       


private:
    AnimFrame m_AnimFrame;
    f32 m_StepFrame;
    f32 m_StartFrame;
    f32 m_EndFrame;
    PlayPolicy m_PlayPolicy;
    void* m_UserData;
};

}
}