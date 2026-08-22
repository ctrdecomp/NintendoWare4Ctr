#pragma once

#include <nw/gfx/gfx_IMaterialIdGenerator.h>

namespace nw{
namespace gfx{
namespace internal{
    struct MaterialKeyValue{
        u32 uniqueId;
        u32 key;
        u32 subKey;
        Material* material;

    };
}

class SortingMaterialIdGenerator : public IMaterialIdGenerator{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SortingMaterialIdGenerator);

    typedef nw::ut::MoveArray<internal::MaterialKeyValue> MaterialKeyValueArray;
public:
    NW_UT_RUNTIME_TYPEINFO;
    
    struct Description{        
        bool isFixedSizeMemory;
        int  maxMaterials;

        Description(): 
            isFixedSizeMemory(true),
            maxMaterials(128)
        {}
    };

    class Builder{
    public:
        Builder& IsFixedSizeMemory(bool isFixedSizeMemory){
            mDescription.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        Builder& MaxMaterials(int maxMaterials){
            mDescription.maxMaterials = maxMaterials;
            return *this;
        }

        IMaterialIdGenerator* Create(nw::os::IAllocator* allocator);

    private:
        Description mDescription;
    };

    virtual void Accept(Material* material);

    virtual void Generate();

private:

    SortingMaterialIdGenerator(nw::os::IAllocator* allocator,MaterialKeyValueArray materials,MaterialKeyValueArray materialsWorkSpace): 
        IMaterialIdGenerator(allocator),
        mMaterials(materials),
        mMaterialsWorkSpace(materialsWorkSpace)
    {}

    MaterialKeyValueArray mMaterials;
    MaterialKeyValueArray mMaterialsWorkSpace;
};

}
}