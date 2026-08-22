#pragma once

#include <nw/ut/ut_Rect.h>
#include <nw/gfx/gfx_IRenderTarget.h>

namespace nw{
namespace gfx{

class Viewport{
public:

    Viewport(): 
        mDepthNear(0.f),
        mDepthFar(1.f) 
    {}

    Viewport(float x,float y,float width,float height,float near, float far ): 
        mRect( x, y, x + width, y + height ),
        mDepthNear(near),
        mDepthFar(far) 
    {}

    Viewport(nw::ut::Rect rect,float near,float far ): 
        mRect( rect ),
        mDepthNear(near),
        mDepthFar(far) 
    {}

    Viewport(IRenderTarget* renderTarget,float near,float far): 
        mDepthNear(near),
        mDepthFar(far) {
       NW_NULL_ASSERT(renderTarget);

       f32 width = static_cast<f32>(renderTarget->GetDescription().width);
       f32 height = static_cast<f32>(renderTarget->GetDescription().height);
       
       this->mRect.SetOriginAndSize(0.f, 0.f, width, height);
    }

    Viewport(const Viewport& v): 
        mRect(v.mRect),
        mDepthNear(v.mDepthNear),
        mDepthFar(v.mDepthFar) 
    {}

    ~Viewport() {}

    void SetDepthRange(f32 near, f32 far){
        this->mDepthNear = near;
        this->mDepthFar  = far;
    }

    void SetBound(const nw::ut::Rect& bound){
        this->mRect = bound;
    }

    void SetBound(float x, float y, float width, float height){
        this->mRect.SetOriginAndSize(x,y,width,height );
    }

    void SetBound(IRenderTarget* renderTarget){
        NW_NULL_ASSERT(renderTarget);

        this->mRect.SetOriginAndSize(0.f, 0.f,static_cast<f32>(renderTarget->GetDescription().width),static_cast<f32>(renderTarget->GetDescription().height));
    }

    const nw::ut::Rect& GetBound() const { return mRect; }

    f32 GetDepthNear() const { return mDepthNear; }

    f32 GetDepthFar() const { return mDepthFar; }

private:
    nw::ut::Rect mRect;
    f32 mDepthNear;
    f32 mDepthFar;
};

}
}