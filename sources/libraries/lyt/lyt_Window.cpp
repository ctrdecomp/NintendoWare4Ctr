// Filename: lyt_Window.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Common.h>
#include <nw/lyt/lyt_DrawInfo.h>
#include <nw/lyt/lyt_GraphicsResource.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_Window.h>
#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_Animation.h>
#include <nw/lyt/lyt_ResourceAccessor.h>

namespace nw{
namespace lyt{
namespace local{
namespace{

struct Rect{
    f32 x, y, w, h;

    const math::VEC2& Position() const{
        return *(const math::VEC2 *)&this->x;
    }

    const nw::lyt::Size& Size() const{
        return *(const nw::lyt::Size *)&this->w;
    }
};

struct TextureFlipInfo{
    u8      coords[VERTEX_MAX][2];
    u8      idx[2];
};

/* Window Inlines */

const TextureFlipInfo& GetTexutreFlipInfo(u8 textureFlip){

    static TextureFlipInfo flipInfos[] ={
        { { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } }, { 0, 1 } },   // TEXTUREFLIP_NONE
        { { { 1, 0 }, { 0, 0 }, { 1, 1 }, { 0, 1 } }, { 0, 1 } },   // TEXTUREFLIP_FLIPH
        { { { 0, 1 }, { 1, 1 }, { 0, 0 }, { 1, 0 } }, { 0, 1 } },   // TEXTUREFLIP_FLIPV
        { { { 0, 1 }, { 0, 0 }, { 1, 1 }, { 1, 0 } }, { 1, 0 } },   // TEXTUREFLIP_ROTATE90
        { { { 1, 1 }, { 0, 1 }, { 1, 0 }, { 0, 0 } }, { 0, 1 } },   // TEXTUREFLIP_ROTATE180
        { { { 1, 0 }, { 1, 1 }, { 0, 0 }, { 0, 1 } }, { 1, 0 } },   // TEXTUREFLIP_ROTATE270
    };

    return flipInfos[textureFlip];
}

void GetLTFrameSize(math::VEC2* pPt,Size* pSize,const math::VEC2& basePt,const Size& winSize,const WindowFrameSize& frameSize){
    *pPt = basePt;

    pSize->width = winSize.width - frameSize.r;
    pSize->height = frameSize.t;
}

void GetLTTexCoord(math::VEC2 texCds[],const Size& polSize,const TexSize& texSize,u8 textureFlip){
    const TextureFlipInfo& flipInfo = GetTexutreFlipInfo(textureFlip);
    const int ix = flipInfo.idx[0];
    const int iy = flipInfo.idx[1];
    const math::VEC2 tSz(texSize.width, texSize.height);

    texCds[VERTEX_LT][ix] = texCds[VERTEX_LB][ix] = flipInfo.coords[VERTEX_LT][ix];
    texCds[VERTEX_LT][iy] = texCds[VERTEX_RT][iy] = flipInfo.coords[VERTEX_LT][iy];

    texCds[VERTEX_RB][ix] = texCds[VERTEX_RT][ix] = polSize.width  / ((flipInfo.coords[VERTEX_RT][ix] - flipInfo.coords[VERTEX_LT][ix]) * tSz[ix]) + flipInfo.coords[VERTEX_LT][ix];
    texCds[VERTEX_RB][iy] = texCds[VERTEX_LB][iy] = polSize.height / ((flipInfo.coords[VERTEX_LB][iy] - flipInfo.coords[VERTEX_LT][iy]) * tSz[iy]) + flipInfo.coords[VERTEX_LT][iy];
}

void GetRTFrameSize(math::VEC2* pPt,Size* pSize,const math::VEC2& basePt,const Size& winSize,const WindowFrameSize& frameSize){
    using namespace nw::math;

    *pPt = VEC2(basePt.x + winSize.width - frameSize.r, basePt.y);

    pSize->width  = frameSize.r;
    pSize->height = winSize.height - frameSize.b;
}

void GetRTTexCoord(math::VEC2 texCds[],const Size& polSize,const TexSize& texSize,u8 textureFlip){
    const TextureFlipInfo& flipInfo = GetTexutreFlipInfo(textureFlip);
    const int ix = flipInfo.idx[0];
    const int iy = flipInfo.idx[1];
    const math::VEC2 tSz(texSize.width, texSize.height);

    texCds[VERTEX_RT][ix] = texCds[VERTEX_RB][ix] = flipInfo.coords[VERTEX_RT][ix];
    texCds[VERTEX_RT][iy] = texCds[VERTEX_LT][iy] = flipInfo.coords[VERTEX_RT][iy];

    texCds[VERTEX_LB][ix] = texCds[VERTEX_LT][ix] = polSize.width  / ((flipInfo.coords[VERTEX_LT][ix] - flipInfo.coords[VERTEX_RT][ix]) * tSz[ix]) + flipInfo.coords[VERTEX_RT][ix];
    texCds[VERTEX_LB][iy] = texCds[VERTEX_RB][iy] = polSize.height / ((flipInfo.coords[VERTEX_RB][iy] - flipInfo.coords[VERTEX_RT][iy]) * tSz[iy]) + flipInfo.coords[VERTEX_RT][iy];
}

void GetLBFrameSize(math::VEC2* pPt,Size* pSize,const math::VEC2& basePt,const Size& winSize,const WindowFrameSize& frameSize){
    *pPt = math::VEC2(basePt.x, basePt.y - frameSize.t);

    pSize->width  = frameSize.l;
    pSize->height = winSize.height - frameSize.t;
}

void GetLBTexCoord(math::VEC2 texCds[],const Size& polSize,const TexSize& texSize,u8 textureFlip){
    const TextureFlipInfo& flipInfo = GetTexutreFlipInfo(textureFlip);
    const int ix = flipInfo.idx[0];
    const int iy = flipInfo.idx[1];
    const math::VEC2 tSz(texSize.width, texSize.height);

    texCds[VERTEX_LB][ix] = texCds[VERTEX_LT][ix] = flipInfo.coords[VERTEX_LB][ix];
    texCds[VERTEX_LB][iy] = texCds[VERTEX_RB][iy] = flipInfo.coords[VERTEX_LB][iy];

    texCds[VERTEX_RT][ix] = texCds[VERTEX_RB][ix] = polSize.width  / ((flipInfo.coords[VERTEX_RB][ix] - flipInfo.coords[VERTEX_LB][ix]) * tSz[ix]) + flipInfo.coords[VERTEX_LB][ix];
    texCds[VERTEX_RT][iy] = texCds[VERTEX_LT][iy] = polSize.height / ((flipInfo.coords[VERTEX_LT][iy] - flipInfo.coords[VERTEX_LB][iy]) * tSz[iy]) + flipInfo.coords[VERTEX_LB][iy];
}

void GetRBFrameSize(math::VEC2* pPt,Size* pSize,const math::VEC2& basePt,const Size& winSize,const WindowFrameSize& frameSize){
    *pPt = math::VEC2(basePt.x + frameSize.l, basePt.y - winSize.height + frameSize.b);

    pSize->width  = winSize.width - frameSize.l;
    pSize->height = frameSize.b;
}

void GetRBTexCoord(math::VEC2 texCds[],const Size& polSize,const TexSize& texSize,u8 textureFlip){
    const TextureFlipInfo& flipInfo = GetTexutreFlipInfo(textureFlip);
    const int ix = flipInfo.idx[0];
    const int iy = flipInfo.idx[1];
    const math::VEC2 tSz(texSize.width, texSize.height);

    texCds[VERTEX_RB][ix] = texCds[VERTEX_RT][ix] = flipInfo.coords[VERTEX_RB][ix];
    texCds[VERTEX_RB][iy] = texCds[VERTEX_LB][iy] = flipInfo.coords[VERTEX_RB][iy];

    texCds[VERTEX_LT][ix] = texCds[VERTEX_LB][ix] = polSize.width  / ((flipInfo.coords[VERTEX_LB][ix] - flipInfo.coords[VERTEX_RB][ix]) * tSz[ix]) + flipInfo.coords[VERTEX_RB][ix];
    texCds[VERTEX_LT][iy] = texCds[VERTEX_RT][iy] = polSize.height / ((flipInfo.coords[VERTEX_RT][iy] - flipInfo.coords[VERTEX_RB][iy]) * tSz[iy]) + flipInfo.coords[VERTEX_RB][iy];
}

void SetupFrameTransform(const GraphicsResource& gres,WindowFrame frame,TextureFlip flip){
    static const u32 frameFlag[WINDOWFRAME_MAX] ={
        internal::FRAMESPECFLAG_FRAME_LT,
        internal::FRAMESPECFLAG_FRAME_RT,
        internal::FRAMESPECFLAG_FRAME_LB,
        internal::FRAMESPECFLAG_FRAME_RB,
        internal::FRAMESPECFLAG_FRAME_LB,
        internal::FRAMESPECFLAG_FRAME_RT,
        internal::FRAMESPECFLAG_FRAME_LT,
        internal::FRAMESPECFLAG_FRAME_RB
    };

    static const u32 flipFlag[TEXTUREFLIP_MAX] ={
        internal::FRAMESPECFLAG_NORMAL,
        internal::FRAMESPECFLAG_FLIP_HFLIP,
        internal::FRAMESPECFLAG_FLIP_VFLIP,
        internal::FRAMESPECFLAG_FLIP_R90,
        internal::FRAMESPECFLAG_FLIP_R180,
        internal::FRAMESPECFLAG_FLIP_R270
    };

    GLint loc = gres.GetUniformLocation(gres.UNIFORM_uFrameSpec);
    u32 flag =frameFlag[frame] | flipFlag[flip];

    glUniform4f(loc, (f32)flag, 0, 0, 0);
}

}
}

/* Window::Frame */

Window::Frame::~Frame(){
    if (pMaterial && !pMaterial->IsUserAllocated()){
        Layout::DeleteObj(pMaterial);
    }
    pMaterial = 0;
}

/* Window */

Window::Window(u8 contentTexNum,u8 frameTexNum){
    const u8 frameNum = 1;
    u8 frameTexNums[frameNum];
    frameTexNums[WINDOWFRAME_LT] = frameTexNum;

    InitTexNum(contentTexNum, frameTexNums, frameNum);
}

Window::Window(u8 contentTexNum,u8 frameLTTexNum,u8 frameRTTexNum,u8 frameRBTexNum,u8 frameLBTexNum){
    const u8 frameNum = 4;
    u8 frameTexNums[frameNum];
    frameTexNums[WINDOWFRAME_LT] = frameLTTexNum;
    frameTexNums[WINDOWFRAME_RT] = frameRTTexNum;
    frameTexNums[WINDOWFRAME_RB] = frameRBTexNum;
    frameTexNums[WINDOWFRAME_LB] = frameLBTexNum;

    this->InitTexNum(contentTexNum, frameTexNums, frameNum);
}

Window::Window(u8 contentTexNum,u8 cornerLTTexNum,u8 cornerRTTexNum,u8 cornerRBTexNum,u8 cornerLBTexNum,u8 frameLTexNum,u8 frameTTexNum,u8 frameRTexNum,u8 frameBTexNum){
    const u8 frameNum = 8;
    u8 frameTexNums[frameNum];
    frameTexNums[WINDOWFRAME_LT] = cornerLTTexNum;
    frameTexNums[WINDOWFRAME_RT] = cornerRTTexNum;
    frameTexNums[WINDOWFRAME_RB] = cornerRBTexNum;
    frameTexNums[WINDOWFRAME_LB] = cornerLBTexNum;
    frameTexNums[WINDOWFRAME_L ] = frameLTexNum;
    frameTexNums[WINDOWFRAME_T ] = frameTTexNum;
    frameTexNums[WINDOWFRAME_R ] = frameRTexNum;
    frameTexNums[WINDOWFRAME_B ] = frameBTexNum;

    this->InitTexNum(contentTexNum, frameTexNums, frameNum);
}

Window::Window(const res::Window* pBlock,const ResBlockSet& resBlockSet):   
Base(pBlock){
    const res::WindowContent *const pResContent = internal::ConvertOffsToPtr<res::WindowContent>(pBlock, pBlock->contentOffset);
    const u8 texCoordNum = ut::Min(pResContent->texCoordNum, u8(TexMapMax));

    this->InitContent(texCoordNum);

    this->mContentInflation = pBlock->inflation;

    const u32 *const matOffsTbl = internal::ConvertOffsToPtr<u32>(resBlockSet.pMaterialList, sizeof(*resBlockSet.pMaterialList));

    for (int i = 0; i < VERTEXCOLOR_MAX; ++i){
        this->mContent.vtxColors[i] = pResContent->vtxCols[i];
    }

    if (texCoordNum > 0){
        if (!mContent.texCoordAry.IsEmpty()){
            this->mContent.texCoordAry.Copy(
                reinterpret_cast<const char*>(pResContent) + sizeof(*pResContent),
                texCoordNum);
        }
    }

    {
        const res::Material *const pResMaterial = internal::ConvertOffsToPtr<res::Material>(resBlockSet.pMaterialList, matOffsTbl[pResContent->materialIdx]);
        mpMaterial = Layout::NewObj<Material>(pResMaterial, resBlockSet);
    }

    mFrameNum = 0;
    mFrames = 0;
    if (pBlock->frameNum > 0){
        this->InitFrame(pBlock->frameNum);

        const u32 *const frameOffsetTable = internal::ConvertOffsToPtr<u32>(pBlock, pBlock->frameOffsetTableOffset);
        for (int i = 0; i < mFrameNum; ++i){
            const res::WindowFrame *const pResWindowFrame = internal::ConvertOffsToPtr<res::WindowFrame>(pBlock, frameOffsetTable[i]);
            mFrames[i].textureFlip = pResWindowFrame->textureFlip;

            const res::Material *const pResMaterial = internal::ConvertOffsToPtr<res::Material>(resBlockSet.pMaterialList, matOffsTbl[pResWindowFrame->materialIdx]);
            mFrames[i].pMaterial = Layout::NewObj<Material>(pResMaterial, resBlockSet);
        }
    }
}

void Window::InitTexNum(u8 contentTexNum,u8 frameTexNums[],u8 frameNum){
    this->InitContent(contentTexNum);

    mContentInflation.l = 0;
    mContentInflation.r = 0;
    mContentInflation.t = 0;
    mContentInflation.b = 0;

    this->mpMaterial = Layout::NewObj<Material>();
    if (mpMaterial){
        mpMaterial->ReserveMem(contentTexNum, contentTexNum, contentTexNum);
    }
    this->InitFrame(frameNum);

    for (int i = 0; i < mFrameNum; ++i){
        mFrames[i].pMaterial = Layout::NewObj<Material>();
        if (mFrames[i].pMaterial){
            mFrames[i].pMaterial->ReserveMem(frameTexNums[i], frameTexNums[i], frameTexNums[i]);
        }
    }
}

void Window::InitContent(u8 texNum){
    if (texNum > 0){
        ReserveTexCoord(texNum);
    }
    this->mIsTexCoordInited = false;
}

void Window::InitFrame(u8 frameNum){
    mFrameNum = 0;
    this->mFrames = Layout::NewArray<Frame>(frameNum);
    if (mFrames){
        mFrameNum = frameNum;
    }
}

Window::~Window(){
    Layout::DeleteArray(this->mFrames, this->mFrameNum);

    if (mpMaterial && ! mpMaterial->IsUserAllocated()){
        Layout::DeleteObj(this->mpMaterial);
        mpMaterial = 0;
    }

    mContent.texCoordAry.Free();
}

void Window::ReserveTexCoord(u8 num){
    mContent.texCoordAry.Reserve(num);
}

u8 Window::GetTexCoordNum() const{
    return mContent.texCoordAry.GetSize();
}

void Window::SetTexCoordNum(u8 num){
    mContent.texCoordAry.SetSize(num);
}

void Window::GetTexCoord(u32 idx,TexCoordQuad coords) const{
    mContent.texCoordAry.GetCoord(idx, coords);
}

void Window::SetTexCoord(u32 idx,const TexCoordQuad coords){
    this->mContent.texCoordAry.SetCoord(idx, coords);

    Material* pMaterial = this->GetContentMaterial();
    if (pMaterial != NULL)
    {
        pMaterial->SetTextureDirty();
    }
}

Material* Window::FindMaterialByName(const char* findName,bool bRecursive){
    if (mpMaterial){
        if (internal::EqualsMaterialName(this->mpMaterial->GetName(), findName)){
            return mpMaterial;
        }
    }
    for (int i = 0; i < mFrameNum; ++i){
        if (internal::EqualsMaterialName(this->mFrames[i].pMaterial->GetName(), findName)){
            return mFrames[i].pMaterial;
        }
    }

    if (bRecursive){
        for (PaneList::Iterator it = GetChildList().GetBeginIter(); it != GetChildList().GetEndIter(); ++it){
            if (Material* pMat = it->FindMaterialByName(findName, bRecursive)){
                return pMat;
            }
        }
    }

    return 0;
}

const ut::Color8 Window::GetVtxColor(u32 idx) const{
    return mContent.vtxColors[idx];
}

void Window::SetVtxColor(u32 idx,ut::Color8 value){
    mContent.vtxColors[idx] = value;
}

u8 Window::GetVtxColorElement(u32 idx) const{
    return internal::GetVtxColorElement(mContent.vtxColors, idx);
}

void Window::SetVtxColorElement(u32 idx, u8 value){
    internal::SetVtxColorElement(mContent.vtxColors, idx, value);
}

void Window::DrawSelf(const DrawInfo& drawInfo){
    LoadMtx(drawInfo);

    const WindowFrameSize frameSize = GetFrameSize(this->mFrameNum, this->mFrames);
    const math::VEC2 basePt = GetVtxPos();

    DrawContent(drawInfo, basePt, frameSize, GetGlobalAlpha());

    switch (this->mFrameNum){
    case 1:
        DrawFrame(drawInfo, basePt, this->mFrames[WINDOWFRAME_LT], frameSize, GetGlobalAlpha());
        break;
    case 4:
        DrawFrame4(drawInfo, basePt, this->mFrames, frameSize, GetGlobalAlpha());
        break;
    case 8:
        DrawFrame8(drawInfo, basePt, this->mFrames, frameSize, GetGlobalAlpha());
        break;
    }
}

void Window::DrawContent(const DrawInfo& drawInfo,const math::VEC2& basePt,const WindowFrameSize& frameSize,u8 alpha){
    mpMaterial->SetupGraphics(drawInfo, alpha);

    internal::DrawQuad(drawInfo,math::VEC2(basePt.x + frameSize.l - this->mContentInflation.l, basePt.y - frameSize.t + this->mContentInflation.t),
        Size(GetSize().width - frameSize.l + this->mContentInflation.l - frameSize.r + this->mContentInflation.r, GetSize().height - frameSize.t + this->mContentInflation.t - frameSize.b + this->mContentInflation.b),
        mContent.texCoordAry.GetSize(),
        mContent.texCoordAry.GetArray(),
        mContent.vtxColors);
}

void Window::DrawFrame(const DrawInfo& drawInfo,const math::VEC2& basePt,const Frame& frame,const WindowFrameSize& frameSize,u8 alpha){
    if (frame.pMaterial->GetTexMapNum() == 0){
        return;
    }

    frame.pMaterial->SetupGraphics(drawInfo, alpha, false);

    math::VEC2 polPt;
    Size polSize;

    GraphicsResource& gres = *drawInfo.GetGraphicsResource();

    local::GetLTFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
    local::SetupFrameTransform(gres, WINDOWFRAME_LT, TEXTUREFLIP_NONE);
    internal::DrawQuad(drawInfo, polPt, polSize);

    local::GetRTFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
    local::SetupFrameTransform(gres, WINDOWFRAME_RT, TEXTUREFLIP_FLIPH);
    internal::DrawQuad(drawInfo, polPt, polSize);

    local::GetRBFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
    local::SetupFrameTransform(gres, WINDOWFRAME_RB, TEXTUREFLIP_ROTATE180);
    internal::DrawQuad(drawInfo, polPt, polSize);

    local::GetLBFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
    local::SetupFrameTransform(gres, WINDOWFRAME_LB, TEXTUREFLIP_FLIPV);
    internal::DrawQuad(drawInfo, polPt, polSize);
}

void Window::DrawFrame4(const DrawInfo& drawInfo,const math::VEC2& basePt,const Frame* frames,const WindowFrameSize& frameSize,u8 alpha){
    math::VEC2 polPt[WINDOWFRAME_RB + 1];
    Size polSize[WINDOWFRAME_RB + 1];
    bool bRepeat = false;
    GraphicsResource& gres = *drawInfo.GetGraphicsResource();

    local::GetLTFrameSize(&polPt[WINDOWFRAME_LT], &polSize[WINDOWFRAME_LT], basePt, GetSize(), frameSize);
    local::GetRTFrameSize(&polPt[WINDOWFRAME_RT], &polSize[WINDOWFRAME_RT], basePt, GetSize(), frameSize);
    local::GetLBFrameSize(&polPt[WINDOWFRAME_LB], &polSize[WINDOWFRAME_LB], basePt, GetSize(), frameSize);
    local::GetRBFrameSize(&polPt[WINDOWFRAME_RB], &polSize[WINDOWFRAME_RB], basePt, GetSize(), frameSize);

    for (int i = WINDOWFRAME_LT; i <= WINDOWFRAME_RB; ++i){
        const Frame& frame = frames[i];

        if (frame.pMaterial && frame.pMaterial->GetTexMapNum() > 0){
            frame.pMaterial->SetupGraphics(drawInfo, alpha, false);
            local::SetupFrameTransform(gres, WindowFrame(i), frame.GetTextureFlip());
            if (!bRepeat){
                internal::DrawQuad(drawInfo, polPt[i], polSize[i]);
                bRepeat = true;
            }
            else{
                internal::DrawQuad_Repeat(drawInfo, polPt[i], polSize[i]);
            }
        }
    }
}

void Window::DrawFrame8(const DrawInfo& drawInfo,const math::VEC2& basePt,const Frame* frames,const WindowFrameSize& frameSize,u8 alpha){
    const f32 x0 = basePt.x;
    const f32 x1 = basePt.x + frameSize.l;
    const f32 x2 = basePt.x + this->GetSize().width - frameSize.r;

    const f32 y0 = basePt.y;
    const f32 y1 = basePt.y - frameSize.t;
    const f32 y2 = basePt.y - this->GetSize().height + frameSize.b;

    const f32 w0 = frameSize.l;
    const f32 w1 = this->GetSize().width - frameSize.l - frameSize.r;
    const f32 w2 = frameSize.r;

    const f32 h0 = frameSize.t;
    const f32 h1 = this->GetSize().height - frameSize.t - frameSize.b;
    const f32 h2 = frameSize.b;

    const local::Rect frameRect[WINDOWFRAME_MAX] ={
        { x0, y0, w0, h0 },
        { x2, y0, w2, h0 },
        { x0, y2, w0, h2 },
        { x2, y2, w2, h2 },
        { x0, y1, w0, h1 },
        { x2, y1, w2, h1 },
        { x1, y0, w1, h0 },
        { x1, y2, w1, h2 },
    };
    
    GraphicsResource& gres = *drawInfo.GetGraphicsResource();
    bool bRepeat = false;

    for (int i = 0; i < WINDOWFRAME_MAX; ++i){
        const Frame& frame = frames[i];

        if (frame.pMaterial->GetTexMapNum() > 0){
            frame.pMaterial->SetupGraphics(drawInfo, alpha, false);
            local::SetupFrameTransform(gres, WindowFrame(i), frame.GetTextureFlip());
            if (!bRepeat){
                internal::DrawQuad(drawInfo, frameRect[i].Position(), frameRect[i].Size());
                bRepeat = true;
            }
            else{
                internal::DrawQuad_Repeat(drawInfo, frameRect[i].Position(), frameRect[i].Size());
            }
        }
    }
}

const WindowFrameSize Window::GetFrameSize(u8 frameNum,const Window::Frame* frames) const{
    WindowFrameSize ret = { 0, 0, 0, 0 };

    switch (frameNum){
    case 1:{
            Material* pMaterial = frames[WINDOWFRAME_LT].pMaterial;

            if (pMaterial != NULL && pMaterial->GetTexMapNum() > 0){
                TexSize texSize = pMaterial->GetTexMap(0).GetSize();
                ret.l = texSize.width;
                ret.t = texSize.height;
                ret.r = texSize.width;
                ret.b = texSize.height;
            }
        }
        break;
    case 4:
    case 8:{
            Material* pMaterialLT = frames[WINDOWFRAME_LT].pMaterial;
            if (pMaterialLT != NULL && pMaterialLT->GetTexMapNum() > 0){
                ret.t = pMaterialLT->GetTexMap(0).GetHeight();
            }

            Material *pMaterialRT = frames[WINDOWFRAME_RT].pMaterial;
            if (pMaterialRT != NULL && pMaterialRT->GetTexMapNum() > 0){
                ret.r = pMaterialRT->GetTexMap(0).GetWidth();
            }

            Material *pMaterialRB = frames[WINDOWFRAME_RB].pMaterial;
            if (pMaterialRB != NULL && pMaterialRB->GetTexMapNum() > 0){
                ret.b = pMaterialRB->GetTexMap(0).GetHeight();
            }

            Material *pMaterialLB = frames[WINDOWFRAME_LB].pMaterial;
            if (pMaterialLB != NULL && pMaterialLB->GetTexMapNum() > 0){
                ret.l = pMaterialLB->GetTexMap(0).GetWidth();
            }
        }
        break;
    }

    return ret;
}

u8 Window::GetMaterialNum() const{
    return u8(1 + this->mFrameNum);
}

Material* Window::GetMaterial(u32 idx) const{
    return idx == 0 ? GetContentMaterial(): GetFrameMaterial(WindowFrame(idx - 1));
}

Material* Window::GetFrameMaterial(WindowFrame frameIdx) const{
    if (frameIdx >= this->mFrameNum){
        return NULL;
    }

    return mFrames[frameIdx].pMaterial;
}

void Window::SetFrameMaterial(WindowFrame frameIdx, Material* pMaterial){
    if (mFrames[frameIdx].pMaterial == pMaterial){
        return;
    }

    if (mFrames[frameIdx].pMaterial != NULL &&
        !mFrames[frameIdx].pMaterial->IsUserAllocated())
    {
        Layout::DeleteObj(this->mFrames[frameIdx].pMaterial);
    }

    mFrames[frameIdx].pMaterial = pMaterial;
    if (pMaterial != NULL){
        pMaterial->SetTextureDirty();
    }
}

Material* Window::GetContentMaterial() const{
    return mpMaterial;
}

void Window::SetContentMaterial(Material* pMaterial){
    if (mpMaterial == pMaterial){
        return;
    }

    if (mpMaterial != NULL && !mpMaterial->IsUserAllocated()){
        Layout::DeleteObj(this->mpMaterial);
    }

    mpMaterial = pMaterial;
    if (mpMaterial != NULL){
        mpMaterial->SetTextureDirty();
    }
}

void Window::MakeUniformDataSelf( DrawInfo* pDrawInfo, Drawer* pDrawer ) const{
    NW_UNUSED_VARIABLE(pDrawInfo);
    const WindowFrameSize frameSize( GetFrameSize(this->mFrameNum, this->mFrames) );
    const math::VEC2 basePt( GetVtxPos() );

    {

        {
            pDrawer->SetUpTexEnv(this->mpMaterial);
        }

        {
            pDrawer->SetUpTextures(this->mpMaterial);
        }

        {
            if (!mIsTexCoordInited || mpMaterial->IsTextureDirty()){
                mUniformTexCoordNum = pDrawer->CalcTextureCoords(this->mpMaterial,this->mContent.texCoordAry.GetArray(),this->mUniformTexCoords);
                mIsTexCoordInited = true;
                mpMaterial->SetTextureDirty( false );
            }
            pDrawer->SetUpTextureCoords(this->mUniformTexCoords, this->mUniformTexCoordNum);
        }

        {
            pDrawer->SetUpMtx(GetGlobalMtx());

            Size size(GetSize().width  - frameSize.l + this->mContentInflation.l - frameSize.r + this->mContentInflation.r,
                       GetSize().height - frameSize.t + this->mContentInflation.t - frameSize.b + this->mContentInflation.b );

            nw::math::VEC2 pos(basePt.x + frameSize.l - this->mContentInflation.l, basePt.y - frameSize.t + this->mContentInflation.t );
            
            pDrawer->SetUpQuad(size, pos);
        }

        {
            pDrawer->SetUpVtxColors(this->mContent.vtxColors, GetGlobalAlpha());
        }

        {
            pDrawer->SetUniformDataEnd();
        }
    }


    {
        ut::Color8 white[ 4 ] = { 0xffffffff,0xffffffff,0xffffffff,0xffffffff };
        pDrawer->SetUpVtxColors(white, GetGlobalAlpha());
    }
    
    const u8 texCoordNum = 1;
    int uniformTexCoordNum;
    nw::math::VEC4 uniformTexCoords[ TexMapMax * 2 ];
    Size polSize;
    math::VEC2 polPt;
    math::VEC2 texCds[ texCoordNum ][ VERTEX_MAX ];

    switch (this->mFrameNum){
      case 1 :{
            const Frame& frame = mFrames[WINDOWFRAME_LT];

            if (frame.pMaterial->GetTexMapNum() == 0) 
                return;

            pDrawer->SetUpTexEnv(frame.pMaterial);
            pDrawer->SetUpTextures(frame.pMaterial); 
            const TexSize texSize = frame.pMaterial->GetTexMap(0).GetSize();

            local::GetLTFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
            local::GetLTTexCoord(texCds[0], polSize, texSize, TEXTUREFLIP_NONE);

            uniformTexCoordNum = pDrawer->CalcTextureCoords( frame.pMaterial, texCds, uniformTexCoords );
            pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum );
            pDrawer->SetUpQuad(polSize, polPt);
            pDrawer->SetUniformDataEnd();
            
            local::GetRTFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
            local::GetRTTexCoord(texCds[0], polSize, texSize, TEXTUREFLIP_FLIPH);          

            uniformTexCoordNum = pDrawer->CalcTextureCoords(frame.pMaterial, texCds, uniformTexCoords);
            pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
            pDrawer->SetUpQuad(polSize, polPt);
            pDrawer->SetUniformDataEnd();
            
            local::GetRBFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
            local::GetRBTexCoord(texCds[0], polSize, texSize, TEXTUREFLIP_ROTATE180);

            uniformTexCoordNum = pDrawer->CalcTextureCoords(frame.pMaterial, texCds, uniformTexCoords);
            pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
            pDrawer->SetUpQuad(polSize, polPt);          
            pDrawer->SetUniformDataEnd();
            
            local::GetLBFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
            local::GetLBTexCoord(texCds[0], polSize, texSize, TEXTUREFLIP_FLIPV);

            uniformTexCoordNum = pDrawer->CalcTextureCoords(frame.pMaterial, texCds, uniformTexCoords);
            pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
            pDrawer->SetUpQuad(polSize, polPt);          
            pDrawer->SetUniformDataEnd();
        }
        break;
      case 4 :{
            const Frame* pFrame = &this->mFrames[WINDOWFRAME_LT];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv(pFrame->pMaterial);
                pDrawer->SetUpTextures(pFrame->pMaterial);
                
                local::GetLTFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
                local::GetLTTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);               
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();              
            }

            pFrame = &this->mFrames[WINDOWFRAME_RT];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv(pFrame->pMaterial);
                pDrawer->SetUpTextures(pFrame->pMaterial);
                
                local::GetRTFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
                local::GetRTTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);               
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();
            }

            pFrame = &this->mFrames[WINDOWFRAME_RB];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv(pFrame->pMaterial);
                pDrawer->SetUpTextures(pFrame->pMaterial);
                
                local::GetRBFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
                local::GetRBTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();
            }
            
            pFrame = &this->mFrames[WINDOWFRAME_LB];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv( pFrame->pMaterial );
                pDrawer->SetUpTextures( pFrame->pMaterial );
                
                local::GetLBFrameSize(&polPt, &polSize, basePt, GetSize(), frameSize);
                local::GetLBTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);
                
                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();              
            }
        }
        break;
      case 8 :{           
            const Frame* pFrame = &this->mFrames[WINDOWFRAME_LT];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv( pFrame->pMaterial );
                pDrawer->SetUpTextures( pFrame->pMaterial );

                polSize = Size(frameSize.l, frameSize.t);
                polPt    = basePt;

                local::GetLTTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);               
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();              
            }

            pFrame = &this->mFrames[WINDOWFRAME_T];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv(pFrame->pMaterial);
                pDrawer->SetUpTextures(pFrame->pMaterial);

                polSize = Size(GetSize().width - frameSize.l - frameSize.r, frameSize.t);
                polPt   = math::VEC2(basePt.x + frameSize.l, basePt.y);

                local::GetLTTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);               
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();              
            }

            pFrame = &this->mFrames[WINDOWFRAME_RT];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv(pFrame->pMaterial);
                pDrawer->SetUpTextures(pFrame->pMaterial);

                polSize = Size(frameSize.r, frameSize.t);
                polPt   = math::VEC2(basePt.x + GetSize().width - frameSize.r, basePt.y);
                
                local::GetRTTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);               
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();
                
            }
            
            pFrame = &this->mFrames[WINDOWFRAME_R];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv( pFrame->pMaterial );

                pDrawer->SetUpTextures( pFrame->pMaterial );
                
                polSize = Size(frameSize.r, GetSize().height - frameSize.t - frameSize.b);
                polPt   = math::VEC2(basePt.x + GetSize().width - frameSize.r, basePt.y - frameSize.t);
                
                local::GetRTTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords( pFrame->pMaterial, texCds,  uniformTexCoords );
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();              
            }

            pFrame = &this->mFrames[WINDOWFRAME_RB];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv(pFrame->pMaterial);
                pDrawer->SetUpTextures(pFrame->pMaterial);
                
                polSize =  Size(frameSize.r, frameSize.b);
                polPt   = math::VEC2(basePt.x + GetSize().width - frameSize.r, basePt.y - GetSize().height + frameSize.b);
                
                local::GetRBTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();              
            }

            pFrame = &this->mFrames[WINDOWFRAME_B];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv(pFrame->pMaterial);
                pDrawer->SetUpTextures(pFrame->pMaterial);
                
                polSize =  Size(GetSize().width - frameSize.l - frameSize.r, frameSize.b);
                polPt   =  math::VEC2(basePt.x + frameSize.l, basePt.y - GetSize().height + frameSize.b);
                
                local::GetRBTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();              
            }

            pFrame = &this->mFrames[WINDOWFRAME_LB];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv(pFrame->pMaterial);
                pDrawer->SetUpTextures(pFrame->pMaterial);
                
                polSize =  Size(frameSize.l, frameSize.b);
                polPt   =  math::VEC2(basePt.x, basePt.y - GetSize().height + frameSize.b);
                
                local::GetLBTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords( pFrame->pMaterial, texCds,  uniformTexCoords );
                pDrawer->SetUpTextureCoords(uniformTexCoords, uniformTexCoordNum);
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();              
            }

            pFrame = &this->mFrames[WINDOWFRAME_L];
            if (pFrame->pMaterial->GetTexMapNum() != 0){
                pDrawer->SetUpTexEnv(pFrame->pMaterial);
                pDrawer->SetUpTextures(pFrame->pMaterial);
                
                polSize =  Size(frameSize.l, GetSize().height - frameSize.t - frameSize.b);
                polPt   =  math::VEC2(basePt.x, basePt.y - frameSize.t);
                
                local::GetLBTexCoord(texCds[0], polSize, pFrame->pMaterial->GetTexMap(0).GetSize(), pFrame->textureFlip);

                uniformTexCoordNum = pDrawer->CalcTextureCoords(pFrame->pMaterial, texCds,  uniformTexCoords);
                pDrawer->SetUpTextureCoords( uniformTexCoords, uniformTexCoordNum);
                pDrawer->SetUpQuad(polSize, polPt);
                pDrawer->SetUniformDataEnd();              
            }
        }
        break;
    }
    pDrawer->FlushBuffer();
}

}
}