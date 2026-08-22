#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResVertex.h>
#include <nw/math/math_ResTypes.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/ut/ut_ResPrimitive.h>

namespace nw {
namespace gfx {
namespace res {

struct ResTargetShapeData{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResS32 mVertexAttributesTableCount;
    nw::ut::Offset toVertexAttributesTable;
};

struct ResBlendShapeData{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResS32 mTargetShapesTableCount;
    nw::ut::Offset toTargetShapesTable;
    nw::ut::ResS32 mUsageTableTableCount;
    nw::ut::Offset toUsageTableTable;
};

class ResTargetShape : public nw::ut::ResCommon<ResTargetShapeData>{
public:
    NW_RES_CTOR(ResTargetShape)

    NW_RES_FIELD_CLASS_LIST_DECL(ResVertexAttribute, VertexAttributes)
};

class ResBlendShape : public nw::ut::ResCommon<ResBlendShapeData>{
public:
    NW_RES_CTOR(ResBlendShape)

    NW_RES_FIELD_CLASS_LIST_DECL(ResTargetShape, TargetShapes)
    NW_RES_FIELD_PRIMITIVE_LIST_DECL(u32, UsageTable)
};

struct ResPrimitiveData{
    nw::ut::ResS32 mIndexStreamsTableCount;
    nw::ut::Offset toIndexStreamsTable;
    nw::ut::ResS32 mBufferObjectsTableCount;
    nw::ut::Offset toBufferObjectsTable;
    nw::ut::ResU32 mFlags;
    nw::os::IAllocator* mCommandAllocator;
};

class ResPrimitive : public nw::ut::ResCommon< ResPrimitiveData >{
public:
    enum Flag{
        FLAG_COMMAND_HAS_BEEN_SETUP    = 0x1 << 0
    };

    NW_RES_CTOR( ResPrimitive )

    NW_RES_FIELD_CLASS_LIST_DECL(ResIndexStream, IndexStreams)
    NW_RES_FIELD_PRIMITIVE_LIST_DECL(u32, BufferObjects)

    NW_RES_FIELD_FLAGS_DECL(u32, Flags)

    void    Setup(nw::os::IAllocator* allocator);

    void    Cleanup();

    void    SetupDrawCommand(bool hasGeometryShader);
};
typedef nw::ut::ResArrayClass<ResPrimitive>::type ResPrimitiveArray;

struct ResPrimitiveSetData{
    nw::ut::ResS32 mBoneIndexTableTableCount;
    nw::ut::Offset toBoneIndexTableTable;
    nw::ut::ResS32 mSkinningMode;
    nw::ut::ResS32 mPrimitivesTableCount;
    nw::ut::Offset toPrimitivesTable;
};

class ResPrimitiveSet : public nw::ut::ResCommon< ResPrimitiveSetData >{
public:
    enum SkinningModeType{
        SKINNING_MODE_NONE,
        SKINNING_MODE_RIGID,
        SKINNING_MODE_SMOOTH
    };
    
    NW_RES_CTOR( ResPrimitiveSet )

    NW_RES_FIELD_PRIMITIVE_LIST_DECL(s32, BoneIndexTable)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, SkinningMode)
    NW_RES_FIELD_CLASS_LIST_DECL(ResPrimitive, Primitives)

    void    Setup(nw::os::IAllocator* allocator);

    void    Cleanup();
};
typedef nw::ut::ResArrayClass<ResPrimitiveSet>::type  ResPrimitiveSetArray;

struct ResShapeData : public ResSceneObjectData{
    nw::ut::ResU32 mFlags;
    nw::ut::Offset toBoundingVolume;
    nw::ut::ResVec3 mPositionOffset;
    nw::ut::ResS32 mPrimitiveSetsTableCount;
    nw::ut::Offset toPrimitiveSetsTable;
    u32            mBaseAddress;
};

class ResShape : public ResSceneObject{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResShape) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SHOB') };
    
    enum Flag{
        FLAG_HAS_BEEN_SETUP      = 0x1 << 0
    };
    
    NW_RES_CTOR_INHERIT( ResShape, ResSceneObject )

    NW_RES_FIELD_FLAGS_DECL(u32, Flags)
    NW_RES_FIELD_CLASS_DECL(nw::ut::ResBoundingVolume, BoundingVolume)

    nw::ut::ResOrientedBoundingBox GetOrientedBoundingBox() {
        nw::ut::ResOrientedBoundingBox resOBB( ref().toBoundingVolume.to_ptr() );
        
        NW_ASSERT(resOBB.IsValid() || (resOBB.GetTypeInfo() == nw::ut::ResOrientedBoundingBox::TYPE_INFO));
        return resOBB;
    }

    const nw::ut::ResOrientedBoundingBox GetOrientedBoundingBox() const{ 
        const nw::ut::ResOrientedBoundingBox resOBB( ref().toBoundingVolume.to_ptr() );

        NW_ASSERT(resOBB.IsValid() || (resOBB.GetTypeInfo() == nw::ut::ResOrientedBoundingBox::TYPE_INFO));
        return resOBB;
    }

    nw::ut::ResOrientedBoundingBoxData& GetOrientedBoundingBoxData() {return this->GetOrientedBoundingBox().ref();}
    const nw::ut::ResOrientedBoundingBoxData& GetOrientedBoundingBoxData() const{ return this->GetOrientedBoundingBox().ref();}

    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, PositionOffset)
    NW_RES_FIELD_CLASS_LIST_DECL(ResPrimitiveSet, PrimitiveSets)

    const nw::math::VEC3& GetCenterPosition() const { return this->GetOrientedBoundingBox().GetCenterPosition(); }

    void SetCenterPosition( f32 x, f32 y, f32 z ) { return this->GetOrientedBoundingBox().SetCenterPosition(x, y, z); }
    void SetCenterPosition(const nw::math::VEC3& value) { return this->GetOrientedBoundingBox().SetCenterPosition(value); }

    Result Setup(nw::os::IAllocator* allocator);

    void   Cleanup();
};
typedef nw::ut::ResArrayClass<ResShape>::type  ResShapeArray;

struct ResSeparateDataShapeData : public ResShapeData{
    nw::ut::ResS32 mVertexAttributesTableCount;
    nw::ut::Offset toVertexAttributesTable;
    nw::ut::Offset toBlendShape;
};

class ResSeparateDataShape : public ResShape{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSeparateDataShape) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SPSH') };
    
    NW_RES_CTOR_INHERIT( ResSeparateDataShape, ResShape )

    NW_RES_FIELD_CLASS_LIST_DECL(ResVertexAttribute, VertexAttributes)
    NW_RES_FIELD_CLASS_DECL(ResBlendShape, BlendShape)

    u32 GetVertexCount();
};

}

namespace internal {

class ResVertexAttributeIterator{
public:
    static ResVertexAttributeIterator Begin(res::ResSeparateDataShape shape){return ResVertexAttributeIterator(shape);}

    ResVertexAttributeIterator(const ResVertexAttributeIterator& value): 
        mShape(value.mShape),
        mInterleave(value.mInterleave),
        mIndexOnShape(value.mIndexOnShape),
        mIndexOnInterleave(value.mIndexOnInterleave)
    {}

    bool IsValid() const{
        return (mIndexOnShape >= 0);
    }

    res::ResVertexAttribute operator*(){
        if (mInterleave.IsValid()){
            return this->mInterleave.GetVertexStreams(this->mIndexOnInterleave);
        }
        else{
            return this->mShape.GetVertexAttributes(this->mIndexOnShape);
        }
    }

    const res::ResVertexAttribute operator*() const{
        if (mInterleave.IsValid()){
            return mInterleave.GetVertexStreams(this->mIndexOnInterleave);
        }
        else{
            return mShape.GetVertexAttributes(this->mIndexOnShape);
        }
    }
    
    ResVertexAttributeIterator& operator++() {
        if (mInterleave.IsValid()){
            ++mIndexOnInterleave;
            if (mInterleave.GetVertexStreamsCount() > mIndexOnInterleave){
                return *this;
            }
            
            mInterleave = res::ResInterleavedVertexStream(NULL);
            mIndexOnInterleave = -1;
        }
        
        ++mIndexOnShape;
        if (mShape.GetVertexAttributesCount() > mIndexOnShape){
            res::ResVertexAttribute attribute = mShape.GetVertexAttributes(mIndexOnShape);
            
            if (attribute.GetFlags() & res::ResVertexAttribute::FLAG_INTERLEAVE){
                mInterleave = nw::ut::ResStaticCast<res::ResInterleavedVertexStream>(attribute);
                mIndexOnInterleave = 0;
            }
        }
        else{
            mIndexOnShape = -1;
        }
        
        return *this;
    }

    ResVertexAttributeIterator operator++(int){
        ResVertexAttributeIterator copy = *this;
        
        this->operator++();
        
        return copy;
    }
    
private:
    res::ResSeparateDataShape       mShape;
    res::ResInterleavedVertexStream mInterleave;
    s32                        mIndexOnShape;
    s32                        mIndexOnInterleave;

    explicit ResVertexAttributeIterator(res::ResSeparateDataShape shape): 
        mShape(shape),
        mInterleave(res::ResInterleavedVertexStream(NULL)),
        mIndexOnShape(0),
        mIndexOnInterleave( -1){
        res::ResVertexAttribute attribute = shape.GetVertexAttributes(0);
        
        if(attribute.GetFlags() & res::ResVertexAttribute::FLAG_INTERLEAVE){
            mInterleave = nw::ut::ResStaticCast<res::ResInterleavedVertexStream>(attribute);
            mIndexOnInterleave = 0;
        }
    }
};

}
}
}