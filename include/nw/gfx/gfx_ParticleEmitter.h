#pragma once

#include <nw/gfx/gfx_TransformNode.h>

#include <nw/gfx/res/gfx_ResParticleEmitter.h>
#include <nw/gfx/gfx_ParticleCollection.h>
#include <nw/gfx/gfx_ParticleRandom.h>
#include <nw/anim/anim_AnimFrameController.h>

namespace nw{
namespace gfx{

class ParticleEmitter : public TransformNode{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleEmitter);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public TransformNode::Description{
        Description()
        {}
    };


    static ParticleEmitter* Create(SceneNode* parent,ResSceneObject resource,const ParticleEmitter::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResParticleEmitter resource,const ParticleEmitter::Description& description,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResParticleEmitter resource,const ParticleEmitter::Description& description);

    virtual void Accept(ISceneVisitor* visitor);

    void UpdateParticleFrame(){
        this->mParticleAnimFrameController.UpdateFrame();
    }

    void Reset(){
        mIsFirstEmission = true;
        mEmissionCount = 0;
        mNextEmissionTime = 0;

        this->mParticleAnimFrameController.SetFrame(0);
    }

    ResParticleEmitter GetResParticleEmitter(){
        return ResDynamicCast<ResParticleEmitter>( this->GetResSceneObject() );
    }

    const ResParticleEmitter GetResParticleEmitter() const{
        return ResDynamicCast<ResParticleEmitter>( this->GetResSceneObject() );
    }

    ResParticleEmitterParameter GetResParticleEmitterParameterCopy(bool copyOnly = false){
        if (mResParameter.IsValid()){
            return mResParameter;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid()){
            return ResParticleEmitterParameter(&this->GetResParticleEmitter().ptr()->mIsResourceCopyEnabled);
        }

        return mResParameter;
    }

    const ResParticleEmitterParameter GetResParticleEmitterParameterCopy(bool copyOnly = false) const{
        if (this->mResParameter.IsValid()){
            return mResParameter;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid()){
            return ResParticleEmitterParameter(&this->GetResParticleEmitter().ptr()->mIsResourceCopyEnabled);
        }

        return mResParameter;
    }

    ResParticleForm GetResParticleFormCopy(bool copyOnly = false){
        if (this->mResForm.IsValid()){
            return mResForm;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid()){
            return this->GetResParticleEmitter().GetParticleForm();
        }

        return mResForm;
    }

    const ResParticleForm GetResParticleFormCopy(bool copyOnly = false) const
    {
        if (this->mResForm.IsValid()){
            return mResForm;
        }

        if (!copyOnly && this->GetResParticleEmitter().IsValid()){
            return this->GetResParticleEmitter().GetParticleForm();
        }

        return mResForm;
    }

    int GetEmissionCount(f32 prevTime, f32 time);

    void SetParticleSet(ParticleSet* particleSet){
        this->mParticleSet = particleSet;
    }

    ParticleSet* GetParticleSet(){
        return this->mParticleSet;
    }

    const ParticleSet* GetParticleSet() const{
        return this->mParticleSet;
    }

    anim::AnimFrameController& ParticleAnimFrameController(){
        return mParticleAnimFrameController;
    }

    const anim::AnimFrameController& ParticleAnimFrameController() const{
        return mParticleAnimFrameController;
    }

    void Srand(u32 seed){
        mParticleRandom.Srand(seed);
    }

    bool IsAlive() const{
        const ResParticleEmitterParameter resource = this->GetResParticleEmitterParameterCopy(false);
        if (!resource.IsValid()){
            return false;
        }

        if (resource.GetEmissionRatio() == 0){
            return false;
        }

        if (resource.GetEmissionSpanInfinity()){
            return true;
        }

        f32 time = this->mParticleAnimFrameController.GetFrame();
        f32 cookedTime = time - resource.GetEmissionStart() - 1;
        if (cookedTime >= resource.GetEmissionSpan()){
            return false;
        }

        return true;
    }

    int Emission(ParticleContext* particleContext);

    static void CalcCubeForm(const ResParticleCubeForm& cubeForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcCylinderForm(const ResParticleCylinderForm& cylinderForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcDiscForm(const ResParticleDiscForm& discForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcPointForm(const ResParticlePointForm& pointForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcSphereForm(const ResParticleSphereForm& sphereForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);

    static void CalcRectangleForm(const ResParticleRectangleForm& rectangleForm,int emissionCount,ParticleRandom* random,nw::math::VEC3* positions);
protected:

    ParticleEmitter(nw::os::IAllocator* allocator,ResParticleEmitter resObj,const ParticleEmitter::Description& description,
        ResParticleEmitterParameter resParameterObj,
        ResParticleForm resFormObj);

    virtual ~ParticleEmitter();

private:
    bool mIsFirstEmission;
    f32 mEmissionCount;
    s32 mNextEmissionTime;
    ParticleSet* mParticleSet;

    ParticleRandom mParticleRandom;
    anim::AnimFrameController mParticleAnimFrameController;

    ResParticleEmitterParameter mResParameter;
    ResParticleForm mResForm;
};

}
}