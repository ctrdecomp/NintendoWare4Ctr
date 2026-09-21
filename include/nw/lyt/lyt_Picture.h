#pragma once

#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/lyt/lyt_Material.h>
#include <nw/lyt/lyt_Pane.h>

namespace nw{
namespace lyt{

class DrawInfo;

class Picture : public Pane
{
    typedef Pane Base;
public:
    NW_UT_RUNTIME_TYPEINFO;
    explicit Picture(u8 texNum);
    explicit Picture(const TexMap& texMap);
    Picture(const res::Picture* pBlock,const ResBlockSet& resBlockSet);
    virtual ~Picture();
    virtual void Append(const TexMap& texMap);
    virtual const ut::Color8 GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color8 value);
    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);

    void ReserveTexCoord(u8 num);
    u8 GetTexCoordNum() const;
    void SetTexCoordNum(u8 num);
    void GetTexCoord(u32 idx, TexCoordQuad coords) const;
    void SetTexCoord(u32 idx, const TexCoordQuad coords);

    using Base::GetMaterial;

    virtual u8 GetMaterialNum() const;
    virtual Material* GetMaterial(u32 idx) const;
    void SetMaterial(Material* pMaterial);

public: 
    virtual void MakeUniformDataSelf( DrawInfo* pDrawInfo, Drawer* pDrawer ) const;
protected:
    mutable bool m_IsTexCoordInited;
    mutable u32 m_UniformTexCoordNum;
    mutable math::VEC4 m_UniformTexCoords[TexMapMax * 2];
    
protected:
    virtual void DrawSelf(const DrawInfo& drawInfo);
    void Init(u8 texNum);

protected:
    Material* m_pMaterial;
    ut::Color8 m_VtxColors[VERTEXCOLOR_MAX];
    internal::TexCoordAry m_TexCoordAry;
private:
    Picture(const Picture& other);
    Picture& operator = (const Picture& other);
};

}
}