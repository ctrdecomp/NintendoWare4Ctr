#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

struct ResParticleShapeBuilderData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResBool m_IsAscendingOrder;
    u8              padding_0[3];
    nw::ut::ResVec3 m_DrawOffset;
};

class ResParticleShapeBuilder : public nw::ut::ResCommon< ResParticleShapeBuilderData >
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleShapeBuilder) 
    };

    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('BUIL') 
    };

    NW_RES_CTOR(ResParticleShapeBuilder)

    bool IsAscendingOrder() const { return static_cast<bool>(ref().m_IsAscendingOrder); }          
    void SetAscendingOrder(bool value) { ref().m_IsAscendingOrder = value; }

    void SetDrawOffset(f32 x, f32 y, f32 z) { ref().m_DrawOffset.Set(x, y, z); }            
    const nw::math::VEC3& GetDrawOffset() const { return ref().m_DrawOffset; }                       
    void SetDrawOffset(const nw::math::VEC3& value) { ref().m_DrawOffset = value; }

    void Setup();
    
    nw::ut::ResTypeInfo GetTypeInfo() const { return ref().typeInfo; }
};

struct ResParticleBillboardShapeBuilderData : public ResParticleShapeBuilderData
{
};

class ResParticleBillboardShapeBuilder : public ResParticleShapeBuilder
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleBillboardShapeBuilder) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('BILL') 
    };

    NW_RES_CTOR_INHERIT(ResParticleBillboardShapeBuilder, ResParticleShapeBuilder)

    void Setup();
};

struct ResParticleWorldBillboardShapeBuilderData : public ResParticleShapeBuilderData
{
};

class ResParticleWorldBillboardShapeBuilder : public ResParticleShapeBuilder
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleWorldBillboardShapeBuilder) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('WBLL') 
    };

    NW_RES_CTOR_INHERIT(ResParticleWorldBillboardShapeBuilder, ResParticleShapeBuilder)

    void Setup();
};

struct ResParticleYBillboardShapeBuilderData : public ResParticleShapeBuilderData
{
};

class ResParticleYBillboardShapeBuilder : public ResParticleShapeBuilder
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleYBillboardShapeBuilder) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('YBIL') 
    };

    NW_RES_CTOR_INHERIT(ResParticleYBillboardShapeBuilder, ResParticleShapeBuilder)

    void Setup();
};

struct ResParticleXyPlaneShapeBuilderData : public ResParticleShapeBuilderData
{
};

class ResParticleXyPlaneShapeBuilder : public ResParticleShapeBuilder
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleXyPlaneShapeBuilder) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('XYPL') 
    };

    NW_RES_CTOR_INHERIT(ResParticleXyPlaneShapeBuilder, ResParticleShapeBuilder)

    void Setup();
};

}
}
}

