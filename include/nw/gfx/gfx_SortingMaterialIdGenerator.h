#pragma once

#include <nw/gfx/gfx_IMaterialIdGenerator.h>

namespace nw{
namespace gfx{
namespace internal{
    struct MaterialKeyValue
    {
        u32 uniqueId;
        u32 key;
        u32 subKey;
        Material* material;

    };
}

class SortingMaterialIdGenerator : public IMaterialIdGenerator
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SortingMaterialIdGenerator);

    typedef nw::ut::MoveArray<internal::MaterialKeyValue> MaterialKeyValueArray;
public:
    NW_UT_RUNTIME_TYPEINFO;
    
    struct Description
{        
        bool isFixedSizeMemory;
        int  maxMaterials;

        Description(): 
            isFixedSizeMemory(true),
            maxMaterials(128) {}
    };

    class Builder
    {
    public:
        Builder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        Builder& MaxMaterials(int maxMaterials)
        {
            m_Description.maxMaterials = maxMaterials;
            return *this;
        }

        IMaterialIdGenerator* Create(nw::os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    virtual void Accept(Material* material);

    virtual void Generate();

private:

    SortingMaterialIdGenerator(nw::os::IAllocator* allocator,MaterialKeyValueArray materials,MaterialKeyValueArray materialsWorkSpace): 
        IMaterialIdGenerator(allocator),
        m_Materials(materials),
        m_MaterialsWorkSpace(materialsWorkSpace) {}

    MaterialKeyValueArray m_Materials;
    MaterialKeyValueArray m_MaterialsWorkSpace;
};

}
}