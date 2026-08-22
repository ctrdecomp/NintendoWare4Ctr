#pragma once

#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/res/gfx_ResParticleModel.h>
#include <nw/gfx/gfx_ParticleShape.h>
#include <nw/anim/anim_AnimFrameController.h>
#include <nw/ut/ut_MoveArray.h>


namespace nw{
namespace gfx{

class ParticleSet;

class ParticleSetsAreEmpty{
public:
    ParticleSetsAreEmpty(bool* result){
        NW_NULL_ASSERT(result);
        mResult = result;
        *mResult = true;
    }

    void operator()(const ParticleSet* particleSet);

private:
    bool* mResult;
};

class ParticleSetsResetDebugHint{
public:
    void operator()(ParticleSet* particleSet);
};

class ParticleSetsClear{
public:
    ParticleSetsClear() {}

    void operator()(ParticleSet* particleSet);
};

class ParticleModel : public Model{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleModel);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public Model::Description{
        Description() :
            particleSetCount(0)
        {}

        uint particleSetCount;
    };

    static ParticleModel* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const ParticleModel::Description& modelDescription,
        nw::os::IAllocator* mainAllocator,
        nw::os::IAllocator* deviceAllocator);

    static size_t GetMemorySize(ResParticleModel resource,const ParticleModel::Description& modelDescription,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, modelDescription);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResParticleModel resource,const ParticleModel::Description& modelDescription);

    static size_t GetDeviceMemorySize(ResParticleModel resource,const ParticleModel::Description& modelDescription,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetDeviceMemorySizeInternal(&size, resource, modelDescription);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetDeviceMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResParticleModel resource,const ParticleModel::Description& modelDescription);
    
    virtual void Accept(ISceneVisitor* visitor);

    void UpdateParticleFrame(){
        this->mParticleAnimFrameController.UpdateFrame();
    }

    template<typename TFunction>
    void ForeachParticleSet(TFunction function){
        for (int i = 0; i < (int)this->GetParticleSetsCount(); ++i){
            ParticleSet* particleSet = this->GetParticleSets(i);

            function(particleSet);
        }
    }

    template<typename TFunction>
    void ForeachConstParticleSet(TFunction function) const{
        for (int i = 0; i < (int)this->GetParticleSetsCount(); ++i){
            const ParticleSet* particleSet = this->GetParticleSets(i);

            function(particleSet);
        }
    }

    ResParticleModel GetResModel(){
        return ResDynamicCast<ResParticleModel>(this->GetResSceneObject());
    }

    const ResParticleModel GetResModel() const{
        return ResDynamicCast<ResParticleModel>(this->GetResSceneObject());
    }

    bool AttachParticleSet(ParticleSet* node){
        if (mParticleSetCount >= this->mParticleSets.size()) return false;

        this->mParticleSets[mParticleSetCount] = node;
        ++this->mParticleSetCount;
        return true;
    }

    u32 GetParticleSetsCount() const{
        return mParticleSetCount;
    }

    ParticleSet* GetParticleSets(int index){
        NW_ASSERT(index >= 0 && index < mParticleSetCount);
        return this->mParticleSets[index];
    }

    const ParticleSet* GetParticleSets(int index) const{
        NW_ASSERT(index >= 0 && index < mParticleSetCount);
        return this->mParticleSets[index];
    }

    bool AttachParticleShape(ParticleShape* node){
        if (mParticleShapeCount >= this->mParticleShapes.size()) return false;

        this->mParticleShapes[this->mParticleSetCount] = node;
        ++this->mParticleShapeCount;
        return true;
    }

    u32 GetParticleShapesCount() const{
        return mParticleShapeCount;
    }

    ParticleShape* GetParticleShapes(int index){
        NW_ASSERT(index >= 0 && index < mParticleShapeCount);
        return this->mParticleShapes[index];
    }

    const ParticleShape* GetParticleShapes(int index) const{
        NW_ASSERT(index >= 0 && index < mParticleShapeCount);
        return this->mParticleShapes[index];
    }

    anim::AnimFrameController& ParticleAnimFrameController(){
        return mParticleAnimFrameController;
    }

    const anim::AnimFrameController& ParticleAnimFrameController() const{
        return mParticleAnimFrameController;
    }

    bool HasParticle() const{
        bool result;
        ParticleSetsAreEmpty function(&result);
        this->ForeachConstParticleSet(function);
        return !result;
    }

protected:
    virtual Result Initialize(nw::os::IAllocator* allocator);

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize,ResParticleModel resource,const ParticleModel::Description& modelDescription);

    ParticleModel(nw::os::IAllocator* allocator,ResTransformNode resource,const ParticleModel::Description& description): 
        Model(allocator,resource,description),
        mMaximumParticleSet(description.particleSetCount),
        mParticleSetCount(0),
        mParticleShapeCount(0),
        mParticleAnimFrameController(0, 16777215, anim::PlayPolicy_Loop)
    {}

    virtual ~ParticleModel(){
        for (int i = 0; i < mParticleShapes.size(); ++i)
            SafeDestroy(this->mParticleShapes[i]);
        }
    private:
        uint mMaximumParticleSet;
        int mParticleSetCount;
        nw::ut::MoveArray<ParticleSet*> mParticleSets;
        int mParticleShapeCount;
        nw::ut::MoveArray<ParticleShape*> mParticleShapes;
        anim::AnimFrameController mParticleAnimFrameController;
};

}
}