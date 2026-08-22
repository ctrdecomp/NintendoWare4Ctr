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

struct ResLightData : public ResTransformNodeData{
    nw::ut::ResBool mIsLightEnabled;
    u8 padding_[3];
};

struct ResFragmentLightData : public ResLightData{
    enum Flag{
        FLAG_TWO_SIDE_DIFFUSE_ENABLED_SHIFT = ResTransformNode::FLAG_SHIFT_MAX,
        FLAG_DISTANCE_ATTENUATION_ENABLED_SHIFT,
        FLAG_INHERITING_DIRECTION_ROTATE_SHIFT,
        FLAG_SHIFT_MAX,
        
        FLAG_TWO_SIDE_DIFFUSE_ENABLED     = 0x1 << FLAG_TWO_SIDE_DIFFUSE_ENABLED_SHIFT,
        FLAG_DISTANCE_ATTENUATION_ENABLED = 0x1 << FLAG_DISTANCE_ATTENUATION_ENABLED_SHIFT,
        FLAG_IS_INHERITING_DIRECTION_ROTATE  = 0x1 << FLAG_INHERITING_DIRECTION_ROTATE_SHIFT
    };

    nw::ut::ResS32 mLightKind;
    nw::ut::ResFloatColor mAmbient;
    nw::ut::ResFloatColor mDiffuse;
    nw::ut::ResFloatColor mSpecular0;
    nw::ut::ResFloatColor mSpecular1;
    nw::ut::ResU32 mAmbientU32;
    nw::ut::ResU32 mDiffuseU32;
    nw::ut::ResU32 mSpecular0U32;
    nw::ut::ResU32 mSpecular1U32;
    nw::ut::ResVec3 mDirection;
    nw::ut::Offset toDistanceSampler;
    nw::ut::Offset toAngleSampler;
    nw::ut::ResF32 mDistanceAttenuationStart;
    nw::ut::ResF32 mDistanceAttenuationEnd;
    nw::ut::ResU32 mDistanceAttenuationScale;
    nw::ut::ResU32 mDistanceAttenuationBias;

    nw::ut::ResBool mIsDirty;
    u8 padding_[3];
};

struct ResAmbientLightData : public ResLightData{
    nw::ut::ResFloatColor mAmbient;
    nw::ut::ResU32 mAmbientU32;

    nw::ut::ResBool mIsDirty;
    u8 padding_[3];
};

struct ResVertexLightData : public ResLightData{
    enum Flag{
        FLAG_INHERITING_DIRECTION_ROTATE_SHIFT = ResTransformNode::FLAG_SHIFT_MAX,
        FLAG_SHIFT_MAX,

        FLAG_IS_INHERITING_DIRECTION_ROTATE  = 0x1 << FLAG_INHERITING_DIRECTION_ROTATE_SHIFT
    };
    
    nw::ut::ResS32 mLightKind;
    nw::ut::ResFloatColor mAmbient;
    nw::ut::ResFloatColor mDiffuse;
    nw::ut::ResVec3 mDirection;
    nw::ut::ResVec3 mDistanceAttenuation;
    nw::ut::ResF32 mIsDistanceAttenuationEnabled;
    nw::ut::VEC2 mSpotFactor;
};

struct ResHemiSphereLightData : public ResLightData{
    enum Flag{
        FLAG_INHERITING_DIRECTION_ROTATE_SHIFT = ResTransformNode::FLAG_SHIFT_MAX,
        FLAG_SHIFT_MAX,
        
        FLAG_IS_INHERITING_DIRECTION_ROTATE  = 0x1 << FLAG_INHERITING_DIRECTION_ROTATE_SHIFT
    };
    
    nw::ut::ResFloatColor mGroundColor;
    nw::ut::ResFloatColor mSkyColor;
    nw::ut::ResVec3 mDirection;
    nw::ut::ResF32 mLerpFactor;
};

class ResLight : public ResTransformNode{
public:
    enum { BINARY_REVISION = REVISION_RES_LIGHT };

    enum Kind{
        KIND_DIRECTIONAL,
        KIND_POINT,
        KIND_SPOT,
        KIND_UNUSED,
        KIND_COUNT
    };
    
    NW_RES_CTOR_INHERIT( ResLight, ResTransformNode )

    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( LightEnabled )

    u32 GetRevision() const { return this->GetHeader().revision; }

    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    void Cleanup();
};
typedef nw::ut::ResArrayPatricia<ResLight>::type  ResLightArray;

class ResFragmentLight : public ResLight{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResFragmentLight) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CFLT') };
    
    NW_RES_CTOR_INHERIT( ResFragmentLight, ResLight )

    NW_RES_FIELD_PRIMITIVE_DECL( s32, LightKind )
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Ambient )
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Diffuse )
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Specular0 )
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Specular1 )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Direction )
    NW_RES_FIELD_CLASS_DECL( ResLookupTable, DistanceSampler )
    NW_RES_FIELD_CLASS_DECL( ResLightingLookupTable, AngleSampler )

    void SetDistanceSampler(ResImageLookupTable lookupTable){
        NW_ASSERT(lookupTable.IsValid());
        ResReferenceLookupTable referenceLut = ResStaticCast<ResReferenceLookupTable>(GetDistanceSampler());
        referenceLut.ref().toTargetLut.set_ptr(reinterpret_cast<const char*>(lookupTable.ptr()));
    }

    f32 GetDistanceAttenuationStart() const{
        return ref().mDistanceAttenuationStart;
    }

    f32 GetDistanceAttenuationEnd() const{
        return ref().mDistanceAttenuationEnd;
    }

    void SetDistanceAttenuationStart(f32 start){
        ref().mDistanceAttenuationStart = start;
        SetDistanceAttenuation(ref().mDistanceAttenuationStart, ref().mDistanceAttenuationEnd);
    }
    
    void SetDistanceAttenuationEnd(f32 end){
        ref().mDistanceAttenuationEnd = end;
        SetDistanceAttenuation(ref().mDistanceAttenuationStart, ref().mDistanceAttenuationEnd);
    }
    
    void SetDistanceAttenuation(f32 start, f32 end){
        f32 diff = end - start;
        
        f32 minimumDistance = 0.01f;
        if (nw::math::FAbs(diff) < minimumDistance){
            diff = minimumDistance;
        }
        
        ref().mDistanceAttenuationScale = nw::ut::Float20::Float32ToBits20(1.0f / diff);
        ref().mDistanceAttenuationBias = nw::ut::Float20::Float32ToBits20(-start / diff);
    }
    
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Dirty )

    void ForceSetupDistanceSampler(const char* targetName, ResLookupTable lookupTable){
        NW_ASSERT(lookupTable.IsValid());
        ResReferenceLookupTable referenceLut = ResStaticCast<ResReferenceLookupTable>(GetDistanceSampler());
        referenceLut.ForceSetup(targetName, lookupTable);
    }
};

class ResAmbientLight : public ResLight{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResAmbientLight) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CALT') };
    
    NW_RES_CTOR_INHERIT( ResAmbientLight, ResLight )

    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Ambient )
};

class ResVertexLight : public ResLight{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexLight) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CVLT') };
 
    NW_RES_CTOR_INHERIT( ResVertexLight, ResLight )

    NW_RES_FIELD_PRIMITIVE_DECL( s32, LightKind )
    NW_RES_FIELD_FLOAT_COLOR_DECL( nw::ut::FloatColor, Ambient )
    NW_RES_FIELD_FLOAT_COLOR_DECL( nw::ut::FloatColor, Diffuse )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Direction )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, DistanceAttenuation )
    NW_RES_FIELD_VECTOR2_DECL( nw::math::VEC2, SpotFactor )

    bool IsDistanceAttenuationEnabled() const { return ref().mIsDistanceAttenuationEnabled != 0.0f; }

    void SetDistanceAttenuationEnabled(bool enable){ref().mIsDistanceAttenuationEnabled = enable ? 1.0f : 0.0f;}

    nw::math::VEC4& GetDistanceAttenuationAndEnabled(){return reinterpret_cast<nw::math::VEC4&>( ref().mDistanceAttenuation );}

    const nw::math::VEC4& GetDistanceAttenuationAndEnabled() const{return reinterpret_cast<const nw::math::VEC4&>( ref().mDistanceAttenuation );}
};


class ResHemiSphereLight : public ResLight{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResHemiSphereLight) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CHLT') };
    
    NW_RES_CTOR_INHERIT( ResHemiSphereLight, ResLight )

    NW_RES_FIELD_FLOAT_COLOR_DECL( nw::ut::FloatColor, GroundColor )
    NW_RES_FIELD_FLOAT_COLOR_DECL( nw::ut::FloatColor, SkyColor )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Direction )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, LerpFactor )

    nw::math::VEC4& GetDirectionAndLerp(){return reinterpret_cast<nw::math::VEC4&>( ref().mDirection );}

    const nw::math::VEC4& GetDirectionAndLerp() const{return reinterpret_cast<const nw::math::VEC4&>( ref().mDirection );}
};

}
}
}