// Filename: gfx_MeshRenderer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/res/gfx_ResMesh.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/gfx_MeshRenderer.h>
#include <nw/gfx/gfx_RenderContext.h>
#include <nw/gfx/gfx_ShaderProgram.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/gfx_ParticleModel.h>
#include <nw/gfx/gfx_ParticleCollection.h>
#include <nw/gfx/gfx_ParticleShape.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/os/os_Memory.h>
#include <nw/types.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_Foreach.h>
#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/gfx/gfx_ActivateCommand.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

#include <cmath>

namespace nw{
namespace gfx{

namespace internal{

bool TestRegisterOverride(int matrixpaletteCount, RenderContext* renderContext)
{
    NW_ASSERT(renderContext->GetShaderProgram()->GetActiveDescription().IsValid());

    int vertexLightEndUniform = renderContext->GetShaderProgram()->GetActiveDescription().GetVertexLightEndUniform();
    int vertexLightCount = renderContext->GetSceneEnvironment().GetVertexLightCount();
    const int UNIT_COUNT = 3;
    const int VERTEX_LIGHT_REGISTER_COUNT = 6;
    int usedMatrixpaletteCount = 0;
    usedMatrixpaletteCount = UNIT_COUNT * matrixpaletteCount + vertexLightCount * VERTEX_LIGHT_REGISTER_COUNT;

    return (usedMatrixpaletteCount <= vertexLightEndUniform);
}

}

/* MeshRenderer */

MeshRenderer* MeshRenderer::Create(nw::os::IAllocator* pAllocator)
{
    NW_NULL_ASSERT(pAllocator);

    size_t size = sizeof(MeshRenderer);

    void* buf = pAllocator->Alloc(size);

    return new(buf) MeshRenderer(pAllocator);
}

void MeshRenderer::RenderMesh(ResMesh mesh, Model* model)
{
    NW_ASSERT(mesh.IsValid());

    ResShape shape = model->GetResModel().GetShapes(mesh.GetShapeIndex());

    switch (shape.ref().typeInfo)
    {
    case ResSeparateDataShape::TYPE_INFO:{
            this->m_RenderContext->SetMaterial(model->GetMaterial(mesh.GetMaterialIndex()));
            this->m_RenderContext->ActivateContext();
            this->m_RenderContext->ActivateVertexAttribute(mesh);
            this->RenderSeparateDataShape(model, ResStaticCast<ResSeparateDataShape>(shape), mesh.GetCurrentPrimitiveIndex());
            this->m_RenderContext->DeactivateVertexAttribute(mesh);
        }
        break;

    case ResParticleShape::TYPE_INFO:{
            ParticleModel* particleModel = static_cast<ParticleModel*>(model);
            NW_NULL_ASSERT(particleModel);

            ParticleSet* particleSet = particleModel->GetParticleSets(mesh.GetShapeIndex());

            if (particleSet->GetParticleCollection()->GetCount() == 0)
            {
                return;
            }

            this->m_RenderContext->SetMaterial(model->GetMaterial(mesh.GetMaterialIndex()));
            this->m_RenderContext->ActivateParticleContext();
            this->RenderParticleShape(model, ResStaticCast<ResParticleShape>(shape), mesh.GetShapeIndex());
        }
        break;
    default:{
            NW_FATAL_ERROR("Unsupported data shape type.");
        }
    }
}

void MeshRenderer::RenderSeparateDataShape(Model* model, ResSeparateDataShape shape, s32 currentPrimitiveIndex)
{
    NW_NULL_ASSERT(this->m_RenderContext);
    if (!shape.IsValid())
    {
        return;
    }

    internal::NWSetVertexUniform3fv(VERTEX_SHADER_UNIFORM_POSOFFS_INDEX, 1, shape.GetPositionOffset());

    SkeletalModel* skeletalModel = ut::DynamicCast<SkeletalModel*>(model);

    if (this->m_RenderContext->GetModelCache() != model)
    {
        if (skeletalModel)
        {
            this->m_RenderContext->SetModelMatrixForSkeletalModel(skeletalModel);
        }
        else{
            this->m_RenderContext->SetModelMatrixForModel(model);
        }
    }

    ResPrimitiveSetArray primitiveSets = shape.GetPrimitiveSets();
    ResPrimitiveSetArray::iterator primitiveSetEnd = primitiveSets.end();

    for (ResPrimitiveSetArray::iterator primitiveSet = primitiveSets.begin(); primitiveSet != primitiveSetEnd; ++primitiveSet)
    {
        NW_ASSERT((*primitiveSet).IsValid());
        NW_ASSERT(0 <= currentPrimitiveIndex && currentPrimitiveIndex < (*primitiveSet).GetPrimitivesCount());

        s32 boneIndexCount = (*primitiveSet).GetBoneIndexTableCount();

        NW_ASSERTMSG(internal::TestRegisterOverride(boneIndexCount, this->m_RenderContext), "Vertex-lights or user-registers might be overridden by bone matrices.%s", model->GetResModel().GetName());

        if (skeletalModel == NULL || boneIndexCount == 0)
        {
            const ShaderProgram* shaderProgram = m_RenderContext->GetShaderProgram();

            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISSMOSK), false);
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISRGDSK), false);

            shaderProgram->SetUniversal(0, model->WorldMatrix());
        }
        else{
            SetMatrixPalette(skeletalModel, *primitiveSet, boneIndexCount);
        }

        this->m_RenderContext->RenderPrimitive((*primitiveSet).GetPrimitives(currentPrimitiveIndex));
    }
}

namespace internal{
static bool isIllegal(f32 val)
{
    return !isfinite(val);
}

static bool isIllegal(const math::VEC3& vec)
{
    return isIllegal(vec.x) || isIllegal(vec.y) || isIllegal(vec.z);
}

static bool isIllegal(const math::MTX34& mtx)
{
    return
        isIllegal(mtx._00) || isIllegal(mtx._01) || isIllegal(mtx._02) || isIllegal(mtx._03) ||
        isIllegal(mtx._10) || isIllegal(mtx._11) || isIllegal(mtx._12) || isIllegal(mtx._13) ||
        isIllegal(mtx._20) || isIllegal(mtx._21) || isIllegal(mtx._22) || isIllegal(mtx._23);
}
}

void MeshRenderer::RenderParticleShape(Model* model, ResParticleShape resource, int shapeIndex)
{
    NW_NULL_ASSERT(this->m_RenderContext);
    if (!resource.IsValid())
    {
        return;
    }

    ParticleModel* particleModel = static_cast<ParticleModel*>(model);
    NW_NULL_ASSERT(particleModel);

    ParticleSet* particleSet = particleModel->GetParticleSets(shapeIndex);

    const ResParticleSet& resParticleSet = particleSet->GetResParticleSet();

    ParticleShape* particleShape = particleModel->GetParticleShapes(shapeIndex);
    NW_NULL_ASSERT(particleShape);

    NW_ASSERT(particleShape->GetResParticleShape().ptr() == resource.ptr());

    if (resParticleSet.GetIsBufferFlushEnabled())
    {
        particleShape->FlushBuffer();
    }

    int bufferSide = particleShape->GetBufferSide();
    internal::NWUseCmdlist(particleShape->m_CommandCache[bufferSide], particleShape->m_CommandCacheSize[bufferSide]);

    enum
    {
        REG_UNIFORM_FLOAT_INDEX = 0x2c0,
        REG_VS_FLOAT_DATA1 = 0x2c1,
        REG_INDEX_STREAM_OFFSET = 0x227,
        REG_INDEX_STREAM_COUNT = 0x228
    };

    u32* command = (u32*)internal::NWGetCurrentCmdBuffer();
    int commandIndex = 0;

    command[commandIndex++] = 0x80000000 | VERTEX_SHADER_UNIFORM_POSOFFS_INDEX;
    command[commandIndex++] = internal::MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1 + 4, true, 0xF);
    command[commandIndex++] = 0;
    command[commandIndex++] = *(u32*)&resource.GetPositionOffset().z;
    command[commandIndex++] = *(u32*)&resource.GetPositionOffset().y;
    command[commandIndex++] = *(u32*)&resource.GetPositionOffset().x;
    NW_ASSERT(!internal::isIllegal(resource.GetPositionOffset()));

    Camera* camera = this->m_RenderContext->GetActiveCamera();
    NW_NULL_ASSERT(camera);

    const u32 HEADER_UNIFORM_FLOAT_INDEX = internal::MakeCommandHeader(REG_UNIFORM_FLOAT_INDEX, 1, false, 0xF);

    command[commandIndex++] = 0x80000000 | VERTEX_SHADER_UNIFORM_UNIVREG_INDEX;
    command[commandIndex++] = HEADER_UNIFORM_FLOAT_INDEX;

    const int universalNum = 15;

    if (particleSet->GetResParticleSet().GetIsForceWorld())
    {
        NW_ASSERT(!internal::isIllegal(camera->ViewMatrix()));
        internal::NWCopyMtx34WithHeader(
            (f32*)&command[commandIndex],
            (f32*)&camera->ViewMatrix(),
            internal::MakeCommandHeader(REG_VS_FLOAT_DATA1, 4 * universalNum, false, 0xF));
        commandIndex += 12 + 1;

        NW_ASSERT(!internal::isIllegal(camera->InverseViewMatrix()));
        internal::NWCopyMtx34Reverse((f32*)&command[commandIndex], (f32*)&camera->InverseViewMatrix());
        commandIndex += 12;

        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0x3f800000;

        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0x3f800000;
        command[commandIndex++] = 0;

        command[commandIndex++] = 0;
        command[commandIndex++] = 0x3f800000;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0;

        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0x3f800000;

        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0x3f800000;
        command[commandIndex++] = 0;

        command[commandIndex++] = 0;
        command[commandIndex++] = 0x3f800000;
        command[commandIndex++] = 0;
        command[commandIndex++] = 0;
    }
    else
    {
        nw::math::MTX34* worldMatrix;

        worldMatrix = &model->WorldMatrix();

        nw::math::MTX34 modelView;
        nw::math::MTX34Mult(&modelView, camera->ViewMatrix(), *worldMatrix);
        NW_ASSERT(!internal::isIllegal(modelView));
        internal::NWCopyMtx34WithHeader(
            (f32*)&command[commandIndex],
            (f32*)&modelView,
            internal::MakeCommandHeader(REG_VS_FLOAT_DATA1, 4 * universalNum, false, 0xF));
        commandIndex += 12 + 1;

        nw::math::MTX34 invModelView;

        nw::math::MTX34Mult(&invModelView, model->InverseWorldMatrix(), camera->InverseViewMatrix());

        NW_ASSERT(!internal::isIllegal(invModelView));
        internal::NWCopyMtx34Reverse((f32*)&command[commandIndex], (f32*)&invModelView);
        commandIndex += 12;

        NW_ASSERT(!internal::isIllegal(model->InverseWorldMatrix()));
        internal::NWCopyMtx34Reverse((f32*)&command[commandIndex], (f32*)&model->InverseWorldMatrix());
        commandIndex += 12;

        NW_ASSERT(!internal::isIllegal(model->WorldMatrix()));
        internal::NWCopyMtx34Reverse((f32*)&command[commandIndex], (f32*)worldMatrix);
        commandIndex += 12;
    }

    math::VEC3 offset(0.0f, 0.0f, 0.0f);
    const ResParticleShapeBuilder& resShapeBuilder = resParticleSet.GetParticleShapeBuilder();
    if (resShapeBuilder.IsValid())
    {
        offset = resShapeBuilder.GetDrawOffset();
    }

    NW_ASSERT(!internal::isIllegal(offset));
    internal::NWCopyVec3Reverse((f32*)&command[commandIndex], (f32*)&offset);
    commandIndex += 4;

    NW_ASSERT(!internal::isIllegal(particleSet->GetScaleOffset()));
    internal::NWCopyVec3Reverse((f32*)&command[commandIndex], (f32*)&particleSet->GetScaleOffset());
    commandIndex += 4;

    NW_ASSERT(!internal::isIllegal(particleSet->GetRotateOffset()));
    internal::NWCopyVec3Reverse((f32*)&command[commandIndex], (f32*)&particleSet->GetRotateOffset());
    commandIndex += 4;

    if ((commandIndex & 1) == 1)
    {
        command[commandIndex++] = 0;
    }

    const u32 HEADER_INDEX_STREAM_COUNT = internal::MakeCommandHeader(REG_INDEX_STREAM_COUNT, 1, false, 0xF);

    command[commandIndex++] = particleSet->GetParticleCollection()->GetCount();
    command[commandIndex++] = HEADER_INDEX_STREAM_COUNT;

    u32 streamOffset = particleShape->GetPrimitiveStreamOffset(PARTICLE_BUFFER_FRONT);

    const bool isAscendingOrder = (resShapeBuilder.IsValid()) ? resShapeBuilder.IsAscendingOrder() : true;
    if (!isAscendingOrder)
    {
        ParticleCollection* collection = particleSet->GetParticleCollection();
        streamOffset += sizeof(u16) * (collection->GetCapacity() - collection->GetCount());
    }

    const u32 HEADER_INDEX_STREAM_OFFSET = internal::MakeCommandHeader(REG_INDEX_STREAM_OFFSET, 1, false, 0xF);

    command[commandIndex++] = 0x80000000 | streamOffset;
    command[commandIndex++] = HEADER_INDEX_STREAM_OFFSET;

    internal::NWForwardCurrentCmdBuffer(sizeof(u32) * commandIndex);

    internal::NWUseCmdlist(particleShape->m_PrimitiveCommandCache, particleShape->m_PrimitiveCommandCacheSize);

    internal::NWUseCmdlist(particleShape->m_DeactivateVertexCommandCache, particleShape->m_DeactivateVertexCommandCacheSize);
}

void MeshRenderer::SetMatrixPalette(SkeletalModel* skeletalModel, ResPrimitiveSet primitiveSet, s32 boneIndexCount)
{
    NW_NULL_ASSERT(m_RenderContext);
    NW_ASSERT(primitiveSet.IsValid());

    const int UNIT_COUNT = 3;

    const ShaderProgram* shaderProgram = m_RenderContext->GetShaderProgram();

    bool isRigid = primitiveSet.GetSkinningMode() != ResPrimitiveSet::SKINNING_MODE_SMOOTH;
    shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISSMOSK), !isRigid);
    shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISRGDSK), isRigid);

    Skeleton* skeleton = skeletalModel->GetSkeleton();
    ResSkeleton resSkeleton = skeleton->GetResSkeleton();
    Skeleton::MatrixPose& matrixPose = skeleton->WorldMatrixPose();
    Skeleton::MatrixPose& skiningPose = skeleton->SkiningMatrixPose();

    for (int count = 0; count < boneIndexCount; ++count)
    {
        s32 boneIndex = primitiveSet.GetBoneIndexTable(count);

        ResBone bone = resSkeleton.GetBones(boneIndex);
        bool hasSkinningMatrix = (bone.GetFlags() & ResBoneData::FLAG_HAS_SKINNING_MATRIX) != 0;
        Skeleton::MatrixPose* pose = (!isRigid && hasSkinningMatrix) ? &skiningPose : &matrixPose;

        math::MTX34* matrix;

        matrix = pose->GetMatrix(boneIndex);

        int index = count * UNIT_COUNT;

        if (count == 0)
        {
            internal::NWSetVertexUniform4fvBegin(VERTEX_SHADER_UNIFORM_UNIVREG_INDEX + index, boneIndexCount * UNIT_COUNT, UNIT_COUNT, *matrix);
        }
        else{
            internal::NWSetVertexUniform4fvContinuous(UNIT_COUNT, *matrix);
        }
    }

    internal::NWSetVertexUniform4fvEnd();
}

}
}