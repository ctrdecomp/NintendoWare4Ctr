#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/gfx/res/gfx_ResFragmentShader.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/ut/ut_Float24.h>

namespace nw {
namespace gfx {
namespace res {

class ResGraphicsFile;

struct ResLightData : public ResTransformNodeData
{
    nw::ut::ResBool m_IsLightEnabled;
    u8 padding_[3];
};

struct ResFragmentLightData : public ResLightData
{
    enum Flag
    {
        FLAG_TWO_SIDE_DIFFUSE_ENABLED_SHIFT = ResTransformNode::FLAG_SHIFT_MAX,
        FLAG_DISTANCE_ATTENUATION_ENABLED_SHIFT,
        FLAG_INHERITING_DIRECTION_ROTATE_SHIFT,
        FLAG_SHIFT_MAX,
        
        FLAG_TWO_SIDE_DIFFUSE_ENABLED     = 0x1 << FLAG_TWO_SIDE_DIFFUSE_ENABLED_SHIFT,
        FLAG_DISTANCE_ATTENUATION_ENABLED = 0x1 << FLAG_DISTANCE_ATTENUATION_ENABLED_SHIFT,
        FLAG_IS_INHERITING_DIRECTION_ROTATE  = 0x1 << FLAG_INHERITING_DIRECTION_ROTATE_SHIFT
    };

    nw::ut::ResS32 m_LightKind;
    nw::ut::ResFloatColor m_Ambient;
    nw::ut::ResFloatColor m_Diffuse;
    nw::ut::ResFloatColor m_Specular0;
    nw::ut::ResFloatColor m_Specular1;
    nw::ut::ResU32 m_AmbientU32;
    nw::ut::ResU32 m_DiffuseU32;
    nw::ut::ResU32 m_Specular0U32;
    nw::ut::ResU32 m_Specular1U32;
    nw::ut::ResVec3 m_Direction;
    nw::ut::Offset toDistanceSampler;
    nw::ut::Offset toAngleSampler;
    nw::ut::ResF32 m_DistanceAttenuationStart;
    nw::ut::ResF32 m_DistanceAttenuationEnd;
    nw::ut::ResU32 m_DistanceAttenuationScale;
    nw::ut::ResU32 m_DistanceAttenuationBias;

    nw::ut::ResBool m_IsDirty;
    u8 padding_[3];
};

struct ResAmbientLightData : public ResLightData
{
    nw::ut::ResFloatColor m_Ambient;
    nw::ut::ResU32 m_AmbientU32;

    nw::ut::ResBool m_IsDirty;
    u8 padding_[3];
};

struct ResVertexLightData : public ResLightData
{
    enum Flag
    {
        FLAG_INHERITING_DIRECTION_ROTATE_SHIFT = ResTransformNode::FLAG_SHIFT_MAX,
        FLAG_SHIFT_MAX,

        FLAG_IS_INHERITING_DIRECTION_ROTATE  = 0x1 << FLAG_INHERITING_DIRECTION_ROTATE_SHIFT
    };
    
    nw::ut::ResS32 m_LightKind;
    nw::ut::ResFloatColor m_Ambient;
    nw::ut::ResFloatColor m_Diffuse;
    nw::ut::ResVec3 m_Direction;
    nw::ut::ResVec3 m_DistanceAttenuation;
    nw::ut::ResF32 m_IsDistanceAttenuationEnabled;
    nw::math::VEC2 m_SpotFactor;
};

struct ResHemiSphereLightData : public ResLightData
{
    enum Flag
    {
        FLAG_INHERITING_DIRECTION_ROTATE_SHIFT = ResTransformNode::FLAG_SHIFT_MAX,
        FLAG_SHIFT_MAX,
        
        FLAG_IS_INHERITING_DIRECTION_ROTATE  = 0x1 << FLAG_INHERITING_DIRECTION_ROTATE_SHIFT
    };
    
    nw::ut::ResFloatColor m_GroundColor;
    nw::ut::ResFloatColor m_SkyColor;
    nw::ut::ResVec3 m_Direction;
    nw::ut::ResF32 m_LerpFactor;
};

class ResLight : public ResTransformNode
{
public:
    enum
{ BINARY_REVISION = REVISION_RES_LIGHT };

    enum Kind
    {
        KIND_DIRECTIONAL,
        KIND_POINT,
        KIND_SPOT,
        KIND_UNUSED,
        KIND_COUNT
    };
    
    NW_RES_CTOR_INHERIT(ResLight, ResTransformNode)


    bool IsLightEnabled() const { return static_cast<bool>(ref().m_IsLightEnabled); }
    void SetLightEnabled(bool value) { ref().m_IsLightEnabled = value; }

    u32 GetRevision() const { return this->GetHeader().revision; }

    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    void Cleanup();
};
typedef nw::ut::ResArrayPatricia<ResLight>::type  ResLightArray;

class ResFragmentLight : public ResLight
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResFragmentLight)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('CFLT')
    };
    
    NW_RES_CTOR_INHERIT( ResFragmentLight, ResLight )


    s32 GetLightKind() const { return static_cast<s32>(ref().m_LightKind); }
    void SetLightKind(s32 value) { ref().m_LightKind = value; }

    u32 GetAmbientU32() const { return ref().m_AmbientU32; }
    void SetAmbient(f32 r, f32 g, f32 b) { ref().m_Ambient.Set(r, g, b); ref().m_AmbientU32 = ref().m_Ambient.ToPicaU32(); }
    void SetAmbient(f32 r, f32 g, f32 b, f32 a) { ref().m_Ambient.Set(r, g, b, a); ref().m_AmbientU32 = ref().m_Ambient.ToPicaU32(); }
    const nw::ut::FloatColor& GetAmbient() const { return ref().m_Ambient; }
    void SetAmbient(const nw::ut::FloatColor& value) { ref().m_Ambient = value; ref().m_AmbientU32 = ref().m_Ambient.ToPicaU32(); }

    u32 GetDiffuseU32() const { return ref().m_DiffuseU32; }
    void SetDiffuse(f32 r, f32 g, f32 b) { ref().m_Diffuse.Set(r, g, b); ref().m_DiffuseU32 = ref().m_Diffuse.ToPicaU32(); }
    void SetDiffuse(f32 r, f32 g, f32 b, f32 a) { ref().m_Diffuse.Set(r, g, b, a); ref().m_DiffuseU32 = ref().m_Diffuse.ToPicaU32(); }
    const nw::ut::FloatColor& GetDiffuse() const { return ref().m_Diffuse; }
    void SetDiffuse(const nw::ut::FloatColor& value) { ref().m_Diffuse = value; ref().m_DiffuseU32 = ref().m_Diffuse.ToPicaU32(); }

    u32 GetSpecular0U32() const { return ref().m_Specular0U32; }
    void SetSpecular0(f32 r, f32 g, f32 b) { ref().m_Specular0.Set(r, g, b); ref().m_Specular0U32 = ref().m_Specular0.ToPicaU32(); }
    void SetSpecular0(f32 r, f32 g, f32 b, f32 a) { ref().m_Specular0.Set(r, g, b, a); ref().m_Specular0U32 = ref().m_Specular0.ToPicaU32(); }
    const nw::ut::FloatColor& GetSpecular0() const { return ref().m_Specular0; }
    void SetSpecular0(const nw::ut::FloatColor& value) { ref().m_Specular0 = value; ref().m_Specular0U32 = ref().m_Specular0.ToPicaU32(); }

    u32 GetSpecular1U32() const { return ref().m_Specular1U32; }
    void SetSpecular1(f32 r, f32 g, f32 b) { ref().m_Specular1.Set(r, g, b); ref().m_Specular1U32 = ref().m_Specular1.ToPicaU32(); }
    void SetSpecular1(f32 r, f32 g, f32 b, f32 a) { ref().m_Specular1.Set(r, g, b, a); ref().m_Specular1U32 = ref().m_Specular1.ToPicaU32(); }
    const nw::ut::FloatColor& GetSpecular1() const { return ref().m_Specular1; }
    void SetSpecular1(const nw::ut::FloatColor& value) { ref().m_Specular1 = value; ref().m_Specular1U32 = ref().m_Specular1.ToPicaU32(); }

    void SetDirection(f32 x, f32 y, f32 z) { ref().m_Direction.Set(x, y, z); }
    const nw::math::VEC3& GetDirection() const { return ref().m_Direction; }
    void SetDirection(const nw::math::VEC3& value) { ref().m_Direction = value; }

    ResLookupTable GetDistanceSampler() { return ResLookupTable(ref().toDistanceSampler.to_ptr()); }
    const ResLookupTable GetDistanceSampler() const { return ResLookupTable(ref().toDistanceSampler.to_ptr()); }

    ResLightingLookupTable GetAngleSampler() { return ResLightingLookupTable(ref().toAngleSampler.to_ptr()); }
    const ResLightingLookupTable GetAngleSampler() const { return ResLightingLookupTable(ref().toAngleSampler.to_ptr()); }

    void SetDistanceSampler(ResImageLookupTable lookupTable)
    {
        NW_ASSERT(lookupTable.IsValid());
        ResReferenceLookupTable referenceLut = ResStaticCast<ResReferenceLookupTable>(GetDistanceSampler());
        referenceLut.ref().toTargetLut.set_ptr(reinterpret_cast<const char*>(lookupTable.ptr()));
    }

    f32 GetDistanceAttenuationStart() const
    {
        return ref().m_DistanceAttenuationStart;
    }

    f32 GetDistanceAttenuationEnd() const
    {
        return ref().m_DistanceAttenuationEnd;
    }

    void SetDistanceAttenuationStart(f32 start)
    {
        ref().m_DistanceAttenuationStart = start;
        SetDistanceAttenuation(ref().m_DistanceAttenuationStart, ref().m_DistanceAttenuationEnd);
    }
    
    void SetDistanceAttenuationEnd(f32 end)
    {
        ref().m_DistanceAttenuationEnd = end;
        SetDistanceAttenuation(ref().m_DistanceAttenuationStart, ref().m_DistanceAttenuationEnd);
    }
    
    void SetDistanceAttenuation(f32 start, f32 end)
    {
        f32 diff = end - start;
        
        f32 minimumDistance = 0.01f;
        if (nw::math::FAbs(diff) < minimumDistance)
        {
            diff = minimumDistance;
        }
        
        ref().m_DistanceAttenuationScale = nw::ut::Float20::Float32ToBits20(1.0f / diff);
        ref().m_DistanceAttenuationBias = nw::ut::Float20::Float32ToBits20(-start / diff);
    }
    

    bool IsDirty() const { return static_cast<bool>(ref().m_IsDirty); }
    void SetDirty(bool value) { ref().m_IsDirty = value; }

    void ForceSetupDistanceSampler(const char* targetName, ResLookupTable lookupTable)
    {
        NW_ASSERT(lookupTable.IsValid());
        ResReferenceLookupTable referenceLut = ResStaticCast<ResReferenceLookupTable>(GetDistanceSampler());
        referenceLut.ForceSetup(targetName, lookupTable);
    }
};

class ResAmbientLight : public ResLight
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResAmbientLight)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('CALT')
    };
    
    NW_RES_CTOR_INHERIT( ResAmbientLight, ResLight )


    u32 GetAmbientU32() const { return ref().m_AmbientU32; }
    void SetAmbient(f32 r, f32 g, f32 b) { ref().m_Ambient.Set(r, g, b); ref().m_AmbientU32 = ref().m_Ambient.ToPicaU32(); }
    void SetAmbient(f32 r, f32 g, f32 b, f32 a) { ref().m_Ambient.Set(r, g, b, a); ref().m_AmbientU32 = ref().m_Ambient.ToPicaU32(); }
    const nw::ut::FloatColor& GetAmbient() const { return ref().m_Ambient; }
    void SetAmbient(const nw::ut::FloatColor& value) { ref().m_Ambient = value; ref().m_AmbientU32 = ref().m_Ambient.ToPicaU32(); }
};

class ResVertexLight : public ResLight
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexLight)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('CVLT')
    };
 
    NW_RES_CTOR_INHERIT(ResVertexLight, ResLight)


    s32 GetLightKind() const { return static_cast<s32>(ref().m_LightKind); }
    void SetLightKind(s32 value) { ref().m_LightKind = value; }

    void SetAmbient(f32 r, f32 g, f32 b) { ref().m_Ambient.Set(r, g, b); }
    void SetAmbient(f32 r, f32 g, f32 b, f32 a) { ref().m_Ambient.Set(r, g, b, a); }
    const nw::ut::FloatColor& GetAmbient() const { return ref().m_Ambient; }
    void SetAmbient(const nw::ut::FloatColor& value) { ref().m_Ambient = value; }

    void SetDiffuse(f32 r, f32 g, f32 b) { ref().m_Diffuse.Set(r, g, b); }
    void SetDiffuse(f32 r, f32 g, f32 b, f32 a) { ref().m_Diffuse.Set(r, g, b, a); }
    const nw::ut::FloatColor& GetDiffuse() const { return ref().m_Diffuse; }
    void SetDiffuse(const nw::ut::FloatColor& value) { ref().m_Diffuse = value; }

    void SetDirection(f32 x, f32 y, f32 z) { ref().m_Direction.Set(x, y, z); }
    const nw::math::VEC3& GetDirection() const { return ref().m_Direction; }
    void SetDirection(const nw::math::VEC3& value) { ref().m_Direction = value; }

    void SetDistanceAttenuation(f32 x, f32 y, f32 z) { ref().m_DistanceAttenuation.Set(x, y, z); }
    const nw::math::VEC3& GetDistanceAttenuation() const { return ref().m_DistanceAttenuation; }
    void SetDistanceAttenuation(const nw::math::VEC3& value) { ref().m_DistanceAttenuation = value; }

    void SetSpotFactor(f32 x, f32 y) { ref().m_SpotFactor.Set(x, y); }
    const nw::math::VEC2& GetSpotFactor() const { return ref().m_SpotFactor; }
    void SetSpotFactor(const nw::math::VEC2& value) { ref().m_SpotFactor = value; }

    bool IsDistanceAttenuationEnabled() const { return ref().m_IsDistanceAttenuationEnabled != 0.0f; }

    void SetDistanceAttenuationEnabled(bool enable) {ref().m_IsDistanceAttenuationEnabled = enable ? 1.0f : 0.0f;}

    nw::math::VEC4& GetDistanceAttenuationAndEnabled() {return reinterpret_cast<nw::math::VEC4&>( ref().m_DistanceAttenuation );}

    const nw::math::VEC4& GetDistanceAttenuationAndEnabled() const {return reinterpret_cast<const nw::math::VEC4&>( ref().m_DistanceAttenuation );}
};


class ResHemiSphereLight : public ResLight
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResHemiSphereLight)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('CHLT')
    };
    
    NW_RES_CTOR_INHERIT( ResHemiSphereLight, ResLight )


    void SetGroundColor(f32 r, f32 g, f32 b) { ref().m_GroundColor.Set(r, g, b); }
    void SetGroundColor(f32 r, f32 g, f32 b, f32 a) { ref().m_GroundColor.Set(r, g, b, a); }
    const nw::ut::FloatColor& GetGroundColor() const { return ref().m_GroundColor; }
    void SetGroundColor(const nw::ut::FloatColor& value) { ref().m_GroundColor = value; }

    void SetSkyColor(f32 r, f32 g, f32 b) { ref().m_SkyColor.Set(r, g, b); }
    void SetSkyColor(f32 r, f32 g, f32 b, f32 a) { ref().m_SkyColor.Set(r, g, b, a); }
    const nw::ut::FloatColor& GetSkyColor() const { return ref().m_SkyColor; }
    void SetSkyColor(const nw::ut::FloatColor& value) { ref().m_SkyColor = value; }

    void SetDirection(f32 x, f32 y, f32 z) { ref().m_Direction.Set(x, y, z); }
    const nw::math::VEC3& GetDirection() const { return ref().m_Direction; }
    void SetDirection(const nw::math::VEC3& value) { ref().m_Direction = value; }

    f32 GetLerpFactor() const { return static_cast<f32>(ref().m_LerpFactor); }
    void SetLerpFactor(f32 value) { ref().m_LerpFactor = value; }

    nw::math::VEC4& GetDirectionAndLerp() {return reinterpret_cast<nw::math::VEC4&>( ref().m_Direction );}

    const nw::math::VEC4& GetDirectionAndLerp() const {return reinterpret_cast<const nw::math::VEC4&>( ref().m_Direction );}
};

}
}
}

