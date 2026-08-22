#pragma once

#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

#include <nw/lyt/lyt_Pane.h>

namespace nw{
namespace lyt{

class DrawInfo;
class AnimationLink;


class Window : public Pane{
    typedef Pane Base;

    struct Content
    {
        ut::Color8 vtxColors[VERTEXCOLOR_MAX];
        internal::TexCoordAry texCoordAry;
    };

    struct Frame{
        Frame():   
            textureFlip(0),
            pMaterial(0)
        {}
        ~Frame();

        TextureFlip GetTextureFlip() const{
            return (TextureFlip) this->textureFlip;
        }

        u8 textureFlip;
        Material* pMaterial;
    };

public:
    NW_UT_RUNTIME_TYPEINFO;
    Window(u8 contentTexNum,u8 frameTexNum);
    Window(u8 contentTexNum,u8 frameLTTexNum,u8 frameRTTexNum,u8 frameRBTexNum,u8 frameLBTexNum);
    Window(u8 contentTexNum,u8 cornerLTTexNum,u8 cornerRTTexNum,u8 cornerRBTexNum,u8 cornerLBTexNum,u8 frameLTexNum,u8 frameTTexNum,u8 frameRTexNum,u8 frameBTexNum);
    Window(const res::Window* pBlock,const ResBlockSet& resBlockSet);
    virtual ~Window();

    virtual const ut::Color8 GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color8 value);

    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);

    using Base::GetMaterial;

    virtual u8 GetMaterialNum() const;
    virtual Material* GetMaterial(u32 idx) const;

    Material* GetContentMaterial() const;
    void SetContentMaterial(Material* pMaterial);

    Material* GetFrameMaterial(WindowFrame frameIdx) const;

    void SetFrameMaterial(WindowFrame frameIdx, Material* pMaterial);

    void ReserveTexCoord(u8 num);

    u8 GetTexCoordNum() const;
    void SetTexCoordNum(u8 num);

    void GetTexCoord(u32 idx, TexCoordQuad coords) const;
    void SetTexCoord(u32 idx, const TexCoordQuad coords);


    u8 GetFrameNum() const{return mFrameNum;}

    virtual Material* FindMaterialByName(const char* findName,bool bRecursive = true);

protected:
    virtual void DrawSelf(const DrawInfo& drawInfo);

protected:
    virtual void DrawContent(const DrawInfo& draInfo,const math::VEC2& basePt,const WindowFrameSize& frameSize,u8 alpha);
    virtual void DrawFrame(const DrawInfo& draInfo,const math::VEC2& basePt,const Frame& frame,const WindowFrameSize& frameSize,u8 alpha);
    virtual void DrawFrame4(const DrawInfo& draInfo,const math::VEC2& basePt,const Frame* frames,const WindowFrameSize& frameSize,u8 alpha);
    virtual void DrawFrame8(const DrawInfo& draInfo,const math::VEC2& basePt,const Frame* frames,const WindowFrameSize& frameSize,u8 alpha);
    
protected:
    const WindowFrameSize GetFrameSize(u8 frameNum,const Frame* frames) const;
    void InitTexNum(u8 contentTexNum,u8 frameTexNums[],u8 frameNum);
    void InitContent(u8 texNum);
    void InitFrame(u8 frameNum);

public:
    virtual void MakeUniformDataSelf( DrawInfo* pDrawInfo, Drawer* pDrawer ) const;

protected:
    mutable bool mIsTexCoordInited;
    mutable u32 mUniformTexCoordNum;
    mutable nw::math::VEC4 mUniformTexCoords[TexMapMax * 2]; 

protected:
    InflationLRTB mContentInflation;
    Content mContent;
    Frame* mFrames;
    u8 mFrameNum;
    Material* mpMaterial;

private:
    Window(const Window& other);
    Window& operator = (const Window& other);

};

}
}