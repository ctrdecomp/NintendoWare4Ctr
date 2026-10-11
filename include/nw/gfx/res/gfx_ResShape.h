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

struct ResTargetShapeData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResS32 m_VertexAttributesTableCount;
    nw::ut::Offset toVertexAttributesTable;
};

struct ResBlendShapeData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResS32 m_TargetShapesTableCount;
    nw::ut::Offset toTargetShapesTable;
    nw::ut::ResS32 m_UsageTableTableCount;
    nw::ut::Offset toUsageTableTable;
};

class ResTargetShape : public nw::ut::ResCommon<ResTargetShapeData>
{
public:
    NW_RES_CTOR(ResTargetShape)


    s32 GetVertexAttributesCount() const
        { return ref().m_VertexAttributesTableCount; }
    ResVertexAttribute GetVertexAttributes(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toVertexAttributesTable.to_table_ptr());
            return ResVertexAttribute(tbl_ptr[idx].to_ptr());
        }
    const ResVertexAttribute GetVertexAttributes(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toVertexAttributesTable.to_table_ptr());
            return ResVertexAttribute(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResVertexAttribute, nw::ut::internal::ResArrayClassTraits> GetVertexAttributes()
        {
            return nw::ut::internal::ResArray<ResVertexAttribute, nw::ut::internal::ResArrayClassTraits>(ref().toVertexAttributesTable.to_table_ptr(), GetVertexAttributesCount());
        }
    const nw::ut::internal::ResArray<const ResVertexAttribute, nw::ut::internal::ResArrayClassTraits> GetVertexAttributes() const
        {
            return nw::ut::internal::ResArray<const ResVertexAttribute, nw::ut::internal::ResArrayClassTraits>(ref().toVertexAttributesTable.to_table_ptr(), GetVertexAttributesCount());
        }
};

class ResBlendShape : public nw::ut::ResCommon<ResBlendShapeData>
{
public:
    NW_RES_CTOR(ResBlendShape)


    s32 GetTargetShapesCount() const
        { return ref().m_TargetShapesTableCount; }
    ResTargetShape GetTargetShapes(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toTargetShapesTable.to_table_ptr());
            return ResTargetShape(tbl_ptr[idx].to_ptr());
        }
    const ResTargetShape GetTargetShapes(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toTargetShapesTable.to_table_ptr());
            return ResTargetShape(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResTargetShape, nw::ut::internal::ResArrayClassTraits> GetTargetShapes()
        {
            return nw::ut::internal::ResArray<ResTargetShape, nw::ut::internal::ResArrayClassTraits>(ref().toTargetShapesTable.to_table_ptr(), GetTargetShapesCount());
        }
    const nw::ut::internal::ResArray<const ResTargetShape, nw::ut::internal::ResArrayClassTraits> GetTargetShapes() const
        {
            return nw::ut::internal::ResArray<const ResTargetShape, nw::ut::internal::ResArrayClassTraits>(ref().toTargetShapesTable.to_table_ptr(), GetTargetShapesCount());
        }

    s32 GetUsageTableCount() const
        { return ref().m_UsageTableTableCount; }
    u32 GetUsageTable(int idx) const
        {
            const u32* tbl_ptr = static_cast<const u32*>(ref().toUsageTableTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetUsageTable(int idx, u32 value)
        {
            u32* tbl_ptr = GetUsageTable();
            tbl_ptr[idx] = value;
        }
    u32* GetUsageTable()
        { return static_cast<u32*>(ref().toUsageTableTable.to_table_ptr()); }
    const u32* GetUsageTable() const
        { return static_cast<const u32*>(ref().toUsageTableTable.to_table_ptr()); }
};

struct ResPrimitiveData
{
    nw::ut::ResS32 m_IndexStreamsTableCount;
    nw::ut::Offset toIndexStreamsTable;
    nw::ut::ResS32 m_BufferObjectsTableCount;
    nw::ut::Offset toBufferObjectsTable;
    nw::ut::ResU32 m_Flags;
    nw::os::IAllocator* m_CommandAllocator;
};

class ResPrimitive : public nw::ut::ResCommon< ResPrimitiveData >
{
public:
    enum Flag
    {
        FLAG_COMMAND_HAS_BEEN_SETUP    = 0x1 << 0
    };

    NW_RES_CTOR( ResPrimitive )


    s32 GetIndexStreamsCount() const
        { return ref().m_IndexStreamsTableCount; }
    ResIndexStream GetIndexStreams(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toIndexStreamsTable.to_table_ptr());
            return ResIndexStream(tbl_ptr[idx].to_ptr());
        }
    const ResIndexStream GetIndexStreams(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toIndexStreamsTable.to_table_ptr());
            return ResIndexStream(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResIndexStream, nw::ut::internal::ResArrayClassTraits> GetIndexStreams()
        {
            return nw::ut::internal::ResArray<ResIndexStream, nw::ut::internal::ResArrayClassTraits>(ref().toIndexStreamsTable.to_table_ptr(), GetIndexStreamsCount());
        }
    const nw::ut::internal::ResArray<const ResIndexStream, nw::ut::internal::ResArrayClassTraits> GetIndexStreams() const
        {
            return nw::ut::internal::ResArray<const ResIndexStream, nw::ut::internal::ResArrayClassTraits>(ref().toIndexStreamsTable.to_table_ptr(), GetIndexStreamsCount());
        }

    s32 GetBufferObjectsCount() const
        { return ref().m_BufferObjectsTableCount; }
    u32 GetBufferObjects(int idx) const
        {
            const u32* tbl_ptr = static_cast<const u32*>(ref().toBufferObjectsTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetBufferObjects(int idx, u32 value)
        {
            u32* tbl_ptr = GetBufferObjects();
            tbl_ptr[idx] = value;
        }
    u32* GetBufferObjects()
        { return static_cast<u32*>(ref().toBufferObjectsTable.to_table_ptr()); }
    const u32* GetBufferObjects() const
        { return static_cast<const u32*>(ref().toBufferObjectsTable.to_table_ptr()); }

    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }

    void    Setup(nw::os::IAllocator* allocator);

    void    Cleanup();

    void    SetupDrawCommand(bool hasGeometryShader);
};
typedef nw::ut::ResArrayClass<ResPrimitive>::type ResPrimitiveArray;

struct ResPrimitiveSetData
{
    nw::ut::ResS32 m_BoneIndexTableTableCount;
    nw::ut::Offset toBoneIndexTableTable;
    nw::ut::ResS32 m_SkinningMode;
    nw::ut::ResS32 m_PrimitivesTableCount;
    nw::ut::Offset toPrimitivesTable;
};

class ResPrimitiveSet : public nw::ut::ResCommon< ResPrimitiveSetData >
{
public:
    enum SkinningModeType
    {
        SKINNING_MODE_NONE,
        SKINNING_MODE_RIGID,
        SKINNING_MODE_SMOOTH
    };
    
    NW_RES_CTOR( ResPrimitiveSet )


    s32 GetBoneIndexTableCount() const
        { return ref().m_BoneIndexTableTableCount; }
    s32 GetBoneIndexTable(int idx) const
        {
            const s32* tbl_ptr = static_cast<const s32*>(ref().toBoneIndexTableTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetBoneIndexTable(int idx, s32 value)
        {
            s32* tbl_ptr = GetBoneIndexTable();
            tbl_ptr[idx] = value;
        }
    s32* GetBoneIndexTable()
        { return static_cast<s32*>(ref().toBoneIndexTableTable.to_table_ptr()); }
    const s32* GetBoneIndexTable() const
        { return static_cast<const s32*>(ref().toBoneIndexTableTable.to_table_ptr()); }

    s32 GetSkinningMode() const { return static_cast<s32>(ref().m_SkinningMode); }
    void SetSkinningMode(s32 value) { ref().m_SkinningMode = value; }

    s32 GetPrimitivesCount() const
        { return ref().m_PrimitivesTableCount; }
    ResPrimitive GetPrimitives(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toPrimitivesTable.to_table_ptr());
            return ResPrimitive(tbl_ptr[idx].to_ptr());
        }
    const ResPrimitive GetPrimitives(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toPrimitivesTable.to_table_ptr());
            return ResPrimitive(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResPrimitive, nw::ut::internal::ResArrayClassTraits> GetPrimitives()
        {
            return nw::ut::internal::ResArray<ResPrimitive, nw::ut::internal::ResArrayClassTraits>(ref().toPrimitivesTable.to_table_ptr(), GetPrimitivesCount());
        }
    const nw::ut::internal::ResArray<const ResPrimitive, nw::ut::internal::ResArrayClassTraits> GetPrimitives() const
        {
            return nw::ut::internal::ResArray<const ResPrimitive, nw::ut::internal::ResArrayClassTraits>(ref().toPrimitivesTable.to_table_ptr(), GetPrimitivesCount());
        }

    void    Setup(nw::os::IAllocator* allocator);

    void    Cleanup();
};
typedef nw::ut::ResArrayClass<ResPrimitiveSet>::type  ResPrimitiveSetArray;

struct ResShapeData : public ResSceneObjectData
{
    nw::ut::ResU32 m_Flags;
    nw::ut::Offset toBoundingVolume;
    nw::ut::ResVec3 m_PositionOffset;
    nw::ut::ResS32 m_PrimitiveSetsTableCount;
    nw::ut::Offset toPrimitiveSetsTable;
    u32            m_BaseAddress;
};

class ResShape : public ResSceneObject
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResShape) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('SHOB') };
    
    enum Flag
    {
        FLAG_HAS_BEEN_SETUP      = 0x1 << 0
    };
    
    NW_RES_CTOR_INHERIT( ResShape, ResSceneObject )

    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }

    nw::ut::ResBoundingVolume GetBoundingVolume() { return nw::ut::ResBoundingVolume(ref().toBoundingVolume.to_ptr()); }
    const nw::ut::ResBoundingVolume GetBoundingVolume() const { return nw::ut::ResBoundingVolume(ref().toBoundingVolume.to_ptr()); }

    nw::ut::ResOrientedBoundingBox GetOrientedBoundingBox() 
    {
        nw::ut::ResOrientedBoundingBox resOBB( ref().toBoundingVolume.to_ptr() );
        
        NW_ASSERT(resOBB.IsValid() || (resOBB.GetTypeInfo() == nw::ut::ResOrientedBoundingBox::TYPE_INFO));
        return resOBB;
    }

    const nw::ut::ResOrientedBoundingBox GetOrientedBoundingBox() const
    { 
        const nw::ut::ResOrientedBoundingBox resOBB( ref().toBoundingVolume.to_ptr() );

        NW_ASSERT(resOBB.IsValid() || (resOBB.GetTypeInfo() == nw::ut::ResOrientedBoundingBox::TYPE_INFO));
        return resOBB;
    }

    nw::ut::ResOrientedBoundingBoxData& GetOrientedBoundingBoxData() {return this->GetOrientedBoundingBox().ref();}
    const nw::ut::ResOrientedBoundingBoxData& GetOrientedBoundingBoxData() const { return this->GetOrientedBoundingBox().ref();}


    void SetPositionOffset(f32 x, f32 y, f32 z) { ref().m_PositionOffset.Set(x, y, z); }
    const nw::math::VEC3& GetPositionOffset() const { return ref().m_PositionOffset; }
    void SetPositionOffset(const nw::math::VEC3& value) { ref().m_PositionOffset = value; }

    s32 GetPrimitiveSetsCount() const
        { return ref().m_PrimitiveSetsTableCount; }
    ResPrimitiveSet GetPrimitiveSets(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toPrimitiveSetsTable.to_table_ptr());
            return ResPrimitiveSet(tbl_ptr[idx].to_ptr());
        }
    const ResPrimitiveSet GetPrimitiveSets(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toPrimitiveSetsTable.to_table_ptr());
            return ResPrimitiveSet(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResPrimitiveSet, nw::ut::internal::ResArrayClassTraits> GetPrimitiveSets()
        {
            return nw::ut::internal::ResArray<ResPrimitiveSet, nw::ut::internal::ResArrayClassTraits>(ref().toPrimitiveSetsTable.to_table_ptr(), GetPrimitiveSetsCount());
        }
    const nw::ut::internal::ResArray<const ResPrimitiveSet, nw::ut::internal::ResArrayClassTraits> GetPrimitiveSets() const
        {
            return nw::ut::internal::ResArray<const ResPrimitiveSet, nw::ut::internal::ResArrayClassTraits>(ref().toPrimitiveSetsTable.to_table_ptr(), GetPrimitiveSetsCount());
        }

    const nw::math::VEC3& GetCenterPosition() const { return this->GetOrientedBoundingBox().GetCenterPosition(); }

    void SetCenterPosition( f32 x, f32 y, f32 z ) { return this->GetOrientedBoundingBox().SetCenterPosition(x, y, z); }
    void SetCenterPosition(const nw::math::VEC3& value) { return this->GetOrientedBoundingBox().SetCenterPosition(value); }

    Result Setup(nw::os::IAllocator* allocator);

    void   Cleanup();
};
typedef nw::ut::ResArrayClass<ResShape>::type  ResShapeArray;

struct ResSeparateDataShapeData : public ResShapeData
{
    nw::ut::ResS32 m_VertexAttributesTableCount;
    nw::ut::Offset toVertexAttributesTable;
    nw::ut::Offset toBlendShape;
};

class ResSeparateDataShape : public ResShape
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSeparateDataShape)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('SPSH')
    };
    
    NW_RES_CTOR_INHERIT( ResSeparateDataShape, ResShape )


    s32 GetVertexAttributesCount() const
        { return ref().m_VertexAttributesTableCount; }
    ResVertexAttribute GetVertexAttributes(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toVertexAttributesTable.to_table_ptr());
            return ResVertexAttribute(tbl_ptr[idx].to_ptr());
        }
    const ResVertexAttribute GetVertexAttributes(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toVertexAttributesTable.to_table_ptr());
            return ResVertexAttribute(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResVertexAttribute, nw::ut::internal::ResArrayClassTraits> GetVertexAttributes()
        {
            return nw::ut::internal::ResArray<ResVertexAttribute, nw::ut::internal::ResArrayClassTraits>(ref().toVertexAttributesTable.to_table_ptr(), GetVertexAttributesCount());
        }
    const nw::ut::internal::ResArray<const ResVertexAttribute, nw::ut::internal::ResArrayClassTraits> GetVertexAttributes() const
        {
            return nw::ut::internal::ResArray<const ResVertexAttribute, nw::ut::internal::ResArrayClassTraits>(ref().toVertexAttributesTable.to_table_ptr(), GetVertexAttributesCount());
        }

    ResBlendShape GetBlendShape() { return ResBlendShape(ref().toBlendShape.to_ptr()); }
    const ResBlendShape GetBlendShape() const { return ResBlendShape(ref().toBlendShape.to_ptr()); }

    u32 GetVertexCount();
};

}

namespace internal {

class ResVertexAttributeIterator
{
public:
    static ResVertexAttributeIterator Begin(res::ResSeparateDataShape shape) {return ResVertexAttributeIterator(shape);}

    ResVertexAttributeIterator(const ResVertexAttributeIterator& value): 
        m_Shape(value.m_Shape),
        m_Interleave(value.m_Interleave),
        m_IndexOnShape(value.m_IndexOnShape),
        m_IndexOnInterleave(value.m_IndexOnInterleave) {}

    bool IsValid() const
    {
        return (m_IndexOnShape >= 0);
    }

    res::ResVertexAttribute operator*()
    {
        if (m_Interleave.IsValid())
        {
            return this->m_Interleave.GetVertexStreams(this->m_IndexOnInterleave);
        }
        else{
            return this->m_Shape.GetVertexAttributes(this->m_IndexOnShape);
        }
    }

    const res::ResVertexAttribute operator*() const
    {
        if (m_Interleave.IsValid())
        {
            return m_Interleave.GetVertexStreams(this->m_IndexOnInterleave);
        }
        else{
            return m_Shape.GetVertexAttributes(this->m_IndexOnShape);
        }
    }
    
    ResVertexAttributeIterator& operator++() 
    {
        if (m_Interleave.IsValid())
        {
            ++m_IndexOnInterleave;
            if (m_Interleave.GetVertexStreamsCount() > m_IndexOnInterleave)
            {
                return *this;
            }
            
            m_Interleave = res::ResInterleavedVertexStream(NULL);
            m_IndexOnInterleave = -1;
        }
        
        ++m_IndexOnShape;
        if (m_Shape.GetVertexAttributesCount() > m_IndexOnShape)
        {
            res::ResVertexAttribute attribute = m_Shape.GetVertexAttributes(m_IndexOnShape);
            
            if (attribute.GetFlags() & res::ResVertexAttribute::FLAG_INTERLEAVE)
            {
                m_Interleave = nw::ut::ResStaticCast<res::ResInterleavedVertexStream>(attribute);
                m_IndexOnInterleave = 0;
            }
        }
        else
        {
            m_IndexOnShape = -1;
        }
        
        return *this;
    }

    ResVertexAttributeIterator operator++(int)
    {
        ResVertexAttributeIterator copy = *this;
        
        this->operator++();
        
        return copy;
    }
    
private:
    res::ResSeparateDataShape       m_Shape;
    res::ResInterleavedVertexStream m_Interleave;
    s32                        m_IndexOnShape;
    s32                        m_IndexOnInterleave;

    explicit ResVertexAttributeIterator(res::ResSeparateDataShape shape): 
        m_Shape(shape),
        m_Interleave(res::ResInterleavedVertexStream(NULL)),
        m_IndexOnShape(0),
        m_IndexOnInterleave( -1)
    {
        res::ResVertexAttribute attribute = shape.GetVertexAttributes(0);
        
        if (attribute.GetFlags() & res::ResVertexAttribute::FLAG_INTERLEAVE)
        {
            m_Interleave = nw::ut::ResStaticCast<res::ResInterleavedVertexStream>(attribute);
            m_IndexOnInterleave = 0;
        }
    }
};

}
}
}

