/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_GFX_RESSHAPE_H_
#define NW_GFX_RESSHAPE_H_

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

//===================================
// Blend shape class
//===================================
//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResTargetShapeData
{
    nw::ut::ResTypeInfo typeInfo;                   //
    nw::ut::ResS32 m_VertexAttributesTableCount;    //
    nw::ut::Offset toVertexAttributesTable;         //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResBlendShapeData
{
    nw::ut::ResTypeInfo typeInfo;               //
    nw::ut::ResS32 m_TargetShapesTableCount;    //
    nw::ut::Offset toTargetShapesTable;         //
    nw::ut::ResS32 m_UsageTableTableCount;      //
    nw::ut::Offset toUsageTableTable;           //
};

//--------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
class ResTargetShape : public nw::ut::ResCommon<ResTargetShapeData>
{
public:
    NW_RES_CTOR( ResTargetShape )

    NW_RES_FIELD_CLASS_LIST_DECL( ResVertexAttribute, VertexAttributes )
};

//--------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
class ResBlendShape : public nw::ut::ResCommon<ResBlendShapeData>
{
public:
    NW_RES_CTOR( ResBlendShape )

    NW_RES_FIELD_CLASS_LIST_DECL( ResTargetShape, TargetShapes )
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u32, UsageTable )
};

//===================================
//  Primitive class
//===================================
//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResPrimitiveData
{
    nw::ut::ResS32 m_IndexStreamsTableCount;    //
    nw::ut::Offset toIndexStreamsTable;         //
    nw::ut::ResS32 m_BufferObjectsTableCount;   //
    nw::ut::Offset toBufferObjectsTable;        //
    nw::ut::ResU32 m_Flags;                     //
    nw::os::IAllocator* m_CommandAllocator;     //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResPrimitive : public nw::ut::ResCommon< ResPrimitiveData >
{
public:
    //--------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_COMMAND_HAS_BEEN_SETUP    = 0x1 << 0 //
    };

    NW_RES_CTOR( ResPrimitive )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResIndexStream, IndexStreams ) // GetIndexStreams(int idx), GetIndexStreamsCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u32, BufferObjects )       // GetBufferObjects(), GetBufferObjects(int idx), GetBufferObjectsCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )                   // GetFlags(), SetFlags(), EnableFlags(), DisableFlags()

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void    Setup(nw::os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void    Cleanup();

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void    SetupDrawCommand(bool hasGeometryShader);
};

//
typedef nw::ut::ResArrayClass<ResPrimitive>::type ResPrimitiveArray;


//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResPrimitiveSetData
{
    nw::ut::ResS32 m_BoneIndexTableTableCount;  //
    nw::ut::Offset toBoneIndexTableTable;       //
    nw::ut::ResS32 m_SkinningMode;              //
    nw::ut::ResS32 m_PrimitivesTableCount;      //
    nw::ut::Offset toPrimitivesTable;           //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResPrimitiveSet : public nw::ut::ResCommon< ResPrimitiveSetData >
{
public:
    //
    enum SkinningModeType
    {
        SKINNING_MODE_NONE,     //
        SKINNING_MODE_RIGID,    //
        SKINNING_MODE_SMOOTH    //
    };

    NW_RES_CTOR( ResPrimitiveSet )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( s32, BoneIndexTable )  // GetBoneIndexTable(), GetBoneIndexTable(int idx), GetBoneIndexTableCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, SkinningMode )         // GetSkinningMode(), SetSkinningMode()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResPrimitive, Primitives ) // GetPrimitives(int idx), GetPrimitivesCount()

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void    Setup(nw::os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void    Cleanup();
};

//
typedef nw::ut::ResArrayClass<ResPrimitiveSet>::type  ResPrimitiveSetArray;


//===================================
//  Shape class
//===================================
//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResShapeData : public ResSceneObjectData
{
    nw::ut::ResU32 m_Flags;                     //
    nw::ut::Offset toBoundingVolume;            //
    nw::ut::ResVec3 m_PositionOffset;           //
    nw::ut::ResS32 m_PrimitiveSetsTableCount;   //
    nw::ut::Offset toPrimitiveSetsTable;        //
    u32            m_BaseAddress;               //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResShape : public ResSceneObject
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResShape) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SHOB') };

    //--------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_HAS_BEEN_SETUP      = 0x1 << 0  //
    };

    NW_RES_CTOR_INHERIT( ResShape, ResSceneObject )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )               // GetFlags(), SetFlags(), EnableFlags(), DisableFlags()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( nw::ut::ResBoundingVolume, BoundingVolume) // GetBoundingVolume()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    nw::ut::ResOrientedBoundingBox GetOrientedBoundingBox()
    {
        nw::ut::ResOrientedBoundingBox resOBB( ref().toBoundingVolume.to_ptr() );

        NW_ASSERT( resOBB.IsValid() || (resOBB.GetTypeInfo() == nw::ut::ResOrientedBoundingBox::TYPE_INFO) );
        return resOBB;
    }

    const nw::ut::ResOrientedBoundingBox GetOrientedBoundingBox() const
    {
        const nw::ut::ResOrientedBoundingBox resOBB( ref().toBoundingVolume.to_ptr() );

        NW_ASSERT( resOBB.IsValid() || (resOBB.GetTypeInfo() == nw::ut::ResOrientedBoundingBox::TYPE_INFO) );
        return resOBB;
    }

    //
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    nw::ut::ResOrientedBoundingBoxData& GetOrientedBoundingBoxData()
    {
        return this->GetOrientedBoundingBox().ref();
    }

    //
    const nw::ut::ResOrientedBoundingBoxData& GetOrientedBoundingBoxData() const
    {
        return this->GetOrientedBoundingBox().ref();
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, PositionOffset )     // GetPositionOffset()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResPrimitiveSet, PrimitiveSets ) // GetPrimitiveSets(int idx), GetPrimitiveSetsCount()

    //--------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const nw::math::VEC3& GetCenterPosition() const { return this->GetOrientedBoundingBox().GetCenterPosition(); }

    //--------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetCenterPosition( f32 x, f32 y, f32 z ) { return this->GetOrientedBoundingBox().SetCenterPosition(x, y, z); }

    //--------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetCenterPosition(const nw::math::VEC3& value) { return this->GetOrientedBoundingBox().SetCenterPosition(value); }

    //
    Result Setup(nw::os::IAllocator* allocator);

    //
    void   Cleanup();
};

//
typedef nw::ut::ResArrayClass<ResShape>::type  ResShapeArray;


//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResSeparateDataShapeData : public ResShapeData
{
    nw::ut::ResS32 m_VertexAttributesTableCount;    //
    nw::ut::Offset toVertexAttributesTable;         //
    nw::ut::Offset toBlendShape;                    //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResSeparateDataShape : public ResShape
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSeparateDataShape) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SPSH') };

    NW_RES_CTOR_INHERIT( ResSeparateDataShape, ResShape )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResVertexAttribute, VertexAttributes ) // GetVertexAttributes(int idx), GetVertexAttributesCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //
    NW_RES_FIELD_CLASS_DECL( ResBlendShape, BlendShape )     // GetBlendShape()

    //
    //

    //
    u32 GetVertexCount();
};

} // namespace res

namespace internal {

//--------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class ResVertexAttributeIterator
{
public:
    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static ResVertexAttributeIterator Begin(ResSeparateDataShape shape)
    {
        return ResVertexAttributeIterator(shape);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    /* implicit */ ResVertexAttributeIterator(const ResVertexAttributeIterator& value)
     : m_Shape( value.m_Shape ),
       m_Interleave( value.m_Interleave ),
       m_IndexOnShape( value.m_IndexOnShape ),
       m_IndexOnInterleave( value.m_IndexOnInterleave )
    {
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    bool IsValid() const
    {
        return (m_IndexOnShape >= 0);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    ResVertexAttribute operator*()
    {
        if (m_Interleave.IsValid())
        {
            return m_Interleave.GetVertexStreams( m_IndexOnInterleave );
        }
        else
        {
            return m_Shape.GetVertexAttributes( m_IndexOnShape );
        }
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const ResVertexAttribute operator*() const
    {
        if (m_Interleave.IsValid())
        {
            return m_Interleave.GetVertexStreams( m_IndexOnInterleave );
        }
        else
        {
            return m_Shape.GetVertexAttributes( m_IndexOnShape );
        }
    }


    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    ResVertexAttributeIterator& operator++()
    {
        if (m_Interleave.IsValid())
        {
            ++m_IndexOnInterleave;
            if (m_Interleave.GetVertexStreamsCount() > m_IndexOnInterleave)
            {
                return *this;
            }

            m_Interleave = ResInterleavedVertexStream(NULL);
            m_IndexOnInterleave = -1;
        }

        ++m_IndexOnShape;
        if (m_Shape.GetVertexAttributesCount() > m_IndexOnShape)
        {
            ResVertexAttribute attribute = m_Shape.GetVertexAttributes(m_IndexOnShape);

            if (attribute.GetFlags() & ResVertexAttribute::FLAG_INTERLEAVE)
            {
                m_Interleave = ResStaticCast<ResInterleavedVertexStream>( attribute );
                m_IndexOnInterleave = 0;
            }
        }
        else
        {
            m_IndexOnShape = -1;
        }

        return *this;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    ResVertexAttributeIterator operator++(int)
    {
        ResVertexAttributeIterator copy = *this;

        this->operator++();

        return copy;
    }

private:
    ResSeparateDataShape       m_Shape;
    ResInterleavedVertexStream m_Interleave;
    s32                        m_IndexOnShape;
    s32                        m_IndexOnInterleave;

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------

    explicit ResVertexAttributeIterator(ResSeparateDataShape shape)
     : m_Shape( shape ),
       m_Interleave( ResInterleavedVertexStream(NULL) ),
       m_IndexOnShape( 0 ),
       m_IndexOnInterleave( -1 )
    {
        NW_ASSERT( shape.IsValid() );

        ResVertexAttribute attribute = shape.GetVertexAttributes(0);

        if (attribute.GetFlags() & ResVertexAttribute::FLAG_INTERLEAVE)
        {
            m_Interleave = ResStaticCast<ResInterleavedVertexStream>( attribute );
            m_IndexOnInterleave = 0;
        }
    }
};

} // namespace internal

} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESSHAPE_H_
