#pragma once

#include <nw/math/math_Types.h>
#include <nw/font/font_WideTextWriter.h>
#include <nw/ut/ut_Rect.h>

namespace nw{
namespace lyt{

class GraphicsResource;
class Layout;

class DrawInfo{
public:
    DrawInfo();
    virtual ~DrawInfo();

    const math::MTX34&  GetViewMtx() const{return mViewMtx;}
    void SetViewMtx(const math::MTX34& value){mViewMtx = value;}
    bool IsInfluencedAlpha() const{return mFlag.influencedAlpha;}
    void SetInfluencedAlpha(bool bEnable){mFlag.influencedAlpha = bEnable;}
    bool IsLocationAdjust() const{return mFlag.locationAdjust;}
    void SetLocationAdjust(bool bEnable){mFlag.locationAdjust = bEnable;}
    const math::VEC2& GetLocationAdjustScale() const{ return mLocationAdjustScale;}
    void SetLocationAdjustScale(const math::VEC2& scale){mLocationAdjustScale = scale;}
    f32 GetGlobalAlpha() const{return mGlobalAlpha;}
    void SetGlobalAlpha(f32 alpha){mGlobalAlpha = alpha;}
    bool IsInvisiblePaneCalculateMtx() const{return mFlag.invisiblePaneCalculateMtx;}
    void SetInvisiblePaneCalculateMtx(bool bEnable){mFlag.invisiblePaneCalculateMtx = bEnable;}
    GraphicsResource* GetGraphicsResource() const{return mpGraphicsResource;}

    void SetProjectionMtx(const nw::math::MTX44& mtx);

    void SetLayout(Layout* pLayout) const{mpLayout = pLayout;}

    Layout* GetLayout() const{ return mpLayout; }
    const nw::math::MTX44& GetProjectionMtx() const{return mProjMtx;}
protected:
    math::MTX44 mProjMtx;
    math::MTX34 mViewMtx;
    math::VEC2 mLocationAdjustScale;
    f32 mGlobalAlpha;
    GraphicsResource* mpGraphicsResource;
    mutable Layout* mpLayout;

    struct Flag{
        u8 influencedAlpha : 1;
        u8 locationAdjust : 1;
        u8 invisiblePaneCalculateMtx : 1;
    };
    Flag mFlag;
};

}
}