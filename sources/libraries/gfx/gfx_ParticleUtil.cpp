// Filename: gfx_ParticleUtil.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_ParticleUtil.h>
#include <nw/gfx/gfx_particleModel.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_ParticleContext.h>
#include <nw/gfx/gfx_ParticleEmitter.h>

namespace nw{
namespace gfx{

ParticleSet* FindParticleSet(const char* name, ut::MoveArray<SceneNode*>* sceneNodeArray)
{
    for (ut::MoveArray<SceneNode*>::iterator i = sceneNodeArray->Begin(); i != sceneNodeArray->End(); ++i)
    {
        ParticleModel* particleModel = ut::DynamicCast<ParticleModel*>(*i);
        if (!particleModel)
        {
            continue;
        }

        for (int j = 0; j < (int)particleModel->GetParticleSetsCount(); ++j)
        {
            ParticleSet* itParticleSet = particleModel->GetParticleSets(j);
            NW_NULL_ASSERT(itParticleSet);

            const char* itParticleSetName = itParticleSet->GetResParticleSet().GetName();

            if (std::strcmp(itParticleSetName, name) == 0)
            {
                return itParticleSet;
            }
        }
    }

    return NULL;
}

void ParticleUtil::SetupParticleObject(ut::MoveArray<SceneNode*>* sceneNodeArray, ParticleContext* particleContext)
{
    NW_NULL_ASSERT(sceneNodeArray);
    NW_NULL_ASSERT(particleContext);

    ResolveParticleObject(sceneNodeArray);

    NW_FOREACH(SceneNode* node, *sceneNodeArray)
    {
        ResSceneNode resSceneNode = node->GetResSceneNode();
        NW_ASSERT(resSceneNode.IsValid());

        switch (resSceneNode.ptr()->typeInfo)
        {
        case ResParticleModel::TYPE_INFO:{
                ParticleModel* model = static_cast<ParticleModel*>(node);
                NW_NULL_ASSERT(model);

                for (u32 i = 0; i < model->GetParticleSetsCount(); ++i)
                {
                    ParticleSet* particleSet = model->GetParticleSets(i);
                    NW_NULL_ASSERT(particleSet);

                    particleSet->Srand(particleContext->GetRandom());
                }
            }
            break;
        case ResParticleEmitter::TYPE_INFO:{
                ParticleEmitter* emitter = static_cast<ParticleEmitter*>(node);
                NW_NULL_ASSERT(emitter);

                emitter->Srand(particleContext->GetRandom());
            }
        }
    }
}

void ParticleUtil::ResolveParticleObject(ut::MoveArray<SceneNode*>* sceneNodeArray)
{
    NW_NULL_ASSERT(sceneNodeArray);

    NW_FOREACH(SceneNode* node, *sceneNodeArray)
    {
        ResSceneNode resSceneNode = node->GetResSceneNode();
        NW_ASSERT(resSceneNode.IsValid());

        switch (resSceneNode.ptr()->typeInfo)
        {
        case ResParticleModel::TYPE_INFO:{

                ParticleModel* model = static_cast<ParticleModel*>(node);
                NW_NULL_ASSERT(model);

                for (u32 i = 0; i < model->GetParticleSetsCount(); ++i)
                {
                    ParticleSet* particleSet = model->GetParticleSets(i);
                    NW_NULL_ASSERT(particleSet);

                    ut::MoveArray<ParticleSet::Updater>* updaters = particleSet->GetUpdaters();
                    NW_NULL_ASSERT(updaters);

                    ut::MoveArray<ParticleSet::Updater>::iterator endIter = updaters->end();
                    for (ut::MoveArray<ParticleSet::Updater>::iterator iter = updaters->begin(); iter != endIter;)
                    {
                        ParticleSet::Updater& updater = *iter++;

                        ResParticleUpdater resUpdater(updater.resource);
                        NW_ASSERT(resUpdater.IsValid());

                        if (resUpdater.GetTypeInfo() == ResParticleChildUpdater::TYPE_INFO)
                        {
                            ResParticleChildUpdater childUpdater = ResDynamicCast<ResParticleChildUpdater>(resUpdater);

                            const char* particleSetPath = childUpdater.GetParticleSetPath();

                            ParticleSet* childParticleSet = FindParticleSet(particleSetPath, sceneNodeArray);
                            if (childParticleSet != NULL)
                            {
                                updater.work = (u32)childParticleSet;
                            }
                        }
                    }
                }
            }
            break;

        case ResParticleEmitter::TYPE_INFO:{
                ParticleEmitter* emitter = static_cast<ParticleEmitter*>(node);
                NW_NULL_ASSERT(emitter);

                ResParticleEmitter resource = emitter->GetResParticleEmitter();
                if (resource.IsValid())
                {
                    ParticleSet* particleSet = FindParticleSet(resource.GetParticleSetPath(), sceneNodeArray);

                    emitter->SetParticleSet(particleSet);
                }
            }
            break;
        }
    }
}

bool ParticleSetCompare::operator()(const ParticleSet* lhs, const ParticleSet* rhs)
{
    if (rhs == NULL)
    {
        return true;
    }

    if (lhs == NULL)
    {
        return false;
    }

    const ResParticleSet& lhsResource = lhs->GetResParticleSet();
    const ResParticleSet& rhsResource = rhs->GetResParticleSet();
    return lhsResource.GetParticleSetPriority() < rhsResource.GetParticleSetPriority();
}

void ParticleUtil::GetMemorySizeForDuplicateResParticleInitializerInternal(nw::os::MemorySizeCalculator* pSize, const ResParticleInitializer* src)
{
    nw::os::MemorySizeCalculator& size = *pSize;

    int baseSize = 0;

    switch (src->GetTypeInfo())
    {
    case ResParticleInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleInitializerData);
        break;
    case ResParticleDirectionalVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleDirectionalVelocityInitializerData);
        break;
    case ResParticleRandomDirectionalVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleRandomDirectionalVelocityInitializerData);
        break;
    case ResParticleOriginVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleOriginVelocityInitializerData);
        break;
    case ResParticleRandomVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleRandomVelocityInitializerData);
        break;
    case ResParticleYAxisVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleYAxisVelocityInitializerData);
        break;
    case ResParticleVector3ImmediateInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector3ImmediateInitializerData);
        break;
    case ResParticleFloatRandomInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleFloatRandomInitializerData);
        break;
    case ResParticleFloatRangeRandomInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleFloatRangeRandomInitializerData);
        break;
    case ResParticleFloatImmediateInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleFloatImmediateInitializerData);
        break;
    case ResParticleVector2ImmediateInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector2ImmediateInitializerData);
        break;
    case ResParticleVector3Random1Initializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector3Random1InitializerData);
        break;
    case ResParticleVector3Random3Initializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector3Random3InitializerData);
        break;
    case ResParticleVector3MultRandomInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector3MultRandomInitializerData);
        break;
    default:
        NW_FATAL_ERROR("unknown initializer type");
    }

    size.Add(baseSize, 4);
}

ResParticleInitializerData* ParticleUtil::DuplicateResParticleInitializer(const ResParticleInitializer* src, nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(src);
    NW_NULL_ASSERT(allocator);

    int baseSize = 0;
    int size = 0;

    switch (src->GetTypeInfo())
    {
    case ResParticleInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleInitializerData);
        break;
    case ResParticleDirectionalVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleDirectionalVelocityInitializerData);
        break;
    case ResParticleRandomDirectionalVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleRandomDirectionalVelocityInitializerData);
        break;
    case ResParticleOriginVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleOriginVelocityInitializerData);
        break;
    case ResParticleRandomVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleRandomVelocityInitializerData);
        break;
    case ResParticleYAxisVelocityInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleYAxisVelocityInitializerData);
        break;
    case ResParticleVector3ImmediateInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector3ImmediateInitializerData);
        break;
    case ResParticleFloatRandomInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleFloatRandomInitializerData);
        break;
    case ResParticleFloatRangeRandomInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleFloatRangeRandomInitializerData);
        break;
    case ResParticleFloatImmediateInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleFloatImmediateInitializerData);
        break;
    case ResParticleVector2ImmediateInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector2ImmediateInitializerData);
        break;
    case ResParticleVector3Random1Initializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector3Random1InitializerData);
        break;
    case ResParticleVector3Random3Initializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector3Random3InitializerData);
        break;
    case ResParticleVector3MultRandomInitializer::TYPE_INFO:
        baseSize += sizeof(ResParticleVector3MultRandomInitializerData);
        break;
    default:
        NW_FATAL_ERROR("unknown initializer type");
    }

    size += ut::RoundUp(baseSize, 4);

    void* memory = allocator->Alloc(size, 4);
    if (memory == NULL)
    {
        return NULL;
    }

    {
        u8* dstPtr = (u8*)memory;

        ResParticleInitializerData* dstData = reinterpret_cast<ResParticleInitializerData*>(dstPtr);
        const ResParticleInitializerData* srcData = src->ptr();

        ::nw::os::MemCpy(dstPtr, srcData, baseSize);
        dstPtr += ut::RoundUp(baseSize, 4);
    }

    switch (src->GetTypeInfo())
    {
    case ResParticleInitializer::TYPE_INFO:
    case ResParticleDirectionalVelocityInitializer::TYPE_INFO:
    case ResParticleRandomDirectionalVelocityInitializer::TYPE_INFO:
    case ResParticleOriginVelocityInitializer::TYPE_INFO:
    case ResParticleRandomVelocityInitializer::TYPE_INFO:
    case ResParticleYAxisVelocityInitializer::TYPE_INFO:
    case ResParticleVector3ImmediateInitializer::TYPE_INFO:
    case ResParticleFloatRandomInitializer::TYPE_INFO:
    case ResParticleFloatRangeRandomInitializer::TYPE_INFO:
    case ResParticleFloatImmediateInitializer::TYPE_INFO:
    case ResParticleVector2ImmediateInitializer::TYPE_INFO:
    case ResParticleVector3Random1Initializer::TYPE_INFO:
    case ResParticleVector3Random3Initializer::TYPE_INFO:
    case ResParticleVector3MultRandomInitializer::TYPE_INFO:
        break;
    default:
        NW_FATAL_ERROR("unknown initializer type");
    }

    return reinterpret_cast<ResParticleInitializerData*>(memory);
}

void ParticleUtil::GetMemorySizeForDuplicateResParticleUpdaterInternal(nw::os::MemorySizeCalculator* pSize, const ResParticleUpdater* src)
{
    int baseSize = 0;
    nw::os::MemorySizeCalculator& size = *pSize;
    int childStrSize = 0;
    int childFormSize = 0;
    int childOptionSize = 0;

    switch (src->GetTypeInfo())
    {
    case ResParticleUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleUpdaterData);
        break;
    case ResParticleAccelarationUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleAccelarationUpdaterData);
        break;
    case ResParticleGeneralUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleGeneralUpdaterData);
        break;
    case ResParticleGravityUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleGravityUpdaterData);
        break;
    case ResParticleSpinUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleSpinUpdaterData);
        break;
    case ResParticleRandomUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleRandomUpdaterData);
        break;
    case ResParticleChildUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleChildUpdaterData);
        {
            const ResParticleChildUpdater& child = ResDynamicCast<ResParticleChildUpdater>(*src);
            NW_ASSERT(child.IsValid());

            childStrSize = strlen(child.GetParticleSetPath()) + 1;

            const ResParticleForm& form = child.GetParticleForm();
            const ResParticleChildUpdaterOption& option = child.GetTiming();

            if (form.IsValid())
            {

                switch (form.GetTypeInfo())
                {
                case ResParticleForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleFormData);
                    break;
                case ResParticleCubeForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleCubeFormData);
                    break;
                case ResParticleCylinderForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleCylinderFormData);
                    break;
                case ResParticleDiscForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleDiscFormData);
                    break;
                case ResParticlePointForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticlePointFormData);
                    break;
                case ResParticleRectangleForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleRectangleFormData);
                    break;
                case ResParticleSphereForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleSphereFormData);
                    break;
                default:
                    NW_FATAL_ERROR("unknown form type");
                }
            }

            if (option.IsValid())
            {
                switch (option.GetTypeInfo())
                {
                case ResParticleChildUpdaterFinalUpdateOption::TYPE_INFO:
                    childOptionSize = sizeof(ResParticleChildUpdaterFinalUpdateOptionData);
                    break;
                case ResParticleChildUpdaterFirstUpdateOption::TYPE_INFO:
                    childOptionSize = sizeof(ResParticleChildUpdaterFirstUpdateOptionData);
                    break;
                case ResParticleChildUpdaterIntervalOption::TYPE_INFO:
                    childOptionSize = sizeof(ResParticleChildUpdaterIntervalOptionData);
                    break;
                default:
                    NW_FATAL_ERROR("unknown updateOption type");
                }
            }
        }
        break;
    case ResParticleUserUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleUserUpdaterData);
        break;
    case ResParticleVector2ImmediateUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector2ImmediateUpdaterData);
        break;
    case ResParticleVector3Updater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector3UpdaterData);
        break;
    case ResParticleFloatUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleFloatUpdaterData);
        break;
    case ResParticleRotateUpVectorUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleRotateUpVectorUpdaterData);
        break;
    case ResParticleTexturePatternUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleTexturePatternUpdaterData);
        break;
    case ResParticleVector3ImmediateUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector3ImmediateUpdaterData);
        break;
    case ResParticleVector3AdditiveUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector3AdditiveUpdaterData);
        break;
    case ResParticleVector3RandomAdditiveUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector3RandomAdditiveUpdaterData);
        break;
    case ResParticleFloatImmediateUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleFloatImmediateUpdaterData);
        break;
    default:
        NW_FATAL_ERROR("unknown updater type");
        break;
    }

    int animationSize = 0;
    int enabledSize = 0;
    int dataSize = 0;
    int animationOptionSize = 0;
    {
        const ResParticleAnimation& resAnimation = src->GetParticleAnimation();
        if (resAnimation.IsValid())
        {
            animationSize += sizeof(ResParticleAnimationData);

            enabledSize = resAnimation.GetAnimationEnabledCount() * sizeof(bool);
            dataSize = resAnimation.GetAnimationDataCount() * sizeof(s32);

            const ResParticleAnimationOption& option = resAnimation.GetParticleAnimationOption();
            if (option.IsValid())
            {
                switch (option.GetTypeInfo())
                {
                case ResParticleAnimationOption::TYPE_INFO:
                    animationOptionSize = sizeof(ResParticleAnimationOptionData);
                    break;
                case ResParticleFittingAnimationOption::TYPE_INFO:
                    animationOptionSize = sizeof(ResParticleFittingAnimationOptionData);
                    break;
                case ResParticleFrameLoopAnimationOption::TYPE_INFO:
                    animationOptionSize = sizeof(ResParticleFrameLoopAnimationOptionData);
                    break;
                case ResParticleRandomAnimationOption::TYPE_INFO:
                    animationOptionSize = sizeof(ResParticleRandomAnimationOptionData);
                    break;
                default:
                    NW_FATAL_ERROR("unknown animationOption type");
                }
            }

            animationSize += resAnimation.GetAnimationEnabledCount() * sizeof(u32);
            animationSize += resAnimation.GetAnimationDataCount() * sizeof(u32);
        }
    }

    size.Add(baseSize, 4);
    size.Add(animationSize, 4);
    size.Add(enabledSize, 4);
    size.Add(dataSize, 4);
    size.Add(animationOptionSize, 4);
    size.Add(childStrSize, 4);
    size.Add(childFormSize, 4);
    size.Add(childOptionSize, 4);
}

ResParticleUpdaterData* ParticleUtil::DuplicateResParticleUpdater(const ResParticleUpdater* src, nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(src);
    NW_NULL_ASSERT(allocator);

    int baseSize = 0;
    int size = 0;
    int childStrSize = 0;
    int childFormSize = 0;
    int childOptionSize = 0;

    switch (src->GetTypeInfo())
    {
    case ResParticleUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleUpdaterData);
        break;
    case ResParticleAccelarationUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleAccelarationUpdaterData);
        break;
    case ResParticleGeneralUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleGeneralUpdaterData);
        break;
    case ResParticleGravityUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleGravityUpdaterData);
        break;
    case ResParticleSpinUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleSpinUpdaterData);
        break;
    case ResParticleRandomUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleRandomUpdaterData);
        break;
    case ResParticleChildUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleChildUpdaterData);
        {
            const ResParticleChildUpdater& child = ResDynamicCast<ResParticleChildUpdater>(*src);
            NW_ASSERT(child.IsValid());

            childStrSize = strlen(child.GetParticleSetPath()) + 1;

            const ResParticleForm& form = child.GetParticleForm();
            const ResParticleChildUpdaterOption& option = child.GetTiming();

            if (form.IsValid())
            {
                switch (form.GetTypeInfo())
                {
                case ResParticleForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleFormData);
                    break;
                case ResParticleCubeForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleCubeFormData);
                    break;
                case ResParticleCylinderForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleCylinderFormData);
                    break;
                case ResParticleDiscForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleDiscFormData);
                    break;
                case ResParticlePointForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticlePointFormData);
                    break;
                case ResParticleRectangleForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleRectangleFormData);
                    break;
                case ResParticleSphereForm::TYPE_INFO:
                    childFormSize = sizeof(ResParticleSphereFormData);
                    break;
                default:
                    NW_FATAL_ERROR("unknown form type");
                }
            }

            if (option.IsValid())
            {
                switch (option.GetTypeInfo())
                {
                case ResParticleChildUpdaterFinalUpdateOption::TYPE_INFO:
                    childOptionSize = sizeof(ResParticleChildUpdaterFinalUpdateOptionData);
                    break;
                case ResParticleChildUpdaterFirstUpdateOption::TYPE_INFO:
                    childOptionSize = sizeof(ResParticleChildUpdaterFirstUpdateOptionData);
                    break;
                case ResParticleChildUpdaterIntervalOption::TYPE_INFO:
                    childOptionSize = sizeof(ResParticleChildUpdaterIntervalOptionData);
                    break;
                default:
                    NW_FATAL_ERROR("unknown updateOption type");
                }
            }
        }
        break;
    case ResParticleUserUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleUserUpdaterData);
        break;
    case ResParticleVector2ImmediateUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector2ImmediateUpdaterData);
        break;
    case ResParticleVector3Updater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector3UpdaterData);
        break;
    case ResParticleFloatUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleFloatUpdaterData);
        break;
    case ResParticleRotateUpVectorUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleRotateUpVectorUpdaterData);
        break;
    case ResParticleTexturePatternUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleTexturePatternUpdaterData);
        break;
    case ResParticleVector3ImmediateUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector3ImmediateUpdaterData);
        break;
    case ResParticleVector3AdditiveUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector3AdditiveUpdaterData);
        break;
    case ResParticleVector3RandomAdditiveUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleVector3RandomAdditiveUpdaterData);
        break;
    case ResParticleFloatImmediateUpdater::TYPE_INFO:
        baseSize = sizeof(ResParticleFloatImmediateUpdaterData);
        break;
    default:
        NW_FATAL_ERROR("unknown updater type");
        break;
    }

    int animationSize = 0;
    int enabledSize = 0;
    int dataSize = 0;
    int animationOptionSize = 0;
    {
        const ResParticleAnimation& resAnimation = src->GetParticleAnimation();
        if (resAnimation.IsValid())
        {
            animationSize += sizeof(ResParticleAnimationData);

            enabledSize = resAnimation.GetAnimationEnabledCount() * sizeof(bool);
            dataSize = resAnimation.GetAnimationDataCount() * sizeof(s32);

            const ResParticleAnimationOption& option = resAnimation.GetParticleAnimationOption();
            if (option.IsValid())
            {
                switch (option.GetTypeInfo())
                {
                case ResParticleAnimationOption::TYPE_INFO:
                    animationOptionSize = sizeof(ResParticleAnimationOptionData);
                    break;
                case ResParticleFittingAnimationOption::TYPE_INFO:
                    animationOptionSize = sizeof(ResParticleFittingAnimationOptionData);
                    break;
                case ResParticleFrameLoopAnimationOption::TYPE_INFO:
                    animationOptionSize = sizeof(ResParticleFrameLoopAnimationOptionData);
                    break;
                case ResParticleRandomAnimationOption::TYPE_INFO:
                    animationOptionSize = sizeof(ResParticleRandomAnimationOptionData);
                    break;
                default:
                    NW_FATAL_ERROR("unknown animationOption type");
                }
            }

            animationSize += resAnimation.GetAnimationEnabledCount() * sizeof(u32);
            animationSize += resAnimation.GetAnimationDataCount() * sizeof(u32);
        }
    }

    size += ut::RoundUp(baseSize, 4);
    size += ut::RoundUp(animationSize, 4);
    size += ut::RoundUp(enabledSize, 4);
    size += ut::RoundUp(dataSize, 4);
    size += ut::RoundUp(animationOptionSize, 4);
    size += ut::RoundUp(childStrSize, 4);
    size += ut::RoundUp(childFormSize, 4);
    size += ut::RoundUp(childOptionSize, 4);

    void* memory = allocator->Alloc(size, 4);
    if (memory == NULL)
    {
        return NULL;
    }

    u8* dstPtr = (u8*)memory;

    ResParticleUpdaterData* dstData = reinterpret_cast<ResParticleUpdaterData*>(dstPtr);

    {
        const ResParticleUpdaterData* srcData = src->ptr();
        ::nw::os::MemCpy(dstPtr, srcData, baseSize);
        dstPtr += ut::RoundUp(baseSize, 4);
    }

    if (animationSize != 0)
    {
        const ResParticleAnimation& animation = src->GetParticleAnimation();

        ResParticleAnimationData* dstAnimation = reinterpret_cast<ResParticleAnimationData*>(dstPtr);

        ::nw::os::MemCpy(dstPtr, animation.ptr(), animationSize);
        dstData->toParticleAnimation.set_ptr(dstPtr);
        dstPtr += ut::RoundUp(animationSize, 4);

        if (enabledSize > 0)
        {
            ::nw::os::MemCpy(dstPtr, animation.GetAnimationEnabled(), enabledSize);
            dstAnimation->toAnimationEnabledTable.set_ptr(dstPtr);
            dstPtr += ut::RoundUp(enabledSize, 4);
        }

        if (dataSize > 0)
        {
            ::nw::os::MemCpy(dstPtr, animation.GetAnimationData(), dataSize);
            dstAnimation->toAnimationDataTable.set_ptr(dstPtr);
            dstPtr += ut::RoundUp(dataSize, 4);
        }

        if (animationOptionSize != 0)
        {
            const ResParticleAnimationOption& option = animation.GetParticleAnimationOption();

            ::nw::os::MemCpy(dstPtr, option.ptr(), animationOptionSize);
            dstAnimation->toParticleAnimationOption.set_ptr(dstPtr);
            dstPtr += ut::RoundUp(animationOptionSize, 4);
        }
    }

    switch (src->GetTypeInfo())
    {
    case ResParticleChildUpdater::TYPE_INFO:{
            ResParticleChildUpdaterData* childData = reinterpret_cast<ResParticleChildUpdaterData*>(memory);

            const ResParticleChildUpdater& child = ResDynamicCast<ResParticleChildUpdater>(*src);

            if (childStrSize > 0)
            {
                const char* str = child.GetParticleSetPath();

                ::nw::os::MemCpy(dstPtr, str, childStrSize);
                childData->toParticleSetPath.set_ptr((const char*)dstPtr);
                dstPtr += ut::RoundUp(childStrSize, 4);
            }

            if (childFormSize > 0)
            {
                const ResParticleForm& form = child.GetParticleForm();

                ::nw::os::MemCpy(dstPtr, form.ptr(), childFormSize);
                childData->toParticleForm.set_ptr(dstPtr);
                dstPtr += ut::RoundUp(childFormSize, 4);
            }

            if (childOptionSize > 0)
            {
                const ResParticleChildUpdaterOption& option = child.GetTiming();

                ::nw::os::MemCpy(dstPtr, option.ptr(), childOptionSize);
                childData->toTiming.set_ptr(dstPtr);
                dstPtr += ut::RoundUp(childOptionSize, 4);
            }
        }
        break;
    }

    return reinterpret_cast<ResParticleUpdaterData*>(memory);
}

}
}