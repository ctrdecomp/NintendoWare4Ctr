#include <nw/gfx/gfx_SortingMaterialIdGenerator.h>
#include <nw/gfx/gfx_Material.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(SortingMaterialIdGenerator);

IMaterialIdGenerator* SortingMaterialIdGenerator::Builder::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    void* memory = allocator->Alloc(sizeof(SortingMaterialIdGenerator));
    NW_NULL_ASSERT(memory);
    
    MaterialKeyValueArray materials;
    MaterialKeyValueArray materialsWorkSpace;
    if (mDescription.isFixedSizeMemory){
        materials = MaterialKeyValueArray(this->mDescription.maxMaterials, allocator);
        materialsWorkSpace = MaterialKeyValueArray(this->mDescription.maxMaterials, allocator);
    }
    else{
        materials = MaterialKeyValueArray(allocator);
        materialsWorkSpace = MaterialKeyValueArray(allocator);
    }

    SortingMaterialIdGenerator* generator =new(memory) SortingMaterialIdGenerator(allocator,materials,materialsWorkSpace);
    return generator;
}


struct MaterialKeyValueCompare : public std::binary_function<internal::MaterialKeyValue, internal::MaterialKeyValue, bool>{
    bool operator() (const internal::MaterialKeyValue& lhs, const internal::MaterialKeyValue& rhs){
        return lhs.key < rhs.key;
    }
};

struct MaterialSubKeyValueCompare : public std::binary_function<internal::MaterialKeyValue, internal::MaterialKeyValue, bool>{
    bool operator() (const internal::MaterialKeyValue& lhs, const internal::MaterialKeyValue& rhs){
        return lhs.subKey < rhs.subKey;
    }
};

struct MaterialUniqueIdCompare : public std::unary_function<internal::MaterialKeyValue, bool>{
    bool operator() (const internal::MaterialKeyValue& keyValue, const u32 uniqueId){
        return keyValue.uniqueId < uniqueId;
    }

    bool operator() (const u32 uniqueId, const internal::MaterialKeyValue& keyValue){
        return keyValue.uniqueId > uniqueId;
    }
};

void SortingMaterialIdGenerator::Accept(Material* material){
    internal::MaterialKeyValue materialKeyValue;
    materialKeyValue.key = reinterpret_cast<u32>(material->GetOriginal().ptr());
    materialKeyValue.material = material;

    bool isPushed = this->mMaterials.push_back(materialKeyValue);
    NW_ASSERT(isPushed);
}

void SortingMaterialIdGenerator::Generate(){
    std::sort(this->mMaterials.begin(), this->mMaterials.end(), gfx::MaterialKeyValueCompare());

    u32 lastPtrKey = 0x0;
    u32 uniquePtrId = 0;
    MaterialKeyValueArray::iterator materialPtrEnd = this->mMaterials.end();
    for (MaterialKeyValueArray::iterator iter = this->mMaterials.begin(); iter != materialPtrEnd; ++iter){
        if (lastPtrKey != (*iter).key){
            ++uniquePtrId;

            ResMaterial resMaterial = (*iter).material->GetOriginal();
            NW_ASSERT(resMaterial.IsValid());
            ResShaderProgramDescription resDescription = (*iter).material->GetDescription();
            NW_ASSERT(resDescription.IsValid());
            u32 key = reinterpret_cast<u32>(resDescription.GetOwnerShaderData());

            internal::MaterialKeyValue materialKeyValue;
            materialKeyValue.uniqueId = uniquePtrId;
            materialKeyValue.key = key;
            materialKeyValue.subKey = resMaterial.GetFragmentLightingTableHash();
            materialKeyValue.material = (*iter).material;
            this->mMaterialsWorkSpace.push_back(materialKeyValue);
            lastPtrKey = (*iter).key;
        }
        (*iter).uniqueId = uniquePtrId;
    }

    std::sort(this->mMaterialsWorkSpace.begin(), this->mMaterialsWorkSpace.end(), gfx::MaterialKeyValueCompare());

    u32 sortStartIndex = 0;
    u32 sortEndIndex = 0;
    u32 lastShaderKey = 0;
    u32 endIndex = this->mMaterialsWorkSpace.size() - 1;
    for (int index = 0; index < endIndex + 1; ++index){
        if (lastShaderKey != mMaterialsWorkSpace[index].key || index == endIndex){
            sortEndIndex = index;
            int diff = (sortEndIndex - sortStartIndex);

            if (diff >= 3){
                std::sort(this->mMaterialsWorkSpace.begin() + sortStartIndex,
                    this->mMaterialsWorkSpace.begin() + sortEndIndex, 
                    gfx::MaterialSubKeyValueCompare() );
            }
            sortStartIndex = index;
        }

        lastShaderKey = mMaterialsWorkSpace[index].key;
    }

    u32 materialId = 1;
    MaterialKeyValueArray::iterator shaderEnd = this->mMaterialsWorkSpace.end();
    for (MaterialKeyValueArray::iterator shaderIter = this->mMaterialsWorkSpace.begin(); shaderIter != shaderEnd; ++shaderIter){
        ::std::pair<MaterialKeyValueArray::iterator, MaterialKeyValueArray::iterator> range =
            equal_range(this->mMaterials.begin(), this->mMaterials.end(), (*shaderIter).uniqueId,
            MaterialUniqueIdCompare());

        for (MaterialKeyValueArray::iterator ptrIter = range.first; ptrIter != range.second; ++ptrIter){
            ResMaterial resMaterial = (*ptrIter).material->GetOriginal();
            resMaterial.SetMaterialId(materialId);
            ++materialId;
        }
    }

    this->mMaterials.clear();
    this->mMaterialsWorkSpace.clear();
}

}
}