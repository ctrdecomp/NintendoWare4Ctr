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


    bool GetIsResourceCopyEnabled() const { return static_cast<bool>(ref().m_IsResourceCopyEnabled); }
    void SetIsResourceCopyEnabled(bool value) { ref().m_IsResourceCopyEnabled = value; }

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


    f32 GetInner() const { return static_cast<f32>(ref().m_Inner); }
    void SetInner(f32 value) { ref().m_Inner = value; }

    void SetScale(f32 x, f32 y, f32 z) { ref().m_Scale.Set(x, y, z); }
    const nw::math::VEC3& GetScale() const { return ref().m_Scale; }
    void SetScale(const nw::math::VEC3& value) { ref().m_Scale = value; }

    bool GetEvenInterval() const { return static_cast<bool>(ref().m_EvenInterval); }
    void SetEvenInterval(bool value) { ref().m_EvenInterval = value; }

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


    f32 GetInner() const { return static_cast<f32>(ref().m_Inner); }
    void SetInner(f32 value) { ref().m_Inner = value; }

    f32 GetAngleWidth() const { return static_cast<f32>(ref().m_AngleWidth); }
    void SetAngleWidth(f32 value) { ref().m_AngleWidth = value; }

    f32 GetAngleOffset() const { return static_cast<f32>(ref().m_AngleOffset); }
    void SetAngleOffset(f32 value) { ref().m_AngleOffset = value; }

    f32 GetAngleSwing() const { return static_cast<f32>(ref().m_AngleSwing); }
    void SetAngleSwing(f32 value) { ref().m_AngleSwing = value; }

    void SetScale(f32 x, f32 y, f32 z) { ref().m_Scale.Set(x, y, z); }
    const nw::math::VEC3& GetScale() const { return ref().m_Scale; }
    void SetScale(const nw::math::VEC3& value) { ref().m_Scale = value; }

    bool GetFixedOffset() const { return static_cast<bool>(ref().m_FixedOffset); }
    void SetFixedOffset(bool value) { ref().m_FixedOffset = value; }

    bool GetEvenInterval() const { return static_cast<bool>(ref().m_EvenInterval); }
    void SetEvenInterval(bool value) { ref().m_EvenInterval = value; }

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


    f32 GetInner() const { return static_cast<f32>(ref().m_Inner); }
    void SetInner(f32 value) { ref().m_Inner = value; }

    f32 GetAngleWidth() const { return static_cast<f32>(ref().m_AngleWidth); }
    void SetAngleWidth(f32 value) { ref().m_AngleWidth = value; }

    f32 GetAngleOffset() const { return static_cast<f32>(ref().m_AngleOffset); }
    void SetAngleOffset(f32 value) { ref().m_AngleOffset = value; }

    f32 GetAngleSwing() const { return static_cast<f32>(ref().m_AngleSwing); }
    void SetAngleSwing(f32 value) { ref().m_AngleSwing = value; }

    void SetScale(f32 x, f32 y) { ref().m_Scale.Set(x, y); }
    const nw::math::VEC2& GetScale() const { return ref().m_Scale; }
    void SetScale(const nw::math::VEC2& value) { ref().m_Scale = value; }

    bool GetFixedOffset() const { return static_cast<bool>(ref().m_FixedOffset); }
    void SetFixedOffset(bool value) { ref().m_FixedOffset = value; }

    bool GetEvenInterval() const { return static_cast<bool>(ref().m_EvenInterval); }
    void SetEvenInterval(bool value) { ref().m_EvenInterval = value; }

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


    f32 GetInner() const { return static_cast<f32>(ref().m_Inner); }
    void SetInner(f32 value) { ref().m_Inner = value; }

    void SetScale(f32 x, f32 y) { ref().m_Scale.Set(x, y); }
    const nw::math::VEC2& GetScale() const { return ref().m_Scale; }
    void SetScale(const nw::math::VEC2& value) { ref().m_Scale = value; }

    bool GetEvenInterval() const { return static_cast<bool>(ref().m_EvenInterval); }
    void SetEvenInterval(bool value) { ref().m_EvenInterval = value; }

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


    f32 GetInner() const { return static_cast<f32>(ref().m_Inner); }
    void SetInner(f32 value) { ref().m_Inner = value; }

    f32 GetAngleWidth() const { return static_cast<f32>(ref().m_AngleWidth); }
    void SetAngleWidth(f32 value) { ref().m_AngleWidth = value; }

    f32 GetAngleOffset() const { return static_cast<f32>(ref().m_AngleOffset); }
    void SetAngleOffset(f32 value) { ref().m_AngleOffset = value; }

    f32 GetAngleSwing() const { return static_cast<f32>(ref().m_AngleSwing); }
    void SetAngleSwing(f32 value) { ref().m_AngleSwing = value; }

    void SetScale(f32 x, f32 y, f32 z) { ref().m_Scale.Set(x, y, z); }
    const nw::math::VEC3& GetScale() const { return ref().m_Scale; }
    void SetScale(const nw::math::VEC3& value) { ref().m_Scale = value; }

    bool GetFixedOffset() const { return static_cast<bool>(ref().m_FixedOffset); }
    void SetFixedOffset(bool value) { ref().m_FixedOffset = value; }

    bool GetEvenInterval() const { return static_cast<bool>(ref().m_EvenInterval); }
    void SetEvenInterval(bool value) { ref().m_EvenInterval = value; }

    void    Setup();
};

}
}
}

