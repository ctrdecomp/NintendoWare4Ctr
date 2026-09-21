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
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleShapeBuilder) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('BUIL') };

    NW_RES_CTOR( ResParticleShapeBuilder )

    NW_RES_FIELD_BOOL_PRIMITIVE_DECL(AscendingOrder)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, DrawOffset)

    void    Setup();
    
    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

struct ResParticleBillboardShapeBuilderData : public ResParticleShapeBuilderData
{
};

class ResParticleBillboardShapeBuilder : public ResParticleShapeBuilder
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleBillboardShapeBuilder) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('BILL') };

    NW_RES_CTOR_INHERIT( ResParticleBillboardShapeBuilder, ResParticleShapeBuilder )

    void    Setup();
};

struct ResParticleWorldBillboardShapeBuilderData : public ResParticleShapeBuilderData
{
};

class ResParticleWorldBillboardShapeBuilder : public ResParticleShapeBuilder
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleWorldBillboardShapeBuilder) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('WBLL') };

    NW_RES_CTOR_INHERIT(ResParticleWorldBillboardShapeBuilder, ResParticleShapeBuilder)

    void    Setup();
};

struct ResParticleYBillboardShapeBuilderData : public ResParticleShapeBuilderData
{
};

class ResParticleYBillboardShapeBuilder : public ResParticleShapeBuilder
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleYBillboardShapeBuilder) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('YBIL') };

    NW_RES_CTOR_INHERIT(ResParticleYBillboardShapeBuilder, ResParticleShapeBuilder)

    void    Setup();
};

struct ResParticleXyPlaneShapeBuilderData : public ResParticleShapeBuilderData
{
};

class ResParticleXyPlaneShapeBuilder : public ResParticleShapeBuilder
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleXyPlaneShapeBuilder) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('XYPL') };

    NW_RES_CTOR_INHERIT(ResParticleXyPlaneShapeBuilder, ResParticleShapeBuilder)

    void    Setup();
};

}
}
}