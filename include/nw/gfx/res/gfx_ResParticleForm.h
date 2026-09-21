#pragma once

#include "nw/ut/ut_ResUtil.h"
#include "nw/ut/ut_ResDictionary.h"
#include "nw/ut/ut_ResDeclMacros.h"
#include "nw/gfx/res/gfx_ResTypeInfo.h"
#include "nw/math/math_Types.h"

namespace nw {
namespace gfx {
namespace res {

struct ResParticleFormData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResBool m_IsResourceCopyEnabled;
    u8              padding_0[3];
};

class ResParticleForm : public nw::ut::ResCommon< ResParticleFormData >
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleForm) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('INIT') };

    NW_RES_CTOR( ResParticleForm )

    NW_RES_FIELD_PRIMITIVE_DECL( bool, IsResourceCopyEnabled )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
    void    Setup();
};

struct ResParticleCubeFormData : public ResParticleFormData
{
    nw::ut::ResF32 m_Inner;
    nw::ut::ResVec3 m_Scale;
    nw::ut::ResBool m_EvenInterval;
    u8              padding_0[3];
};

class ResParticleCubeForm : public ResParticleForm
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleCubeForm) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('CUBE') };

    NW_RES_CTOR_INHERIT( ResParticleCubeForm, ResParticleForm )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Inner )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Scale )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, EvenInterval )

    void    Setup();
};

struct ResParticleCylinderFormData : public ResParticleFormData
{
    nw::ut::ResF32 m_Inner;
    nw::ut::ResF32 m_AngleWidth;
    nw::ut::ResF32 m_AngleOffset;
    nw::ut::ResF32 m_AngleSwing;
    nw::ut::ResVec3 m_Scale;
    nw::ut::ResBool m_FixedOffset;
    nw::ut::ResBool m_EvenInterval;
    u8              padding_0[2];
};

class ResParticleCylinderForm : public ResParticleForm
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleCylinderForm) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('CYLI') };

    NW_RES_CTOR_INHERIT( ResParticleCylinderForm, ResParticleForm )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Inner )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, AngleWidth )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, AngleOffset )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, AngleSwing )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Scale )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, FixedOffset )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, EvenInterval )

    void    Setup();
};

struct ResParticleDiscFormData : public ResParticleFormData
{
    nw::ut::ResF32 m_Inner;
    nw::ut::ResF32 m_AngleWidth;
    nw::ut::ResF32 m_AngleOffset;
    nw::ut::ResF32 m_AngleSwing;
    nw::math::VEC2 m_Scale;
    nw::ut::ResBool m_FixedOffset;
    nw::ut::ResBool m_EvenInterval;
    u8              padding_0[2];
};

class ResParticleDiscForm : public ResParticleForm
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleDiscForm) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('DISC') };

    NW_RES_CTOR_INHERIT( ResParticleDiscForm, ResParticleForm )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Inner )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, AngleWidth )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, AngleOffset )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, AngleSwing )
    NW_RES_FIELD_VECTOR2_DECL( nw::math::VEC2, Scale )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, FixedOffset )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, EvenInterval )

    void    Setup();
};

struct ResParticlePointFormData : public ResParticleFormData
{
};

class ResParticlePointForm : public ResParticleForm
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticlePointForm) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('POIN') };

    NW_RES_CTOR_INHERIT( ResParticlePointForm, ResParticleForm )

    void    Setup();
};

struct ResParticleRectangleFormData : public ResParticleFormData
{
    nw::ut::ResF32 m_Inner;
    nw::math::VEC2 m_Scale;
    nw::ut::ResBool m_EvenInterval;
    u8              padding_0[3];
};

class ResParticleRectangleForm : public ResParticleForm
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleRectangleForm) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('RECT') };

    NW_RES_CTOR_INHERIT( ResParticleRectangleForm, ResParticleForm )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Inner )
    NW_RES_FIELD_VECTOR2_DECL( nw::math::VEC2, Scale )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, EvenInterval )

    void    Setup();
};

struct ResParticleSphereFormData : public ResParticleFormData
{
    nw::ut::ResF32 m_Inner;
    nw::ut::ResF32 m_AngleWidth;
    nw::ut::ResF32 m_AngleOffset;
    nw::ut::ResF32 m_AngleSwing;
    nw::ut::ResVec3 m_Scale;
    nw::ut::ResBool m_FixedOffset;
    nw::ut::ResBool m_EvenInterval;
    u8              padding_0[2];
};

class ResParticleSphereForm : public ResParticleForm
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleSphereForm) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('SPHE') };

    NW_RES_CTOR_INHERIT( ResParticleSphereForm, ResParticleForm )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Inner )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, AngleWidth )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, AngleOffset )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, AngleSwing )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Scale )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, FixedOffset )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, EvenInterval )

    void    Setup();
};

}
}
}