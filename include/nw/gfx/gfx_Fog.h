#pragma once

#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/res/gfx_ResFog.h>

#include <nw/ut/ut_MovePtr.h>
#include <functional>

namespace nw{
namespace gfx{

class Camera;

class Fog : public TransformNode{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Fog);

    #define NW_FOG_TABLE_COMMAND_NUM    (132)
    #define NW_FOG_TABLE_COMMAND_SIZE   (NW_FOG_TABLE_COMMAND_NUM * 4)

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public TransformNode::Description{
        Description(){}
    };

    struct UpdateFunctor {
        UpdateFunctor(Camera* camera): 
            camera(camera) 
        {}
        
        void operator() (Fog* fog){
            if (fog != NULL){
                fog->Update(camera);
            }
        }
        
        Camera* camera;
    };

    class DynamicBuilder{
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory){
            mDescription.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        DynamicBuilder& MaxChildren(int maxChildren){
            mDescription.maxChildren = maxChildren;
            return *this;
        }

        DynamicBuilder& MaxCallbacks(int maxCallbacks){
            mDescription.maxCallbacks = maxCallbacks;
            return *this;
        }

        Fog* Create(nw::os::IAllocator* allocator);
        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        Fog::Description mDescription;
    };


    static Fog* Create(SceneNode* parent,ResSceneObject resource,const Fog::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResFog resource,Description description,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal( nw::os::MemorySizeCalculator* pSize,ResFog resource,Description description);

    void Update(const Camera* camera);

    virtual void Accept(ISceneVisitor* visitor);

    ResFog GetResFog() {
        return ResStaticCast<ResFog>(this->GetResSceneObject());
    }

    const ResFog GetResFog() const{
        return ResStaticCast<ResFog>(this->GetResSceneObject());
    }

    AnimGroup* GetAnimGroup() { return mAnimGroup; }

    const AnimGroup* GetAnimGroup() const { return mAnimGroup; }

    AnimObject* GetAnimObject() { return this->mAnimBinding->GetAnimObject(0); }

    const AnimObject* GetAnimObject() const { return this->mAnimBinding->GetAnimObject(0); }

    void SetAnimObject(AnimObject* animObject) { this->mAnimBinding->SetAnimObject(0, animObject); }

protected:
    struct ResFogDataDestroyer : public std::unary_function<ResFogData*, void>{
        ResFogDataDestroyer(nw::os::IAllocator* allocator = 0): 
            mAllocator(allocator)
        {}
        result_type operator()(argument_type data){
            DestroyResFog(this->mAllocator, data);
        }

        nw::os::IAllocator* mAllocator;
    };

    struct ResFogUpdaterDataDestroyer : public std::unary_function<ResFogUpdaterData*, void>
    {
        ResFogUpdaterDataDestroyer(os::IAllocator* allocator = 0): 
            mAllocator(allocator)
        {}
        result_type operator()(argument_type data){
            DestroyResFogUpdater(this->mAllocator, data);
        }

        nw::os::IAllocator* mAllocator;
    };

    typedef nw::ut::MovePtr<ResFogData, ResFogDataDestroyer> ResPtr;
    typedef nw::ut::MovePtr<ResFogUpdaterData, ResFogUpdaterDataDestroyer> ResUpdaterPtr;

    Fog(nw::os::IAllocator* allocator,ResFog resObj,const Fog::Description& description): 
        TransformNode(allocator,resObj,description),
        mNear(0.0f),
        mFar(0.0f),
        mWScale(0.0f),
        mAnimGroup(NULL),
        mOriginalValue(NULL)
    {}

    Fog(nw::os::IAllocator* allocator,ResPtr resource,const Fog::Description& description): 
        TransformNode(allocator,ResFog(resource.Get()),description),
        mResource(resource),
        mNear(0.0f),
        mFar(0.0f),
        mWScale(0.0f),
        mAnimGroup(NULL),
        mOriginalValue(NULL)
    {}

    virtual ~Fog(){
        nw::ut::SafeDestroy(this->mAnimGroup);

        if (this->mOriginalValue.IsValid()){
            GetAllocator().Free(this->mOriginalValue.ptr());
            mOriginalValue = ResFog(NULL);
        }
    }

private:
    virtual Result Initialize(nw::os::IAllocator* allocator);

    Result CreateAnimGroup(nw::os::IAllocator* allocator);

    void BindAnim(AnimGroup* animGroup);

    Result CreateOriginalValue(nw::os::IAllocator* allocator);

    static ResFogData*    CreateResFog(nw::os::IAllocator* allocator, const char* name = NULL);

    static void DestroyResFog(nw::os::IAllocator* allocator, ResFogData* resFog);

    static ResFogUpdaterData*    CreateResFogUpdater(nw::os::IAllocator* allocator);

    static void DestroyResFogUpdater(nw::os::IAllocator* allocator, ResFogUpdaterData* resFogUpdater);

    void SetupFogSampler(ResImageLookupTable fogSampler,ResFogUpdater fogUpdater,const nw::math::MTX44& inverseProjectionMatrix);

    ResPtr mResource;
    ResUpdaterPtr mUpdaterCache;
    f32 mNear;
    f32 mFar;
    f32 mWScale;
    AnimGroup* mAnimGroup;
    ResFog mOriginalValue;
    static const float FOG_DENSITY;
    static const ResFogUpdater::FogUpdaterType FOG_UPDATER_TYPE;
    static const float FOG_MAX_DEPTH;
    static const float FOG_MIN_DEPTH;
};

}
}