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

#ifndef NW_GFX_SORTINGMATERIALIDGENERATOR_H_
#define NW_GFX_SORTINGMATERIALIDGENERATOR_H_

#include <nw/gfx/gfx_IMaterialIdGenerator.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
namespace nw
{
namespace gfx
{

namespace internal
{
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    struct MaterialKeyValue
    {
        u32 uniqueId;
        u32 key;
        u32 subKey;
        Material* material;

    };
}

//---------------------------------------------------------------------------
//
//
//
//
//---------------------------------------------------------------------------
class SortingMaterialIdGenerator : public IMaterialIdGenerator
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SortingMaterialIdGenerator);

    typedef ut::MoveArray<internal::MaterialKeyValue> MaterialKeyValueArray;
public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description
    {
        bool isFixedSizeMemory; //
        int  maxMaterials;      //

        //
        Description()
         : isFixedSizeMemory(true),
           maxMaterials(128)
        {}
    };

    //----------------------------------------
    //
    //


    //
    //
    //
    class Builder
    {
    public:
        //
        //
        //
        //
        //
        Builder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //
        Builder& MaxMaterials(int maxMaterials)
        {
            m_Description.maxMaterials = maxMaterials;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        IMaterialIdGenerator* Create(os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    //
    //
    //
    //
    //
    virtual void Accept(Material* material);

    //
    virtual void Generate();

    //

private:

    //----------------------------------------
    //
    //

    //
    SortingMaterialIdGenerator(
        os::IAllocator* allocator,
        MaterialKeyValueArray materials,
        MaterialKeyValueArray materialsWorkSpace)
        : IMaterialIdGenerator(allocator),
          m_Materials(materials),
          m_MaterialsWorkSpace(materialsWorkSpace)
    {}

    //

    MaterialKeyValueArray m_Materials;
    MaterialKeyValueArray m_MaterialsWorkSpace;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_SORTINGMATERIALIDGENERATOR_H_
