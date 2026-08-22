#pragma once

#include <nw/gfx/gfx_SceneObject.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_ShaderProgram.h>

namespace nw{
namespace gfx{

class Model;
class AnimGroup;

class Material : public SceneObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Material);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static Material* Create(ResMaterial resource,s32 bufferCount,Model* parent,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResMaterial resource,s32 bufferCount,bit32 bufferOption,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, bufferCount, bufferOption);

        return size.GetSizeWithPadding(alignment);
    }


    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResMaterial resource,s32 bufferCount,bit32 bufferOption);

    ResMaterial GetOriginal() {
        return ResStaticCast<ResMaterial>(this->GetResSceneObject());
    }

    const ResMaterial GetOriginal() const {
        return ResStaticCast<ResMaterial>(this->GetResSceneObject());
    }

    Model* GetOwnerModel(){
        return this->mOwner;
    }

    const Model* GetOwnerModel() const{
        return this->mOwner;
    }

    void SetDescription(ResShaderProgramDescription description) { mProgramDescription = description; }

    const ResShaderProgramDescription GetDescription() const { return mProgramDescription; }

    int GetBufferCount() const{
        return this->mBuffers.size();
    }

    ResMaterial GetBuffer(){
        bool hasBuffer = (0 < mBufferCount);

        if (hasBuffer){
            return this->mBuffers[0];
        }
        else{
            return ResMaterial(NULL);
        }
    }

    const ResMaterial GetBuffer() const{
        bool hasBuffer = (0 < mBufferCount);

        if (hasBuffer){
            return this->mBuffers[0];
        }
        else{
            return ResMaterial(NULL);
        }
    }

    ResMaterial& GetShaderParameterResMaterial() { return mShaderParameterResMaterial; }
    const ResMaterial& GetShaderParameterResMaterial() const { return mShaderParameterResMaterial; }
    ResMaterial& GetShadingParameterResMaterial() { return mShadingParameterResMaterial; }
    const ResMaterial& GetShadingParameterResMaterial() const { return mShadingParameterResMaterial; }
    ResMaterial& GetMaterialColorResMaterial() { return mMaterialColorResMaterial; }
    const ResMaterial& GetMaterialColorResMaterial() const { return mMaterialColorResMaterial; }
    ResMaterial& GetRasterizationResMaterial() { return mRasterizationResMaterial; }
    const ResMaterial& GetRasterizationResMaterial() const { return mRasterizationResMaterial; }
    ResMaterial& GetTextureCoordinatorResMaterial() { return mTextureCoordinatorResMaterial; }
    const ResMaterial& GetTextureCoordinatorResMaterial() const { return mTextureCoordinatorResMaterial; }
    ResMaterial& GetTextureMapperResMaterial() { return mTextureMapperResMaterial; }
    const ResMaterial& GetTextureMapperResMaterial() const { return mTextureMapperResMaterial; }
    ResMaterial& GetFragmentLightingResMaterial() { return mFragmentLightingResMaterial; }
    const ResMaterial& GetFragmentLightingResMaterial() const { return mFragmentLightingResMaterial; }
    ResMaterial& GetFragmentLightingTableResMaterial() { return mFragmentLightingTableResMaterial; }
    const ResMaterial& GetFragmentLightingTableResMaterial() const { return mFragmentLightingTableResMaterial; }
    ResMaterial& GetTextureCombinerResMaterial() { return mTextureCombinerResMaterial; }
    const ResMaterial& GetTextureCombinerResMaterial() const { return mTextureCombinerResMaterial; }
    ResMaterial& GetAlphaTestResMaterial() { return mAlphaTestResMaterial; }
    const ResMaterial& GetAlphaTestResMaterial() const { return mAlphaTestResMaterial; }
    ResMaterial& GetFragmentOperationResMaterial() { return mFragmentOperationResMaterial; }
    const ResMaterial& GetFragmentOperationResMaterial() const { return mFragmentOperationResMaterial; }
    ResMaterial& GetSceneEnvironmentResMaterial() { return mSceneEnvironmentResMaterial; }
    const ResMaterial& GetSceneEnvironmentResMaterial() const { return mSceneEnvironmentResMaterial; }

protected:
    Material(nw::os::IAllocator* allocator,ResMaterial resMaterial,s32 bufferCount,Model* parent);
    virtual ~Material();


    struct ResMaterialDestroyer : public std::unary_function<ResMaterial, void>{
        ResMaterialDestroyer(nw::os::IAllocator* allocator = 0): 
            mAllocator(allocator)
        {}
        result_type operator()(argument_type data){
            DestroyResMaterial(mAllocator, data);
        }

        nw::os::IAllocator* mAllocator;
    };
    
private:

    Result Initialize(nw::os::IAllocator* allocator);
    Result CreateBuffers(nw::os::IAllocator* allocator);

    ::std::pair<ResMaterial, Result> CopyResMaterial(nw::os::IAllocator* allocator,bit32 bufferOption);

    static void DestroyResMaterial(nw::os::IAllocator* allocator, ResMaterial resMaterial);

    static void DestroyResFragmentShader(nw::os::IAllocator* allocator,ResFragmentShader resFragmentShader);

    static void DestroyResShaderParameter(nw::os::IAllocator* allocator,ResShaderParameter resShaderParameter);

    ::std::pair<ResFragmentLightingTable, Result> CopyResFragmentLightingTable(nw::os::IAllocator* allocator,ResFragmentLightingTable resFragmentLightingTable);

    ::std::pair<ResLightingLookupTable, Result> CopyResLightingLookupTable(nw::os::IAllocator* allocator,ResLightingLookupTable resLightingLookupTable);

    ResMaterial GetActiveResource(u32 objectType){
        return (CanUseBuffer(objectType)) ? GetBuffer() : GetOriginal();
    }

    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim, const ResMaterial resMaterial);

    bool CanUseBuffer(u32 objectType) const;

    typedef nw::ut::MoveArray<ResMaterial> ResMaterialArray;

    ResMaterialArray mBuffers;
    Model* mOwner;
    ResShaderProgramDescription mProgramDescription;
    s32 mBufferCount;

    ResMaterial mShaderParameterResMaterial;
    ResMaterial mShadingParameterResMaterial;
    ResMaterial mMaterialColorResMaterial;
    ResMaterial mRasterizationResMaterial;
    ResMaterial mTextureCoordinatorResMaterial;
    ResMaterial mTextureMapperResMaterial;
    ResMaterial mFragmentLightingResMaterial;
    ResMaterial mFragmentLightingTableResMaterial;
    ResMaterial mTextureCombinerResMaterial;
    ResMaterial mAlphaTestResMaterial;
    ResMaterial mFragmentOperationResMaterial;
    ResMaterial mSceneEnvironmentResMaterial;

    friend class Model;
};

typedef nw::ut::MoveArray<Material*> MaterialArray;

}
}