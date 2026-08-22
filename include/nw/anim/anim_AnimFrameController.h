#pragma once

#include <nw/types.h>

namespace nw   {
namespace anim {

class AnimFrame{
public:
    AnimFrame() { ResetFrame(0.0f); }
    AnimFrame(f32 frame) { ResetFrame(frame); }
    AnimFrame(f32 frame, f32 lastFrame) { Set(frame, lastFrame); }
    AnimFrame(const AnimFrame& animFrame) { Set(animFrame); }
    virtual ~AnimFrame() {}

    f32 GetFrame() const { return mFrame; }

    void SetFrame(f32 frame){
        mLastFrame = mFrame;
        mFrame = frame;
    }

    f32 GetLastFrame() const { return mLastFrame; }

    void SetLastFrame(f32 lastFrame) { mLastFrame = lastFrame; }

    void Set(const AnimFrame& animFrame){
        mFrame = animFrame.GetFrame();
        mLastFrame = animFrame.GetLastFrame();
    }

    void Set(f32 frame, f32 lastFrame){
        mFrame = frame;
        mLastFrame = lastFrame;
    }

    void ResetFrame(f32 frame){
        mFrame = mLastFrame = frame;
    }

    f32 GetDelta() const { return mFrame - mLastFrame; }

    bool IsOrder() const { return mFrame >= mLastFrame; }

    bool operator==(const AnimFrame& rhs) const{
        return mFrame == rhs.mFrame && mLastFrame == rhs.mLastFrame;
    }
    bool operator!=(const AnimFrame& rhs) const{
        return mFrame != rhs.mFrame || mLastFrame != rhs.mLastFrame;
    }

private:
    f32 mFrame;
    f32 mLastFrame;
};

f32 PlayPolicy_Onetime(f32 startFrame, f32 endFrame, f32 inputFrame, void* pUserData);
f32 PlayPolicy_Loop(f32 startFrame, f32 endFrame, f32 inputFrame, void* pUserData);

class AnimFrameController{
public:
    typedef f32 (*PlayPolicy)(f32 startFrame, f32 endFrame, f32 inputFrame, void* pUserData);

    AnimFrameController(f32 startFrame = 0.0f,f32 endFrame = 1.0f,PlayPolicy playPolicy = PlayPolicy_Onetime,void* userData = NULL): 
        mAnimFrame(startFrame),
        mStepFrame(1.0f),
        mStartFrame(startFrame),
        mEndFrame(endFrame),
        mPlayPolicy(playPolicy),
        mUserData(userData)
    {}


    virtual ~AnimFrameController() {}


    const AnimFrame& GetAnimFrame() const { return mAnimFrame; }
    AnimFrame& GetAnimFrame() { return mAnimFrame; }
    void SetAnimFrame(const AnimFrame& animFrame) { mAnimFrame = animFrame; }

    f32 GetFrame() const { return this->mAnimFrame.GetFrame(); }
    void SetFrame(f32 frame){
        this->mAnimFrame.SetFrame(mPlayPolicy(this->mStartFrame, this->mEndFrame, frame, this->mUserData));
    }

    f32 GetStepFrame() const { return mStepFrame; }
    void SetStepFrame(f32 stepFrame) { mStepFrame = stepFrame; }

    f32 GetStartFrame() const { return mStartFrame; }
    void SetStartFrame(f32 startFrame) { mStartFrame = startFrame; }

    f32 GetEndFrame() const { return mEndFrame; }
    void SetEndFrame(f32 endFrame) { mEndFrame = endFrame; }

    PlayPolicy GetPlayPolicy() const { return mPlayPolicy; }
    void SetPlayPolicy(PlayPolicy playPolicy) { mPlayPolicy = playPolicy; }

    const void* GetUserData() const { return mUserData; }
    void* GetUserData() { return mUserData; }
    void SetUserData(void* userData) { mUserData = userData; }

    void UpdateFrame(){
        SetFrame(GetFrame() + this->mStepFrame);
    }       


private:
    AnimFrame mAnimFrame;
    f32 mStepFrame;
    f32 mStartFrame;
    f32 mEndFrame;
    PlayPolicy mPlayPolicy;
    void* mUserData;
};

}
}