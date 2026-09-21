// Filename: lyt_Util.cpp
//
// Project: NintendoWare4Ctr

#include <nw/lyt/lyt_Util.h>
#include <nw/lyt/lyt_Bounding.h>
#include <nw/lyt/lyt_Layout.h>
#include <nw/lyt/lyt_Group.h>
#include <nw/lyt/lyt_Animation.h>
#include <nw/lyt/lyt_TexResource.h>
#include <nn/gx.h>

#define ARRAY_LENGTH(a)   (sizeof(a) / sizeof((a)[0]))

namespace nw{
namespace lyt{
namespace{

struct TexSpec
{
    int lytFormat;
    GLenum format;
    GLenum type;
    int minSize;
    bool compressed;
    bool final;
};

bool Contains(const nw::ut::Rect& rect,const nw::math::VEC2& point)
{
    return rect.left <= point.x && point.x <= rect.right && rect.bottom <= point.y && point.y <= rect.top;
}

}

void BindAnimation(Group* pGroup,AnimTransform* pAnimTrans,bool bRecursive,bool bDisable)
{
    PaneLinkList& paneList = pGroup->GetPaneList();
    for (PaneLinkList::Iterator it = paneList.GetBeginIter(); it != paneList.GetEndIter(); ++it)
    {
        it->target->BindAnimation(pAnimTrans, bRecursive, bDisable);
    }
}

void UnbindAnimation(Group* pGroup,AnimTransform* pAnimTrans,bool bRecursive)
{
    PaneLinkList& paneList = pGroup->GetPaneList();
    for (PaneLinkList::Iterator it = paneList.GetBeginIter(); it != paneList.GetEndIter(); ++it)
    {
        it->target->UnbindAnimation(pAnimTrans, bRecursive);
    }
}

void SetAnimationEnable(Group* pGroup,AnimTransform* pAnimTrans,bool bEnable,bool bRecursive)
{
    PaneLinkList& paneList = pGroup->GetPaneList();
    for (PaneLinkList::Iterator it = paneList.GetBeginIter(); it != paneList.GetEndIter(); ++it)
    {
        it->target->SetAnimationEnable(pAnimTrans, bEnable, bRecursive);
    }
}

bool IsContain(Pane* pPane,const math::VEC2& pos)
{
    math::MTX34 invGlbMtx;
    math::MTX34Inverse(&invGlbMtx, &pPane->GetGlobalMtx());

    math::VEC3 pos3(pos.x, pos.y, 0.f);
    math::VEC3 lclPos;
    math::VEC3Transform(&lclPos, &invGlbMtx, &pos3);

    return Contains(pPane->GetPaneRect(), math::VEC2(lclPos.x, lclPos.y));
}

Pane* FindHitPane(Pane* pPane,const math::VEC2& pos)
{
    if (!pPane->IsVisible())
    {
        return 0;
    }

    for (PaneList::ReverseIterator it = pPane->GetChildList().GetBeginReverseIter(); it != pPane->GetChildList().GetEndReverseIter(); ++it)
    {
        if (Pane *const ret = FindHitPane(&(*it), pos))
    {
            return ret;
        }
    }

    if (nw::lyt::Bounding *const pBounding = nw::ut::DynamicCast<nw::lyt::Bounding*>(pPane))
    {

        if (IsContain(pBounding, pos))
    {
            return pBounding;
        }
    }

    return 0;
}

Pane* FindHitPane(Layout* pLayout,const math::VEC2& pos)
{
    return FindHitPane(pLayout->GetRootPane(), pos);
}

Pane* GetNextPane(Pane* pPane)
{
    if (!pPane->GetChildList().IsEmpty())
    {
        PaneList::Iterator paneIt = pPane->GetChildList().GetBeginIter();
        return &(*paneIt);
    }

    while (true)
    {
        if (pPane->GetParent() == 0)
        {
            return 0;
        }

        PaneList::Iterator nextIt = PaneList::GetIteratorFromPointer(pPane->m_Link.GetNext());
        PaneList::Iterator endIt = pPane->GetParent()->GetChildList().GetEndIter();
        if (nextIt != endIt)
        {
            break;
        }

        pPane = pPane->GetParent();
    }

    return PaneList::GetPointerFromNode(pPane->m_Link.GetNext());
}

const TextureInfo LoadTexture(const void* pImgRes, u32 size, int texLoadFlag)
{
    const TexResource texResource(const_cast<void*>(pImgRes), size);
    if (!texResource.IsValid())
    {
        return TextureInfo();
    }

#ifndef NW_TARGET_CTR_GL_FINAL
#  undef GL_UNSIGNED_BYTE_4_4_DMP
#  define GL_UNSIGNED_BYTE_4_4_DMP GL_UNSIGNED_BYTE
#  undef GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP
#  define GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP GL_ETC1_RGB8_NATIVE_DMP
#  undef GL_UNSIGNED_4BITS_DMP
#  define GL_UNSIGNED_4BITS_DMP GL_UNSIGNED_BYTE
#endif

    static const TexSpec texSpec[] ={
        { TEXFORMAT_L8, GL_LUMINANCE_NATIVE_DMP, GL_UNSIGNED_BYTE, 8, false, false }, 
        { TEXFORMAT_A8, GL_ALPHA_NATIVE_DMP, GL_UNSIGNED_BYTE, 8, false, false },
        { TEXFORMAT_LA4, GL_LUMINANCE_ALPHA_NATIVE_DMP, GL_UNSIGNED_BYTE_4_4_DMP, 8, false, true },
        { TEXFORMAT_LA8, GL_LUMINANCE_ALPHA_NATIVE_DMP, GL_UNSIGNED_BYTE, 8, false, false },
        { TEXFORMAT_HILO8, GL_HILO8_DMP_NATIVE_DMP, GL_UNSIGNED_BYTE, 8, false, false },
        { TEXFORMAT_RGB565, GL_RGB_NATIVE_DMP, GL_UNSIGNED_SHORT_5_6_5, 8, false, false },
        { TEXFORMAT_RGB8, GL_RGB_NATIVE_DMP, GL_UNSIGNED_BYTE, 8, false, false },
        { TEXFORMAT_RGB5A1, GL_RGBA_NATIVE_DMP, GL_UNSIGNED_SHORT_5_5_5_1, 8, false, false },
        { TEXFORMAT_RGBA4, GL_RGBA_NATIVE_DMP, GL_UNSIGNED_SHORT_4_4_4_4, 8, false, false },
        { TEXFORMAT_RGBA8, GL_RGBA_NATIVE_DMP, GL_UNSIGNED_BYTE, 8, false, false },
        { TEXFORMAT_ETC1, GL_ETC1_RGB8_NATIVE_DMP, 0 /* N/A */, 16, true, false },
        { TEXFORMAT_ETC1A4, GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP, 0 /* N/A */, 8, true, true },
        { TEXFORMAT_L4, GL_LUMINANCE_NATIVE_DMP, GL_UNSIGNED_4BITS_DMP, 8, false, true },
        { TEXFORMAT_A4, GL_ALPHA_NATIVE_DMP, GL_UNSIGNED_4BITS_DMP, 8, false, true },
    };

    TexFormat format = texResource.GetFormat();
    const void* pixels = texResource.GetImageAddress();
    u32 imageSize = texResource.GetImageSize();


    u16 width = texResource.GetWidth();
    u16 realWidth = texSpec[format].minSize;
    while (realWidth != 0 && realWidth < width)
    {
        realWidth <<= 1;
    }

    u16 height = texResource.GetHeight();
    u16 realHeight = texSpec[format].minSize;
    while (realHeight != 0 && realHeight < height)
    {
        realHeight <<= 1;
    }

    if (texLoadFlag == 0)
    {
        switch (texResource.GetImageArea())
        {
        case MEMAREA_FCRAM:
            texLoadFlag = NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP;
            break;

        case MEMAREA_VRAMA:
            texLoadFlag = NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP;
            break;

        case MEMAREA_VRAMB:
            texLoadFlag = NN_GX_MEM_VRAMB | GL_NO_COPY_FCRAM_DMP;
            break;

        default:
            NW_FATAL_ERROR("unexpected image area.");
            return TextureInfo();
        }
    }

    GLuint texName = TextureInfo::INVALID;
    uptr physicalAddress = 0;

    if (Layout::GetLayoutDrawEnable())
    {
        glGenTextures(1, &texName);

        glBindTexture(GL_TEXTURE_2D, texName);
        NW_GL_ASSERT();

        if (texSpec[format].compressed)
        {
            glCompressedTexImage2D(GL_TEXTURE_2D | texLoadFlag,0,texSpec[format].format,realWidth,realHeight,0,imageSize,pixels);
            NW_GL_ASSERT();
        }
        else{
            glTexImage2D(GL_TEXTURE_2D | texLoadFlag,0,texSpec[format].format,realWidth,realHeight,0,texSpec[format].format,texSpec[format].type,pixels);
            NW_GL_ASSERT();
        }

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0f);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, -1000);

        {
            GLint dataAddr = 0;
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_DATA_ADDR_DMP, &dataAddr);
            physicalAddress = nngxGetPhysicalAddr((uptr)(dataAddr));
        }
    }
    else{

        const u32 MEM_MASK = 0x00030000;
        GLenum transtype = texLoadFlag & 0xFFFF0000;
        switch (transtype)
        {
        case NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP:
            nngxUpdateBuffer(pixels, imageSize);
            physicalAddress = nngxGetPhysicalAddr((uptr)(pixels));
            break;

        case NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP:
        case NN_GX_MEM_VRAMB | GL_NO_COPY_FCRAM_DMP:{
            GLvoid* (*pGlAllocator)(GLenum, GLenum, GLuint, GLsizei) = NULL;
            nngxGetAllocator(&pGlAllocator, NULL);
            if (pGlAllocator == NULL)
            {
                NW_FATAL_ERROR("can not get DMPGL allocator.");
                return TextureInfo();
            }

            GLuint area = transtype & MEM_MASK;

            void* address = pGlAllocator(area, NN_GX_MEM_TEXTURE, texName, imageSize);

            if (address != NULL)
            {
                nngxAddVramDmaCommand(pixels, address, imageSize);

                physicalAddress = nngxGetPhysicalAddr((uptr)(address));

                NW_ASSERT(((u32)(address) & MEM_AREA_FIELD) == 0);
                texName = u32(address) | u32(area / NN_GX_MEM_FCRAM);
            }
        }
        break;

    default:
        NW_FATAL_ERROR("unexpected texture transfer type.");
    }

    return TextureInfo(texName,physicalAddress,TexSize(width, height),TexSize(realWidth, realHeight),format);
}

}
}
}