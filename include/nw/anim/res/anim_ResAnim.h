#pragma once

#include <nw/types.h>
#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDeclMacros.h>
#include <nw/ut/ut_ResArrayTypes.h>
#include <nw/anim/res/anim_ResAnimCurve.h>
#include <nw/anim/anim_AnimBlend.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/anim/res/anim_ResRevision.h>

namespace nw {
namespace gfx {

class CalculatedTransform;

namespace res {

class ResGraphicsFile;

}
}

namespace anim {

namespace res {}
using namespace ::nw::anim::res;

namespace res {

struct ResAnimData{
    enum LoopMode{
        LOOP_MODE_ONETIME,
        LOOP_MODE_LOOP
    };

    nw::ut::BinaryRevisionHeader mHeader;
    nw::ut::BinString   toName;
    nw::ut::BinString   toTargetAnimGroupName;
    nw::ut::ResU8       mLoopMode;
    u8              padding_[3];
    nw::ut::ResF32      mFrameSize;
    nw::ut::ResS32      mMemberAnimSetDicCount;
    nw::ut::Offset      toMemberAnimSetDic;
    nw::ut::ResS32      mUserDataDicCount;
    nw::ut::Offset      toUserDataDic;
};

struct ResLightAnimData : public ResAnimData{
    enum LightType{
        LIGHT_TYPE_FRAGMENT,
        LIGHT_TYPE_VERTEX,
        LIGHT_TYPE_HEMISPHERE,
        LIGHT_TYPE_AMBIENT
    };

    nw::ut::ResU32 mLightKind;

    nw::ut::ResU32 mLightType;
};

struct ResCameraAnimData : public ResAnimData{
    enum ProjectionUpdaterKind{
        PROJECTION_UPDATER_PERSPECTIVE,
        PROJECTION_UPDATER_ORTHO,
        PROJECTION_UPDATER_FRUSTUM
    };

    enum ViewUpdaterKind{
        VIEW_UPDATER_AIM,
        VIEW_UPDATER_LOOKAT,
        VIEW_UPDATER_ROTATE
    };

    nw::ut::ResU32 mProjectionUpdaterKind;

    nw::ut::ResU32 mViewUpdaterKind;
};


struct ResMemberAnimData{
    nw::ut::ResU32                      mFlags;
    nw::ut::BinString                   toPath;
    nw::ut::ResU32                      mPrimitiveType;
};

struct ResFloatAnimData : public ResMemberAnimData{
    enum Flag{
        FLAG_CONSTANT = 0x1 << 0
    };
    
    union{
        nw::ut::ResF32      constantValue;
        nw::ut::Offset      toCurve;
    };
};

struct ResIntAnimData : public ResMemberAnimData{
    enum Flag{
        FLAG_CONSTANT = 0x1 << 0
    };
    
    union{
        nw::ut::ResS32      constantValue;
        nw::ut::Offset      toCurve;
    };
};

struct ResBoolAnimData : public ResMemberAnimData{
    enum Flag{
        FLAG_CONSTANT       = 0x1 << 0,
        FLAG_CONSTANT_VALUE = 0x1 << 1
    };
    nw::ut::Offset      toCurve;
};

struct ResVec2AnimData : public ResMemberAnimData{
    enum Flag{
        FLAG_CONSTANT_X_SHIFT = 0,
        FLAG_CONSTANT_Y_SHIFT,
        FLAG_X_NOT_EXIST_SHIFT,
        FLAG_Y_NOT_EXIST_SHIFT,
        
        FLAG_CONSTANT_X     = 0x1 << FLAG_CONSTANT_X_SHIFT,
        FLAG_CONSTANT_Y     = 0x1 << FLAG_CONSTANT_Y_SHIFT,
        FLAG_X_NOT_EXIST    = 0x1 << FLAG_X_NOT_EXIST_SHIFT,
        FLAG_Y_NOT_EXIST    = 0x1 << FLAG_Y_NOT_EXIST_SHIFT
    };
    
    union{
        nw::ut::ResF32      constantValueCurveX;
        nw::ut::Offset      toCurveX;
    };
    union{
        nw::ut::ResF32      constantValueCurveY;
        nw::ut::Offset      toCurveY;
    };
};

struct ResVec3AnimData : public ResMemberAnimData{
    enum Flag{
        FLAG_CONSTANT_X_SHIFT = 0,
        FLAG_CONSTANT_Y_SHIFT,
        FLAG_CONSTANT_Z_SHIFT,
        FLAG_X_NOT_EXIST_SHIFT,
        FLAG_Y_NOT_EXIST_SHIFT,
        FLAG_Z_NOT_EXIST_SHIFT,
        
        FLAG_CONSTANT_X     = 0x1 << FLAG_CONSTANT_X_SHIFT,
        FLAG_CONSTANT_Y     = 0x1 << FLAG_CONSTANT_Y_SHIFT,
        FLAG_CONSTANT_Z     = 0x1 << FLAG_CONSTANT_Z_SHIFT,
        FLAG_X_NOT_EXIST    = 0x1 << FLAG_X_NOT_EXIST_SHIFT,
        FLAG_Y_NOT_EXIST    = 0x1 << FLAG_Y_NOT_EXIST_SHIFT,
        FLAG_Z_NOT_EXIST    = 0x1 << FLAG_Z_NOT_EXIST_SHIFT
    };
    
    union{
        nw::ut::ResF32      constantValueCurveX;
        nw::ut::Offset      toCurveX;
    };
    union{
        nw::ut::ResF32      constantValueCurveY;
        nw::ut::Offset      toCurveY;
    };
    union{
        nw::ut::ResF32      constantValueCurveZ;
        nw::ut::Offset      toCurveZ;
    };
};

struct ResRgbaColorAnimData : public ResMemberAnimData{
    enum Flag{
        FLAG_CONSTANT_R_SHIFT = 0,
        FLAG_CONSTANT_G_SHIFT,
        FLAG_CONSTANT_B_SHIFT,
        FLAG_CONSTANT_A_SHIFT,
        FLAG_R_NOT_EXIST_SHIFT,
        FLAG_G_NOT_EXIST_SHIFT,
        FLAG_B_NOT_EXIST_SHIFT,
        FLAG_A_NOT_EXIST_SHIFT,
        
        FLAG_CONSTANT_R     = 0x1 << FLAG_CONSTANT_R_SHIFT,
        FLAG_CONSTANT_G     = 0x1 << FLAG_CONSTANT_G_SHIFT,
        FLAG_CONSTANT_B     = 0x1 << FLAG_CONSTANT_B_SHIFT,
        FLAG_CONSTANT_A     = 0x1 << FLAG_CONSTANT_A_SHIFT,
        FLAG_R_NOT_EXIST    = 0x1 << FLAG_R_NOT_EXIST_SHIFT,
        FLAG_G_NOT_EXIST    = 0x1 << FLAG_G_NOT_EXIST_SHIFT,
        FLAG_B_NOT_EXIST    = 0x1 << FLAG_B_NOT_EXIST_SHIFT,
        FLAG_A_NOT_EXIST    = 0x1 << FLAG_A_NOT_EXIST_SHIFT
    };
    
    union{
        nw::ut::ResF32      constantValueCurveR;
        nw::ut::Offset      toCurveR;
    };
    union{
        nw::ut::ResF32      constantValueCurveG;
        nw::ut::Offset      toCurveG;
    };
    union{
        nw::ut::ResF32      constantValueCurveB;
        nw::ut::Offset      toCurveB;
    };
    union{
        nw::ut::ResF32      constantValueCurveA;
        nw::ut::Offset      toCurveA;
    };
};

struct ResTextureAnimData : public ResMemberAnimData{
    enum Flag{
        FLAG_CONSTANT = 0x1 << 0
    };

    union{
        nw::ut::ResF32      constantValueCurve;
        nw::ut::Offset      toCurve;
    };
    
    nw::ut::ResS32 mTexturesTableCount;
    nw::ut::Offset toTexturesTable;
};

struct ResTransformAnimData : public ResMemberAnimData{
    enum Flag{
        FLAG_IDENTITY_SHIFT     = 0,
        FLAG_RT_ZERO_SHIFT,
        FLAG_SCALE_ONE_SHIFT,
        FLAG_SCALE_UNIFORM_SHIFT,
        FLAG_ROTATE_ZERO_SHIFT,
        FLAG_TRANSLATE_ZERO_SHIFT,
        
        FLAG_CONSTANT_SCALE_X_SHIFT,
        FLAG_CONSTANT_SCALE_Y_SHIFT,
        FLAG_CONSTANT_SCALE_Z_SHIFT,
        FLAG_CONSTANT_ROTATE_X_SHIFT,
        FLAG_CONSTANT_ROTATE_Y_SHIFT,
        FLAG_CONSTANT_ROTATE_Z_SHIFT,
        FLAG_CONSTANT_ROTATE_W_SHIFT,
        FLAG_CONSTANT_TRANSLATE_X_SHIFT,
        FLAG_CONSTANT_TRANSLATE_Y_SHIFT,
        FLAG_CONSTANT_TRANSLATE_Z_SHIFT,
        
        FLAG_SCALE_X_NOT_EXIST_SHIFT,
        FLAG_SCALE_Y_NOT_EXIST_SHIFT,
        FLAG_SCALE_Z_NOT_EXIST_SHIFT,
        FLAG_ROTATE_X_NOT_EXIST_SHIFT,
        FLAG_ROTATE_Y_NOT_EXIST_SHIFT,
        FLAG_ROTATE_Z_NOT_EXIST_SHIFT,
        FLAG_ROTATE_W_NOT_EXIST_SHIFT,
        FLAG_TRANSLATE_X_NOT_EXIST_SHIFT,
        FLAG_TRANSLATE_Y_NOT_EXIST_SHIFT,
        FLAG_TRANSLATE_Z_NOT_EXIST_SHIFT,

        FLAG_IDENTITY               = 0x1 << FLAG_IDENTITY_SHIFT, 
        FLAG_RT_ZERO                = 0x1 << FLAG_RT_ZERO_SHIFT, 
        FLAG_SCALE_ONE              = 0x1 << FLAG_SCALE_ONE_SHIFT, 
        FLAG_SCALE_UNIFORM          = 0x1 << FLAG_SCALE_UNIFORM_SHIFT, 
        FLAG_ROTATE_ZERO            = 0x1 << FLAG_ROTATE_ZERO_SHIFT,
        FLAG_TRANSLATE_ZERO         = 0x1 << FLAG_TRANSLATE_ZERO_SHIFT,
        
        FLAG_CONSTANT_SCALE_X       = 0x1 << FLAG_CONSTANT_SCALE_X_SHIFT,
        FLAG_CONSTANT_SCALE_Y       = 0x1 << FLAG_CONSTANT_SCALE_Y_SHIFT,
        FLAG_CONSTANT_SCALE_Z       = 0x1 << FLAG_CONSTANT_SCALE_Z_SHIFT,
        FLAG_CONSTANT_ROTATE_X      = 0x1 << FLAG_CONSTANT_ROTATE_X_SHIFT,
        FLAG_CONSTANT_ROTATE_Y      = 0x1 << FLAG_CONSTANT_ROTATE_Y_SHIFT,
        FLAG_CONSTANT_ROTATE_Z      = 0x1 << FLAG_CONSTANT_ROTATE_Z_SHIFT,
        FLAG_CONSTANT_ROTATE_W      = 0x1 << FLAG_CONSTANT_ROTATE_W_SHIFT,
        FLAG_CONSTANT_TRANSLATE_X   = 0x1 << FLAG_CONSTANT_TRANSLATE_X_SHIFT,
        FLAG_CONSTANT_TRANSLATE_Y   = 0x1 << FLAG_CONSTANT_TRANSLATE_Y_SHIFT,
        FLAG_CONSTANT_TRANSLATE_Z   = 0x1 << FLAG_CONSTANT_TRANSLATE_Z_SHIFT,
        
        FLAG_SCALE_X_NOT_EXIST      = 0x1 << FLAG_SCALE_X_NOT_EXIST_SHIFT,
        FLAG_SCALE_Y_NOT_EXIST      = 0x1 << FLAG_SCALE_Y_NOT_EXIST_SHIFT,
        FLAG_SCALE_Z_NOT_EXIST      = 0x1 << FLAG_SCALE_Z_NOT_EXIST_SHIFT,
        FLAG_ROTATE_X_NOT_EXIST     = 0x1 << FLAG_ROTATE_X_NOT_EXIST_SHIFT,
        FLAG_ROTATE_Y_NOT_EXIST     = 0x1 << FLAG_ROTATE_Y_NOT_EXIST_SHIFT,
        FLAG_ROTATE_Z_NOT_EXIST     = 0x1 << FLAG_ROTATE_Z_NOT_EXIST_SHIFT,
        FLAG_ROTATE_W_NOT_EXIST     = 0x1 << FLAG_ROTATE_W_NOT_EXIST_SHIFT,
        FLAG_TRANSLATE_X_NOT_EXIST  = 0x1 << FLAG_TRANSLATE_X_NOT_EXIST_SHIFT,
        FLAG_TRANSLATE_Y_NOT_EXIST  = 0x1 << FLAG_TRANSLATE_Y_NOT_EXIST_SHIFT,
        FLAG_TRANSLATE_Z_NOT_EXIST  = 0x1 << FLAG_TRANSLATE_Z_NOT_EXIST_SHIFT,
        
        FLAG_SCALE_NOT_EXIST        = (FLAG_SCALE_X_NOT_EXIST | FLAG_SCALE_Y_NOT_EXIST | FLAG_SCALE_Z_NOT_EXIST),
        FLAG_ROTATE_NOT_EXIST       = (FLAG_ROTATE_X_NOT_EXIST | FLAG_ROTATE_Y_NOT_EXIST | FLAG_ROTATE_Z_NOT_EXIST),
        FLAG_TRANSLATE_NOT_EXIST    = (FLAG_TRANSLATE_X_NOT_EXIST | FLAG_TRANSLATE_Y_NOT_EXIST | FLAG_TRANSLATE_Z_NOT_EXIST)
    };
    
    union{
        nw::ut::ResF32      constantValueScaleX;
        nw::ut::Offset      toScaleX;
    };
    
    union{
        nw::ut::ResF32      constantValueScaleY;
        nw::ut::Offset      toScaleY;
    };
    
    union{
        nw::ut::ResF32      constantValueScaleZ;
        nw::ut::Offset      toScaleZ;
    };
    
    union{
        nw::ut::ResF32      constantValueRotateX;
        nw::ut::Offset      toRotateX;
    };
    
    union{
        nw::ut::ResF32      constantValueRotateY;
        nw::ut::Offset      toRotateY;
    };
    
    union{
        nw::ut::ResF32      constantValueRotateZ;
        nw::ut::Offset      toRotateZ;
    };
    
    union{
        nw::ut::ResF32      constantValueRotateW;
        nw::ut::Offset      toRotateW;
    };
    
    union{
        nw::ut::ResF32      constantValueTranslateX;
        nw::ut::Offset      toTranslateX;
    };
    
    union{
        nw::ut::ResF32      constantValueTranslateY;
        nw::ut::Offset      toTranslateY;
    };
    
    union{
        nw::ut::ResF32      constantValueTranslateZ;
        nw::ut::Offset      toTranslateZ;
    };
};

struct ResBakedTransformAnimData : public ResMemberAnimData{
    enum Flag{
        FLAG_CONSTANT_TRANSLATE_SHIFT = 0,
        FLAG_CONSTANT_ROTATE_SHIFT,
        FLAG_CONSTANT_SCALE_SHIFT,
        FLAG_TRANSLATE_NOT_EXIST_SHIFT,
        FLAG_ROTATE_NOT_EXIST_SHIFT,
        FLAG_SCALE_NOT_EXIST_SHIFT,

        FLAG_CONSTANT_TRANSLATE  = 0x1 << FLAG_CONSTANT_TRANSLATE_SHIFT,
        FLAG_CONSTANT_ROTATE   = 0x1 << FLAG_CONSTANT_ROTATE_SHIFT,
        FLAG_CONSTANT_SCALE   = 0x1 << FLAG_CONSTANT_SCALE_SHIFT,
        FLAG_TRANSLATE_NOT_EXIST = 0x1 << FLAG_TRANSLATE_NOT_EXIST_SHIFT,
        FLAG_ROTATE_NOT_EXIST = 0x1 << FLAG_ROTATE_NOT_EXIST_SHIFT,
        FLAG_SCALE_NOT_EXIST  = 0x1 << FLAG_SCALE_NOT_EXIST_SHIFT
    };

    nw::ut::Offset      toRotate;
    nw::ut::Offset      toTranslate;
    nw::ut::Offset      toScale;
};

struct ResFullBakedAnimData : public ResMemberAnimData{
    nw::ut::Offset      toTransform;
};

class ResMemberAnim : public nw::ut::ResCommon< ResMemberAnimData >{
public:
    enum PrimitiveType{
        PRIMITIVETYPE_FLOAT,
        PRIMITIVETYPE_INT,
        PRIMITIVETYPE_BOOL,
        PRIMITIVETYPE_VECTOR2,
        PRIMITIVETYPE_VECTOR3,
        PRIMITIVETYPE_TRANSFORM,
        PRIMITIVETYPE_RGBA_COLOR,
        PRIMITIVETYPE_TEXTURE,
        PRIMITIVETYPE_BAKED_TRANSFORM,
        PRIMITIVETYPE_FULL_BAKED
    };
    
    NW_RES_CTOR( ResMemberAnim )
    
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )

    NW_RES_FIELD_STRING_DECL( Path )

    NW_RES_FIELD_PRIMITIVE_DECL( u32, PrimitiveType )

    gfx::Result Setup(nw::os::IAllocator* allocator, gfx::ResGraphicsFile graphicsFile);

    void Cleanup();

    int GetPrimitiveSize() const;

    bit32 EvaluateResultForType(void* dstBuf,bit32 dstFlags,float frame,const void* originalValue) const;

    void ApplyCacheForType(void* target,const void* cacheBuf) const;
};

typedef nw::ut::ResArrayClass<ResMemberAnim>::type  ResMemberAnimArray;

class ResAnim : public nw::ut::ResCommon< ResAnimData >{
public:
    enum { BINARY_REVISION = REVISION_RES_ANIM_DATA };
    enum { TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResAnim) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('ADAT') };

    NW_RES_CTOR( ResAnim )

    NW_RES_FIELD_STRUCT_DECL( nw::ut::BinaryRevisionHeader, Header )

    u32 GetRevision() const { return this->GetHeader().revision; }

    NW_RES_FIELD_STRING_DECL( Name )

    NW_RES_FIELD_STRING_DECL( TargetAnimGroupName )

    NW_RES_FIELD_PRIMITIVE_DECL( u8, LoopMode )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, FrameSize )

    NW_RES_FIELD_CLASS_DIC_DECL( ResMemberAnim, MemberAnimSet, nw::ut::ResDicPatricia )

    NW_RES_FIELD_CLASS_DIC_DECL( nw::ut::ResMetaData, UserData, nw::ut::ResDicPatricia )

    gfx::Result Setup(nw::os::IAllocator* allocator, gfx::ResGraphicsFile graphicsFile);

    void Cleanup();

    static ResAnim CreateEmptySkeletalAnim(nw::os::IAllocator* allocator){
        return CreateEmptyResAnim(allocator, "SkeletalAnimation");
    }

    static ResAnim CreateEmptyMaterialAnim(nw::os::IAllocator* allocator){
        return CreateEmptyResAnim(allocator, "MaterialAnimation");
    }

    static ResAnim CreateEmptyVisibilityAnim(nw::os::IAllocator* allocator){
        return CreateEmptyResAnim(allocator, "VisibilityAnimation");
    }

    static ResAnim CreateEmptyCameraAnim(nw::os::IAllocator* allocator){
        return CreateEmptyResAnim(allocator, "CameraAnimation");
    }

    static ResAnim CreateEmptyLightAnim(nw::os::IAllocator* allocator){
        return CreateEmptyResAnim(allocator, "LightAnimation");
    }

    bool IsFullBakedAnim() const;

private:
    static ResAnim CreateEmptyResAnim(nw::os::IAllocator* allocator, const char* targetAnimGroupName);
};

class ResLightAnim : public ResAnim{
public:
    enum { TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResLightAnim) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('ADLT') };

    NW_RES_CTOR_INHERIT( ResLightAnim, ResAnim )

    NW_RES_FIELD_PRIMITIVE_DECL( u32, LightKind )
    NW_RES_FIELD_PRIMITIVE_DECL( u32, LightType )
};

class ResCameraAnim : public ResAnim{
public:
    enum { TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResCameraAnim) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('ADCM') };

    NW_RES_CTOR_INHERIT( ResCameraAnim, ResAnim )
    
    NW_RES_FIELD_PRIMITIVE_DECL( u32, ProjectionUpdaterKind )
    NW_RES_FIELD_PRIMITIVE_DECL( u32, ViewUpdaterKind )
};

#define NW_RESANM_GETCURVE_DECL( type, name, hasNoCurveFlag )                   \
    const type* Get##name() const                                               \
    {                                                                           \
        if ( ref().mFlags & (hasNoCurveFlag) )                                 \
        {                                                                       \
            return NULL;                                                        \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            return reinterpret_cast<const type*>( ref().to##name.to_ptr() );    \
        }                                                                       \
    }                                                                           \
                                                                                \
    type* Get##name()                                                           \
    {                                                                           \
        if ( ref().mFlags & (hasNoCurveFlag) )                                 \
        {                                                                       \
            return NULL;                                                        \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            return reinterpret_cast<type*>( ref().to##name.to_ptr() );          \
        }                                                                       \
    }                                                                           \


#define NW_RESANM_EVALUATE_FLOAT_DECL(name, constantFlag)                       \
    f32 Evaluate##name( f32 frame ) const                                       \
    {                                                                           \
        if ( ref().mFlags & (constantFlag) )                                   \
        {                                                                       \
            return ref().constantValue##name;                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            return CalcFloatCurve(                                              \
                    reinterpret_cast<const ResFloatCurveData*>( ref().to##name.to_ptr() ), \
                    frame );                                                    \
        }                                                                       \
    }                                                                           \


#define NW_RESANM_EVALUATE_INT_DECL(name, constantFlag)                         \
    s32 Evaluate##name( f32 frame ) const                                       \
    {                                                                           \
        if ( ref().mFlags & (constantFlag) )                                   \
        {                                                                       \
            return ref().constantValue##name;                                   \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            return CalcIntCurve(                                                \
                    reinterpret_cast<const ResIntCurveData*>( ref().to##name.to_ptr() ), \
                    frame );                                                    \
        }                                                                       \
    }                                                                           \


#define NW_RESANM_EVALUATE_VEC3_DECL(name, constantFlag)                        \
    void Evaluate##name( math::VEC3* result, bit32* flags, f32 frame ) const    \
    {                                                                           \
        if ( ref().mFlags & (constantFlag) )                                   \
        {                                                                       \
            CalcVector3Curve(                                                     \
                result,                                                         \
                flags,                                                          \
                reinterpret_cast<const ResVector3CurveData*>( ref().to##name.to_ptr() ), \
                0 );                                                            \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            CalcVector3Curve(                                                     \
                result,                                                         \
                flags,                                                          \
                reinterpret_cast<const ResVector3CurveData*>( ref().to##name.to_ptr() ), \
                frame );                                                        \
        }                                                                       \
    }                                                                           \


class ResFloatAnim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResFloatAnim, ResMemberAnim )
    
    /* ctor */ ResFloatAnim( ResMemberAnim res )
     : ResMemberAnim( res )
    {
        NW_ASSERT( (!res.IsValid()) || 
                    res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_FLOAT );
    }
    
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, Curve, ResFloatAnimData::FLAG_CONSTANT )

    f32 Evaluate( f32 frame ) const{
        if ( ref().mFlags & ResFloatAnimData::FLAG_CONSTANT ){
            return ref().constantValue;
        }
        else{
            return CalcFloatCurve(reinterpret_cast<const ResFloatCurveData*>( ref().toCurve.to_ptr() ),frame );
        }
    }

    bit32 EvaluateResult(void* dstBuf,bit32 dstFlags,float frame) const{
        *reinterpret_cast<float*>(dstBuf) = Evaluate(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT0;
        return dstFlags;
    }

    void ApplyCache(void* target,const void* cacheBuf) const{
        *reinterpret_cast<float*>(target) = *reinterpret_cast<const float*>(cacheBuf);
    }
};

class ResIntAnim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResIntAnim, ResMemberAnim )
    
    ResIntAnim( ResMemberAnim res ): 
    ResMemberAnim( res ){
        NW_ASSERT( (!res.IsValid()) || res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_INT );
    }
    
    NW_RESANM_GETCURVE_DECL( ResIntCurveData, Curve, ResIntAnimData::FLAG_CONSTANT )

    s32 Evaluate( f32 frame ) const{
        if ( ref().mFlags & ResIntAnimData::FLAG_CONSTANT ){
            return ref().constantValue;
        }
        else{
            return CalcIntCurve(reinterpret_cast<const ResIntCurveData*>( ref().toCurve.to_ptr() ),frame );
        }
    }

    bit32 EvaluateResult(void* dstBuf,bit32 dstFlags,float frame) const{
        *reinterpret_cast<s32*>(dstBuf) = Evaluate(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT0;
        return dstFlags;
    }

    void ApplyCache(void* target,const void* cacheBuf) const{
        *reinterpret_cast<s32*>(target) = *reinterpret_cast<const s32*>(cacheBuf);
    }
};

class ResBoolAnim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResBoolAnim, ResMemberAnim )

    ResBoolAnim( ResMemberAnim res ): 
    ResMemberAnim( res ){
        NW_ASSERT( (!res.IsValid()) || res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_BOOL );
    }
    
    NW_RESANM_GETCURVE_DECL( ResBoolCurveData, Curve, ResBoolAnimData::FLAG_CONSTANT )

    bool Evaluate( f32 frame ) const{
        if ( ref().mFlags & ResBoolAnimData::FLAG_CONSTANT ){
            return (ref().mFlags & ResBoolAnimData::FLAG_CONSTANT_VALUE)? true : false;
        }
        else{
            return CalcBoolCurve(reinterpret_cast<const ResBoolCurveData*>( ref().toCurve.to_ptr() ),frame );
        }
    }

    bit32 EvaluateResult(void* dstBuf,bit32 dstFlags,float frame) const{
        *reinterpret_cast<bool*>(dstBuf) = Evaluate(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT0;
        return dstFlags;
    }

    void ApplyCache(void* target,const void* cacheBuf) const{
        *reinterpret_cast<bool*>(target) = *reinterpret_cast<const bool*>(cacheBuf);
    }
};

class ResVec2Anim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResVec2Anim, ResMemberAnim )

    ResVec2Anim( ResMemberAnim res ): 
    ResMemberAnim( res ){
        NW_ASSERT( (!res.IsValid()) || res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_VECTOR2 );
    }

    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, CurveX, ResVec2AnimData::FLAG_CONSTANT_X )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, CurveY, ResVec2AnimData::FLAG_CONSTANT_Y )

    NW_RESANM_EVALUATE_FLOAT_DECL( CurveX, ResVec2AnimData::FLAG_CONSTANT_X )
    NW_RESANM_EVALUATE_FLOAT_DECL( CurveY, ResVec2AnimData::FLAG_CONSTANT_Y )

    bit32 EvaluateResult(void* dstBuf,bit32 dstFlags,float frame) const;

    void ApplyCache(void* target,const void* cacheBuf) const;
};

class ResVec3Anim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResVec3Anim, ResMemberAnim )

    ResVec3Anim( ResMemberAnim res ): 
    ResMemberAnim( res ){
        NW_ASSERT( (!res.IsValid()) || res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_VECTOR3 );
    }

    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, CurveX, ResVec3AnimData::FLAG_CONSTANT_X )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, CurveY, ResVec3AnimData::FLAG_CONSTANT_Y )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, CurveZ, ResVec3AnimData::FLAG_CONSTANT_Z )

    NW_RESANM_EVALUATE_FLOAT_DECL( CurveX, ResVec3AnimData::FLAG_CONSTANT_X )
    NW_RESANM_EVALUATE_FLOAT_DECL( CurveY, ResVec3AnimData::FLAG_CONSTANT_Y )
    NW_RESANM_EVALUATE_FLOAT_DECL( CurveZ, ResVec3AnimData::FLAG_CONSTANT_Z )

    bit32 EvaluateResult(void* dstBuf,bit32 dstFlags,float frame) const;

    void ApplyCache(void* target,const void* cacheBuf) const;
};

class ResRgbaColorAnim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResRgbaColorAnim, ResMemberAnim )

    ResRgbaColorAnim( ResMemberAnim res ): 
    ResMemberAnim( res ){
        NW_ASSERT( (!res.IsValid()) || res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR );
    }

    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, CurveR, ResRgbaColorAnimData::FLAG_CONSTANT_R )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, CurveG, ResRgbaColorAnimData::FLAG_CONSTANT_G )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, CurveB, ResRgbaColorAnimData::FLAG_CONSTANT_B )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, CurveA, ResRgbaColorAnimData::FLAG_CONSTANT_A )

    NW_RESANM_EVALUATE_FLOAT_DECL( CurveR, ResRgbaColorAnimData::FLAG_CONSTANT_R )
    NW_RESANM_EVALUATE_FLOAT_DECL( CurveG, ResRgbaColorAnimData::FLAG_CONSTANT_G )
    NW_RESANM_EVALUATE_FLOAT_DECL( CurveB, ResRgbaColorAnimData::FLAG_CONSTANT_B )
    NW_RESANM_EVALUATE_FLOAT_DECL( CurveA, ResRgbaColorAnimData::FLAG_CONSTANT_A )

    bit32 EvaluateResult(void* dstBuf,bit32 dstFlags,float frame) const;

    void ApplyCache(void* target,const void* cacheBuf) const;
};

class ResTextureAnim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResTextureAnim, ResMemberAnim )

    ResTextureAnim( ResMemberAnim res ): 
    ResMemberAnim( res ){
        NW_ASSERT( (!res.IsValid()) || res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_TEXTURE );
    }

    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, Curve, ResTextureAnimData::FLAG_CONSTANT )
    NW_RESANM_EVALUATE_FLOAT_DECL( Curve, ResTextureAnimData::FLAG_CONSTANT )

    NW_RES_FIELD_CLASS_LIST_DECL( gfx::ResReferenceTexture, Textures )

    gfx::Result Setup(nw::os::IAllocator* allocator, gfx::ResGraphicsFile graphicsFile);

    void Cleanup();

    bit32 EvaluateResult(void* dstBuf,bit32 dstFlags,float frame) const;

    void ApplyCache(void* target,const void* cacheBuf) const;
};

class ResTransformAnim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResTransformAnim, ResMemberAnim )

    ResTransformAnim( ResMemberAnim res ): 
    ResMemberAnim( res ){
        NW_ASSERT( (!res.IsValid()) || res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_TRANSFORM );
    }
    
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, ScaleX, ResTransformAnimData::FLAG_CONSTANT_SCALE_X | ResTransformAnimData::FLAG_SCALE_X_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, ScaleY, ResTransformAnimData::FLAG_CONSTANT_SCALE_Y | ResTransformAnimData::FLAG_SCALE_Y_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, ScaleZ, ResTransformAnimData::FLAG_CONSTANT_SCALE_Z | ResTransformAnimData::FLAG_SCALE_Z_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, RotateX, ResTransformAnimData::FLAG_CONSTANT_ROTATE_X | ResTransformAnimData::FLAG_ROTATE_X_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, RotateY, ResTransformAnimData::FLAG_CONSTANT_ROTATE_Y | ResTransformAnimData::FLAG_ROTATE_Y_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, RotateZ, ResTransformAnimData::FLAG_CONSTANT_ROTATE_Z | ResTransformAnimData::FLAG_ROTATE_Z_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, TranslateX, ResTransformAnimData::FLAG_CONSTANT_TRANSLATE_X | ResTransformAnimData::FLAG_TRANSLATE_X_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, TranslateY, ResTransformAnimData::FLAG_CONSTANT_TRANSLATE_Y | ResTransformAnimData::FLAG_TRANSLATE_Y_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResFloatCurveData, TranslateZ, ResTransformAnimData::FLAG_CONSTANT_TRANSLATE_Z | ResTransformAnimData::FLAG_TRANSLATE_Z_NOT_EXIST )
    
    NW_RESANM_EVALUATE_FLOAT_DECL( ScaleX, ResTransformAnimData::FLAG_CONSTANT_SCALE_X )
    NW_RESANM_EVALUATE_FLOAT_DECL( ScaleY, ResTransformAnimData::FLAG_CONSTANT_SCALE_Y )
    NW_RESANM_EVALUATE_FLOAT_DECL( ScaleZ, ResTransformAnimData::FLAG_CONSTANT_SCALE_Z )

    NW_RESANM_EVALUATE_FLOAT_DECL( RotateX, ResTransformAnimData::FLAG_CONSTANT_ROTATE_X )
    NW_RESANM_EVALUATE_FLOAT_DECL( RotateY, ResTransformAnimData::FLAG_CONSTANT_ROTATE_Y )
    NW_RESANM_EVALUATE_FLOAT_DECL( RotateZ, ResTransformAnimData::FLAG_CONSTANT_ROTATE_Z )

    NW_RESANM_EVALUATE_FLOAT_DECL( TranslateX, ResTransformAnimData::FLAG_CONSTANT_TRANSLATE_X )
    NW_RESANM_EVALUATE_FLOAT_DECL( TranslateY, ResTransformAnimData::FLAG_CONSTANT_TRANSLATE_Y )
    NW_RESANM_EVALUATE_FLOAT_DECL( TranslateZ, ResTransformAnimData::FLAG_CONSTANT_TRANSLATE_Z )

    bit32 EvaluateResult(void* dstBuf,bit32 dstFlags,float frame,const void* originalTransform) const;
    void ApplyCache(void* target,const void* cacheBuf) const;
};

class ResBakedTransformAnim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResBakedTransformAnim, ResMemberAnim )

    ResBakedTransformAnim( ResMemberAnim res ): 
    ResMemberAnim( res ){
        NW_ASSERT( (!res.IsValid()) || res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM );
    }
    
    NW_RESANM_GETCURVE_DECL( ResVector4CurveData, Rotate, ResBakedTransformAnimData::FLAG_CONSTANT_ROTATE | ResBakedTransformAnimData::FLAG_ROTATE_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResVector3CurveData, Translate, ResBakedTransformAnimData::FLAG_CONSTANT_TRANSLATE | ResBakedTransformAnimData::FLAG_TRANSLATE_NOT_EXIST )
    NW_RESANM_GETCURVE_DECL( ResVector3CurveData, Scale, ResBakedTransformAnimData::FLAG_CONSTANT_SCALE | ResBakedTransformAnimData::FLAG_SCALE_NOT_EXIST )
    
    NW_RESANM_EVALUATE_VEC3_DECL( Scale, ResBakedTransformAnimData::FLAG_CONSTANT_SCALE )

    void EvaluateRotate( math::MTX34* result, bit32* flags, f32 frame ) const{
        float f =
            ( ref().mFlags & ResBakedTransformAnimData::FLAG_CONSTANT_ROTATE ) ? 0 : frame;
        
        CalcRotateCurve( result, flags, reinterpret_cast<const ResVector4CurveData*>( ref().toRotate.to_ptr() ), f );
    }
    void EvaluateTranslate( math::MTX34* result, bit32* flags, f32 frame ) const{
        float f = (ref().mFlags & ResBakedTransformAnimData::FLAG_CONSTANT_TRANSLATE ) ? 0 : frame;

        CalcTranslateCurve(result, flags, reinterpret_cast<const ResVector3CurveData*>( ref().toTranslate.to_ptr() ), f );
    }

    static void ApplyBakedFlags(gfx::CalculatedTransform* transform, const bit32 flags);
};

class ResFullBakedAnim : public ResMemberAnim{
public:
    NW_RES_CTOR_INHERIT( ResFullBakedAnim, ResMemberAnim )

    ResFullBakedAnim( ResMemberAnim res ): 
        ResMemberAnim( res ){
        NW_ASSERT((!res.IsValid()) || 
                    res.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_FULL_BAKED );
    }

    const ResFullBakedCurveData* GetTransform() const{
        return reinterpret_cast<const ResFullBakedCurveData*>( ref().toTransform.to_ptr() );
    }

    ResFullBakedCurveData* GetTransform(){
        return reinterpret_cast<ResFullBakedCurveData*>( ref().toTransform.to_ptr() );
    }

    void EvaluateTransform( math::MTX34* result, f32 frame ) const{
        CalcTransformCurve( result, GetTransform(), frame );
    }
};

}
}
}