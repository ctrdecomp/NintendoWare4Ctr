// Filename: gfx_Fog.cpp
//
// Project: NintendoWare4Ctr

#include <nw/os/os_Memory.h>

#include <nw/gfx/gfx_Fog.h>
#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/gfx_Camera.h>

#include <nw/ut/ut_Float24.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(Fog, TransformNode);
const float Fog::FOG_DENSITY = 0.0f;
const ResFogUpdater::FogUpdaterType Fog::FOG_UPDATER_TYPE = ResFogUpdater::FOG_UPDATER_TYPE_NONE;
const float Fog::FOG_MAX_DEPTH = 0.0f;
const float Fog::FOG_MIN_DEPTH = 0.0f;

Fog* Fog::DynamicBuilder::Create(nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    ResPtr resource(CreateResFog(allocator), ResFogDataDestroyer(allocator));

    void* memory = allocator->Alloc(sizeof(Fog));
    NW_NULL_ASSERT(memory);
    Fog* fog = new(memory) Fog(allocator, resource, m_Description);
    Result result = fog->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    return fog;
}

size_t Fog::DynamicBuilder::GetMemorySize(size_t alignment) const
{
    NW_ASSERT(this->m_Description.isFixedSizeMemory);

    nw::os::MemorySizeCalculator size(alignment);

    size += sizeof(ResFogData);
    size += sizeof(ResImageLookupTableData);
    size += sizeof(ResFogUpdaterData);
    size += sizeof(u32) * NW_FOG_TABLE_COMMAND_NUM;

    size += sizeof(Fog);

    TransformNode::GetMemorySizeForInitialize(&size, ResTransformNode(), m_Description);

    size += sizeof(ResFogUpdaterData);

    size += sizeof(ResFogData);

    return size.GetSizeWithPadding(alignment);
}

void Fog::Accept(ISceneVisitor* visitor)
{
    visitor->VisitFog(this);
    AcceptChildren(visitor);
}

Fog* Fog::Create(SceneNode* parent, ResSceneObject resource, const Fog::Description& description, nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    ResFog resNode = ResDynamicCast<ResFog>(resource);
    NW_ASSERT(resNode.IsValid());
    NW_ASSERT(internal::ResCheckRevision(resNode));

    void* memory = allocator->Alloc(sizeof(Fog));
    NW_NULL_ASSERT(memory);

    Fog* fog = new(memory) Fog(allocator, resNode, description);
    Result result = fog->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    if (parent)
    {
        bool isAttached = parent->AttachChild(fog);
        NW_ASSERT(isAttached);
    }

    return fog;
}

ResFogData* Fog::CreateResFog(nw::os::IAllocator* allocator, const char* name /* = NULL */)
{
    ResFogData* resFog = AllocateAndFillN<ResFogData>(allocator, sizeof(ResFogData), 0);

    resFog->typeInfo = ResFog::TYPE_INFO;
    resFog->m_Header.revision = ResFog::BINARY_REVISION;
    resFog->m_Header.signature = ResFog::SIGNATURE;

    resFog->m_UserDataDicCount = 0;
    resFog->toUserDataDic.set_ptr(NULL);

    resFog->toName.set_ptr(AllocateAndCopyString(name, allocator, MAX_NAME_LENGTH));

    resFog->m_ChildrenTableCount = 0;
    resFog->toChildrenTable.set_ptr(NULL);
    resFog->m_AnimGroupsDicCount = NULL;
    resFog->toAnimGroupsDic.set_ptr(NULL);

    const math::VEC3 scale(1.0f, 1.0f, 1.0f);
    const math::VEC3 rotate(0.0f, 0.0f, 0.0f);
    const math::VEC3 translate(0.0f, 0.0f, 0.0f);
    resFog->m_Transform = math::Transform3(scale, rotate, translate);
    resFog->m_WorldMatrix = math::MTX34::Identity();
    ResTransformNode(resFog).SetBranchVisible(true);

    ResImageLookupTableData* fogSampler = AllocateAndFill<ResImageLookupTableData>(allocator, 0);

    ResFogUpdaterData* fogUpdater = AllocateAndFill<ResFogUpdaterData>(allocator, 0);

    resFog->toFogUpdater.set_ptr(fogUpdater);
    resFog->toFogSampler.set_ptr(fogSampler);

    fogSampler->typeInfo = ResImageLookupTable::TYPE_INFO;

    void* tableMemory = allocator->AllocAndFill<u32>(NW_FOG_TABLE_COMMAND_NUM, 0);
    fogSampler->m_CommandCacheTableCount = NW_FOG_TABLE_COMMAND_SIZE;
    fogSampler->toCommandCacheTable.set_ptr(tableMemory);

    return resFog;
}

void Fog::DestroyResFog(nw::os::IAllocator* allocator, ResFogData* resFog)
{
    NW_NULL_ASSERT(allocator);
    NW_NULL_ASSERT(resFog);

    if (resFog->toName.to_ptr() != NULL)
    {
        allocator->Free(const_cast<char*>(resFog->toName.to_ptr()));
    }

    if (resFog->toFogUpdater.to_ptr() != NULL)
    {
        allocator->Free(resFog->toFogUpdater.to_ptr());
    }

    if (resFog->toFogSampler.to_ptr() != NULL)
    {
        ResImageLookupTable fogSampler = ResImageLookupTable(resFog->toFogSampler.to_ptr());
        if (fogSampler.ref().toCommandCacheTable.to_ptr() != NULL)
        {
            allocator->Free(fogSampler.ref().toCommandCacheTable.to_ptr());
        }

        allocator->Free(resFog->toFogSampler.to_ptr());
    }

    allocator->Free(resFog);
}

ResFogUpdaterData* Fog::CreateResFogUpdater(nw::os::IAllocator* allocator)
{
    ResFogUpdaterData* fogUpdater = AllocateAndFill<ResFogUpdaterData>(allocator, 0);

    NW_NULL_ASSERT(fogUpdater);

    return fogUpdater;
}

void Fog::DestroyResFogUpdater(nw::os::IAllocator* allocator, ResFogUpdaterData* resFogUpdater)
{
    NW_NULL_ASSERT(allocator);
    NW_NULL_ASSERT(resFogUpdater);

    allocator->Free(resFogUpdater);
}

void Fog::Update(const Camera* camera)
{
    NW_ASSERT(GetResFog().IsValid());
    ResFogUpdater fogUpdater = GetResFog().GetFogUpdater();
    NW_ASSERT(fogUpdater.IsValid());
    ResImageLookupTable fogSampler = GetResFog().GetFogSampler();
    NW_ASSERT(fogSampler.IsValid());
    NW_ASSERT(fogSampler.GetCommandCacheCount() == NW_FOG_TABLE_COMMAND_SIZE);

    ResCameraProjectionUpdater resProjectionUpdater = camera->GetProjectionUpdater()->GetResource();
    NW_ASSERT(resProjectionUpdater.IsValid());
    f32 near = resProjectionUpdater.GetNear();
    f32 far = resProjectionUpdater.GetFar();
    f32 wscale = camera->GetWScale();

    ResFogUpdaterData* updaterData = m_UpdaterCache.Get();
    if (!ut::FloatEqualsWeak(m_Near, near) ||
        !ut::FloatEqualsWeak(m_Far, far) ||
        !ut::FloatEqualsWeak(m_WScale, wscale) ||
        !ut::FloatEqualsWeak(updaterData->m_Density, fogUpdater.GetDensity()) ||
        !(updaterData->m_FogUpdaterType == fogUpdater.GetFogUpdaterType()) ||
        !ut::FloatEqualsWeak(updaterData->m_MaxFogDepth, fogUpdater.GetMaxFogDepth()) ||
        !ut::FloatEqualsWeak(updaterData->m_MinFogDepth, fogUpdater.GetMinFogDepth()))
        {
        m_Near = near;
        m_Far = far;
        m_WScale = wscale;
        updaterData->m_Density = fogUpdater.GetDensity();
        updaterData->m_FogUpdaterType = fogUpdater.GetFogUpdaterType();
        updaterData->m_MaxFogDepth = fogUpdater.GetMaxFogDepth();
        updaterData->m_MinFogDepth = fogUpdater.GetMinFogDepth();

        SetupFogSampler(fogSampler, fogUpdater, camera->InverseProjectionMatrix());
    }
}

void Fog::SetupFogSampler(ResImageLookupTable fogSampler, ResFogUpdater fogUpdater, const math::MTX44& inverseProjectionMatrix)
{
    const uint HALF_TABLE_SIZE = FOG_TABLE_SIZE / 2;

    enum
    {
        REG_FOG_TABLE_INDEX = 0xe6,
        REG_FOG_TABLE_PARAM = 0xe8
    };

    const u32 HEADER_FOG_TABLE_INDEX = internal::MakeCommandHeader(REG_FOG_TABLE_INDEX, 1, false, 0xF);
    const u32 HEADER_FOG_TABLE_PARAM = internal::MakeCommandHeader(REG_FOG_TABLE_PARAM, 128, false, 0xF);

    fogSampler.SetCommandCache(0, 0);
    fogSampler.SetCommandCache(1, HEADER_FOG_TABLE_INDEX);
    fogSampler.SetCommandCache(3, HEADER_FOG_TABLE_PARAM);

    uint index = 0;
    math::VEC4 viewPos;
    f32 prevLut = 0.0f;

    for (int i = 0; i <= HALF_TABLE_SIZE; ++i)
    {
        if (m_WScale == 0.0f)
        {
            f32 depth = -(f32)i / (f32)(HALF_TABLE_SIZE);
            viewPos.w = inverseProjectionMatrix.f._32 * depth + inverseProjectionMatrix.f._33;
            viewPos.z = -(inverseProjectionMatrix.f._22 * depth + inverseProjectionMatrix.f._23) / viewPos.w;
        }
        else
        {
            viewPos.z = ((f32)i / (f32)HALF_TABLE_SIZE) * (m_Far - m_Near) + m_Near;
        }

        if (viewPos.z <= fogUpdater.GetMinFogDepth())
        {
            viewPos.z = 1.0f;
        }
        else if (viewPos.z > fogUpdater.GetMaxFogDepth())
        {
            viewPos.z = 0.0f;
        }
        else
        {
            if (fogUpdater.GetFogUpdaterType() == ResFogUpdater::FOG_UPDATER_TYPE_LINEAR)
            {
                viewPos.z = (fogUpdater.GetMaxFogDepth() - viewPos.z) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth());
            }
            else if (fogUpdater.GetFogUpdaterType() == ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT)
            {
                viewPos.z = math::FExp(
                    -fogUpdater.GetDensity() * (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth()));
            }
            else if (fogUpdater.GetFogUpdaterType() == ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT_SQUARE)
            {
                f32 viewDepth = fogUpdater.GetDensity() *
                    (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth());
                viewPos.z = math::FExp(-viewDepth * viewDepth);
            }
            else{
                NW_ASSERTMSG(false, "Unknown FogUpdater type.");
            }
        }

        if (i > 0)
        {
            u32 value = nw::ut::Fixed13::Float32ToFixed13(viewPos.z - prevLut) | (nw::ut::Fixed11::Float32ToFixed11(prevLut) << 13);
            u32 commandIndex = (index == 0) ? 2 : index + 3;

            fogSampler.SetCommandCache(commandIndex, value);

            ++index;
        }

        prevLut = viewPos.z;
    }
}

Result Fog::Initialize(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    result |= TransformNode::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);

    ResFogUpdaterData* resFogUpdater = CreateResFogUpdater(allocator);
    if (resFogUpdater != NULL)
    {
        m_UpdaterCache = ResUpdaterPtr(resFogUpdater, ResFogUpdaterDataDestroyer(allocator));
    }
    else{
        result |= Result::MASK_FAIL_BIT;
    }
    NW_ENSURE_AND_RETURN(result);

    result |= CreateOriginalValue(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateAnimGroup(allocator);
    NW_ENSURE_AND_RETURN(result);

    return result;
}

Result Fog::CreateAnimGroup(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    AnimBinding* animBinding = GetAnimBinding();
    if (animBinding == NULL)
    {
        return result;
    }

    ResFog resFog = GetResFog();
    NW_ASSERT(resFog.IsValid());

    NW_ASSERT(resFog.GetAnimGroupsCount() == 1);
    anim::ResAnimGroup resAnimGroup = resFog.GetAnimGroups(0);

    NW_ASSERT(resAnimGroup.GetTargetType() == anim::ResGraphicsAnimGroup::TARGET_TYPE_FOG);

    AnimGroup* animGroup = AnimGroup::Builder().ResAnimGroup(resAnimGroup).SetSceneNode(this).UseOriginalValue(true).Create(allocator);

    if (animGroup == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
    }

    NW_ENSURE_AND_RETURN(result);

    this->BindAnim(animGroup);

    animBinding->SetAnimGroup(0, animGroup);
    m_AnimGroup = animGroup;

    return result;
}

Result Fog::CreateOriginalValue(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    void* buffer = allocator->Alloc(sizeof(ResFogData));
    NW_NULL_ASSERT(buffer);

    ResFogData* originalValue = new(buffer) ResFogData(GetResFog().ref());
    m_OriginalValue = ResFog(originalValue);

    return result;
}

void Fog::BindAnim(AnimGroup* animGroup)
{
    using namespace anim;

    const int animMemberCount = animGroup->GetMemberCount();
    for (int memberIdx = 0; memberIdx < animMemberCount; ++memberIdx)
    {
        anim::ResAnimGroupMember resAnimGroupMember = animGroup->GetResAnimGroupMember(memberIdx);

        switch (resAnimGroupMember.GetObjectType())
        {
        case anim::ResAnimGroupMember::OBJECT_TYPE_FOG:{
                void* object = GetResFog().ptr();
                animGroup->SetTargetObject(memberIdx, object);
                u8* target = static_cast<u8*>(object);
                target += resAnimGroupMember.GetMemberOffset();

                animGroup->SetTargetPtr(memberIdx, target);

                u8* originalValue = reinterpret_cast<u8*>(m_OriginalValue.ptr());
                originalValue += resAnimGroupMember.GetMemberOffset();
                animGroup->SetOriginalValue(memberIdx, originalValue);
            }
            break;
        default:{
                NW_FATAL_ERROR("Unknown animation member type");
            }
            break;
        }

        animGroup->SetTargetObjectIndex(memberIdx, 0);
    }
}

void Fog::GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResFog resFog, Description description)
{
    NW_ASSERT(description.isFixedSizeMemory);

    nw::os::MemorySizeCalculator& size = *pSize;

    size += sizeof(Fog);

    TransformNode::GetMemorySizeForInitialize(&size, resFog, description);

    size += sizeof(ResFogUpdaterData);

    if (description.isAnimationEnabled && resFog.GetAnimGroupsCount() > 0)
    {
        AnimGroup::Builder().ResAnimGroup(resFog.GetAnimGroups(0)).UseOriginalValue(true).GetMemorySizeInternal(&size);
    }

    size += sizeof(ResFogData);
}

}
}