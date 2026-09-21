#include <nw/gfx/res/gfx_ResCamera.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/gfx_AimTargetViewUpdater.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_LookAtTargetViewUpdater.h>
#include <nw/gfx/gfx_RotateViewUpdater.h>
#include <nw/gfx/gfx_PerspectiveProjectionUpdater.h>
#include <nw/gfx/gfx_OrthoProjectionUpdater.h>
#include <nw/gfx/gfx_FrustumProjectionUpdater.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(Camera, TransformNode);

Camera::Camera(
    nw::os::IAllocator* allocator,
    ResTransformNode resObj,
    const Camera::Description& description,
    GfxPtr<CameraViewUpdater> viewUpdater,
    GfxPtr<CameraProjectionUpdater> projectionUpdater,
    f32 wscale,
    bool isDynamic):
    TransformNode(allocator, resObj, description),
    m_ViewMatrix(math::MTX34::Identity()),
    m_InverseViewMatrix(math::MTX34::Identity()),
    m_ProjectionMatrix(math::MTX44::Identity()),
    m_InverseProjectionMatrix(math::MTX44::Identity()),
    m_TextureProjectionMatrix(math::MTX34::Identity()),
    m_ViewUpdater(viewUpdater),
    m_ProjectionUpdater(projectionUpdater),
    m_AnimGroup(NULL),
    m_OriginalValue(),
    m_WScale(wscale),
    m_IsDynamic(isDynamic) {}

Camera::~Camera()
{
    ResCamera resCamera = ResDynamicCast<ResCamera>(GetResSceneNode());
    if (m_IsDynamic && resCamera.IsValid())
    {
        GetAllocator().Free(resCamera.ptr());
    }

    ResCameraDestroyer(&GetAllocator());
    m_OriginalValue = ResCamera(NULL);

    ut::SafeDestroy(m_AnimGroup);
}

Result Camera::CreateAnimGroup(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    NW_ASSERT(m_OriginalValue.IsValid());

    if (!m_AnimBinding)
    {
        return result;
    }

    ResCamera resCamera = GetResCamera();
    NW_ASSERT(resCamera.IsValid());

    NW_ASSERT(resCamera.GetAnimGroupsCount() == 1);
    anim::ResAnimGroup resAnimGroup = resCamera.GetAnimGroups(0);

    NW_ASSERT(resAnimGroup.GetTargetType() == anim::ResGraphicsAnimGroup::TARGET_TYPE_CAMERA);

    AnimGroup* animGroup = AnimGroup::Builder()
        .ResAnimGroup(resAnimGroup)
        .SetSceneNode(this)
        .UseOriginalValue(true)
        .Create(allocator);

    if (animGroup == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
    }

    NW_ENSURE_AND_RETURN(result);

    m_AnimGroup = animGroup;

    const int animMemberCount = m_AnimGroup->GetMemberCount();
    for (int memberIdx = 0; memberIdx < animMemberCount; ++memberIdx)
    {
        anim::ResAnimGroupMember member = m_AnimGroup->GetResAnimGroupMember(memberIdx);

        void* object = GetAnimTargetObject(member);
        m_AnimGroup->SetTargetObject(memberIdx, object);

        if (member.GetObjectType() == anim::ResAnimGroupMember::OBJECT_TYPE_TRANSFORM &&
            member.GetMemberType() == anim::ResTransformMember::MEMBER_TYPE_TRANSFORM)
            {
            m_AnimGroup->SetTargetPtr(memberIdx, &Transform());
        }
        else{
            u8* target = static_cast<u8*>(object);
            target += member.GetMemberOffset();
            m_AnimGroup->SetTargetPtr(memberIdx, target);
        }

        m_AnimGroup->SetTargetObjectIndex(memberIdx, 0);

        void* originalValue = NULL;
        switch (member.GetObjectType())
        {
        case anim::ResAnimGroupMember::OBJECT_TYPE_TRANSFORM:{
                switch (member.GetMemberType())
                {
                case anim::ResTransformMember::MEMBER_TYPE_TRANSFORM:
                    originalValue = &m_OriginalTransform;
                    break;
                default:
                    NW_ASSERT(false);
                }
            }
            break;
        case anim::ResAnimGroupMember::OBJECT_TYPE_VIEW_UPDATER:{
                originalValue = ut::AddOffsetToPtr(m_OriginalValue.GetViewUpdater().ptr(), member.GetMemberOffset());
            }
            break;
        case anim::ResAnimGroupMember::OBJECT_TYPE_PROJECTION_UPDATER:{
                originalValue = ut::AddOffsetToPtr(m_OriginalValue.GetProjectionUpdater().ptr(), member.GetMemberOffset());
            }
            break;
        default:
            NW_ASSERT(false);
        }

        NW_NULL_ASSERT(originalValue);
        m_AnimGroup->SetOriginalValue(memberIdx, originalValue);
    }

    m_AnimBinding->SetAnimGroup(0, m_AnimGroup);

    return result;
}

Camera* Camera::DynamicBuilder::Create(nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    if (!m_ViewUpdater)
    {
        m_ViewUpdater.Reset(LookAtTargetViewUpdater::Create(allocator));
    }

    if (!m_ProjectionUpdater)
    {
        m_ProjectionUpdater.Reset(PerspectiveProjectionUpdater::Create(allocator));
    }

    ResCameraData* resCamera = AllocateAndFillN<ResCameraData>(allocator, sizeof(ResCameraData), 0);

    resCamera->toViewUpdater.set_ptr(m_ViewUpdater->GetResource().ptr());
    resCamera->toProjectionUpdater.set_ptr(m_ProjectionUpdater->GetResource().ptr());

    switch (m_ViewUpdater->GetResource().GetTypeInfo())
    {
    case ResAimTargetViewUpdater::TYPE_INFO:
        resCamera->m_ViewType = ResCamera::VIEWTYPE_AIM;
        break;
    case ResLookAtTargetViewUpdater::TYPE_INFO:
        resCamera->m_ViewType = ResCamera::VIEWTYPE_LOOKAT;
        break;
    case ResRotateViewUpdater::TYPE_INFO:
        resCamera->m_ViewType = ResCamera::VIEWTYPE_ROTATE;
        break;
    default:
        NW_ASSERT(false);
    }

    switch (m_ProjectionUpdater->GetResource().GetTypeInfo())
    {
    case ResPerspectiveProjectionUpdater::TYPE_INFO:
        resCamera->m_ProjectionType = ResCamera::PROJTYPE_PERSPECTIVE;
        break;
    case ResFrustumProjectionUpdater::TYPE_INFO:
        resCamera->m_ProjectionType = ResCamera::PROJTYPE_FRUSTUM;
        break;
    case ResOrthoProjectionUpdater::TYPE_INFO:
        resCamera->m_ProjectionType = ResCamera::PROJTYPE_ORTHO;
        break;
    default:
        NW_ASSERT(false);
    }

    resCamera->typeInfo = ResCamera::TYPE_INFO;
    resCamera->m_Header.revision = ResCamera::BINARY_REVISION;
    resCamera->m_Header.signature = ResCamera::SIGNATURE;

    resCamera->m_UserDataDicCount = 0;
    resCamera->toUserDataDic.set_ptr(NULL);

    const char* name = NULL;
    resCamera->toName.set_ptr(AllocateAndCopyString(name, allocator, MAX_NAME_LENGTH));

    resCamera->m_ChildrenTableCount = 0;
    resCamera->toChildrenTable.set_ptr(NULL);
    resCamera->m_AnimGroupsDicCount = 0;
    resCamera->toAnimGroupsDic.set_ptr(NULL);

    const math::VEC3 scale(1.0f, 1.0f, 1.0f);
    const math::VEC3 rotate(0.0f, 0.0f, 0.0f);
    const math::VEC3 translate(0.0f, 0.0f, 0.0f);
    resCamera->m_Transform = math::Transform3(scale, rotate, translate);
    resCamera->m_WorldMatrix = math::MTX34::Identity();
    ResTransformNode(resCamera).SetBranchVisible(true);

    void* memory = allocator->Alloc(sizeof(Camera));
    NW_NULL_ASSERT(memory);

    Camera* camera = new(memory) Camera(allocator, ResCamera(resCamera), m_Description, m_ViewUpdater, m_ProjectionUpdater, 0.0f, true);

    camera->Initialize(allocator);

    return camera;
}

size_t Camera::DynamicBuilder::GetMemorySize(size_t alignment) const
{
    NW_ASSERT(m_Description.isFixedSizeMemory);

    nw::os::MemorySizeCalculator size(alignment);

    ResCamera::ViewType viewType;
    ResCamera::ProjType projType;

    if (!m_ViewUpdater)
    {
        LookAtTargetViewUpdater::GetMemorySizeInternal(&size, true);
        viewType = ResCamera::VIEWTYPE_LOOKAT;
    }
    else{
        switch (m_ViewUpdater->GetResource().GetTypeInfo())
        {
        case ResAimTargetViewUpdater::TYPE_INFO:
            viewType = ResCamera::VIEWTYPE_AIM;
            break;
        case ResLookAtTargetViewUpdater::TYPE_INFO:
            viewType = ResCamera::VIEWTYPE_LOOKAT;
            break;
        case ResRotateViewUpdater::TYPE_INFO:
            viewType = ResCamera::VIEWTYPE_ROTATE;
            break;
        default:
            NW_ASSERT(false);
        }
    }

    if (!m_ProjectionUpdater)
    {
        PerspectiveProjectionUpdater::GetMemorySizeInternal(&size, true);
        projType = ResCamera::PROJTYPE_PERSPECTIVE;
    }
    else{
        switch (m_ProjectionUpdater->GetResource().GetTypeInfo())
        {
        case ResPerspectiveProjectionUpdater::TYPE_INFO:
            projType = ResCamera::PROJTYPE_PERSPECTIVE;
            break;
        case ResFrustumProjectionUpdater::TYPE_INFO:
            projType = ResCamera::PROJTYPE_FRUSTUM;
            break;
        case ResOrthoProjectionUpdater::TYPE_INFO:
            projType = ResCamera::PROJTYPE_ORTHO;
            break;
        default:
            NW_ASSERT(false);
        }
    }

    size += sizeof(ResCameraData);
    size += sizeof(Camera);

    TransformNode::GetMemorySizeForInitialize(&size, res::ResTransformNode(), m_Description);

    size += sizeof(ResCameraData);

    switch (viewType)
    {
    case ResCamera::VIEWTYPE_AIM:
        size += sizeof(ResAimTargetViewUpdaterData);
        break;
    case ResCamera::VIEWTYPE_LOOKAT:
        size += sizeof(ResLookAtTargetViewUpdaterData);
        break;
    case ResCamera::VIEWTYPE_ROTATE:
        size += sizeof(ResRotateViewUpdaterData);
        break;
    default:
        NW_ASSERT(false);
    }

    switch (projType)
    {
    case ResCamera::PROJTYPE_PERSPECTIVE:
        size += sizeof(ResPerspectiveProjectionUpdaterData);
        break;
    case ResCamera::PROJTYPE_FRUSTUM:
        size += sizeof(ResFrustumProjectionUpdaterData);
        break;
    case ResCamera::PROJTYPE_ORTHO:
        size += sizeof(ResOrthoProjectionUpdaterData);
        break;
    default:
        NW_ASSERT(false);
    }

    return size.GetSizeWithPadding(alignment);
}

Camera* Camera::Create(SceneNode* parent, ResSceneObject resource, const Camera::Description& description, nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    ResCamera resCamera = ResDynamicCast<ResCamera>(resource);
    NW_ASSERT(resCamera.IsValid());
    NW_ASSERT(internal::ResCheckRevision(resCamera));

    void* memory = allocator->Alloc(sizeof(Camera));
    NW_NULL_ASSERT(memory);

    ResCamera::ViewType viewType = resCamera.GetViewType();
    ResCameraViewUpdater resViewUpdater = resCamera.GetViewUpdater();
    NW_ASSERT(resViewUpdater.IsValid());
    NW_ASSERT(viewType < ResCamera::VIEWTYPE_COUNT);
    CameraViewUpdater* cameraView = NULL;
    if (viewType == ResCamera::VIEWTYPE_AIM)
    {
        ResAimTargetViewUpdater resAimView(resViewUpdater.ptr());
        AimTargetViewUpdater* aimView = AimTargetViewUpdater::Create(allocator, resAimView);
        cameraView = aimView;
    }
    else if (viewType == ResCamera::VIEWTYPE_LOOKAT)
    {
        ResLookAtTargetViewUpdater resLookAtView(resViewUpdater.ptr());
        LookAtTargetViewUpdater* lookAtView = LookAtTargetViewUpdater::Create(allocator, resLookAtView);

        cameraView = lookAtView;
    }
    else if (viewType == ResCamera::VIEWTYPE_ROTATE)
    {
        ResRotateViewUpdater resRotateView(resViewUpdater.ptr());
        RotateViewUpdater* rotateView = RotateViewUpdater::Create(allocator, resRotateView);

        cameraView = rotateView;
    }
    GfxPtr<CameraViewUpdater> viewUpdater(cameraView);

    ResCamera::ProjType projType = resCamera.GetProjectionType();
    ResCameraProjectionUpdater resProjectionUpdater = resCamera.GetProjectionUpdater();
    NW_ASSERT(resProjectionUpdater.IsValid());
    NW_ASSERT(projType < ResCamera::PROJTYPE_COUNT);
    CameraProjectionUpdater* cameraProjection = NULL;
    if (projType == ResCamera::PROJTYPE_PERSPECTIVE)
    {
        ResPerspectiveProjectionUpdater resPersProjection(resProjectionUpdater.ptr());
        PerspectiveProjectionUpdater* persProjection = PerspectiveProjectionUpdater::Create(allocator, resPersProjection);

        cameraProjection = persProjection;
    }
    else if (projType == ResCamera::PROJTYPE_FRUSTUM)
    {
        ResFrustumProjectionUpdater resFrustumProjection(resProjectionUpdater.ptr());
        FrustumProjectionUpdater* frustumProjection = FrustumProjectionUpdater::Create(allocator, resFrustumProjection);

        cameraProjection = frustumProjection;
    }
    else if (projType == ResCamera::PROJTYPE_ORTHO)
    {
        ResOrthoProjectionUpdater resOrthoProjection(resProjectionUpdater.ptr());
        OrthoProjectionUpdater* orthoProjection = OrthoProjectionUpdater::Create(allocator, resOrthoProjection);

        cameraProjection = orthoProjection;
    }
    GfxPtr<CameraProjectionUpdater> projectionUpdater(cameraProjection);

    Camera* camera = new(memory) Camera(allocator, resCamera, description, viewUpdater, projectionUpdater, resCamera.GetWScale(), false);

    camera->Initialize(allocator);

    if (parent)
    {
        bool result = parent->AttachChild(camera);
        NW_ASSERT(result);
    }

    return camera;
}

void Camera::GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResCamera resCamera, Description description)
{
    NW_ASSERT(description.isFixedSizeMemory);

    nw::os::MemorySizeCalculator& size = *pSize;

    size += sizeof(Camera);

    ResCamera::ViewType viewType = resCamera.GetViewType();
    NW_ASSERT(viewType < ResCamera::VIEWTYPE_COUNT);
    if (viewType == ResCamera::VIEWTYPE_AIM)
    {
        AimTargetViewUpdater::GetMemorySizeInternal(&size, false);
    }
    else if (viewType == ResCamera::VIEWTYPE_LOOKAT)
    {
        LookAtTargetViewUpdater::GetMemorySizeInternal(&size, false);
    }
    else if (viewType == ResCamera::VIEWTYPE_ROTATE)
    {
        RotateViewUpdater::GetMemorySizeInternal(&size, false);
    }

    ResCamera::ProjType projType = resCamera.GetProjectionType();
    NW_ASSERT(projType < ResCamera::PROJTYPE_COUNT);
    if (projType == ResCamera::PROJTYPE_PERSPECTIVE)
    {
        PerspectiveProjectionUpdater::GetMemorySizeInternal(&size, false);
    }
    else if (projType == ResCamera::PROJTYPE_FRUSTUM)
    {
        FrustumProjectionUpdater::GetMemorySizeInternal(&size, false);
    }
    else if (projType == ResCamera::PROJTYPE_ORTHO)
    {
        OrthoProjectionUpdater::GetMemorySizeInternal(&size, false);
    }

    TransformNode::GetMemorySizeForInitialize(&size, resCamera, description);

    size += sizeof(ResCameraData);

    switch (viewType)
    {
    case ResCamera::VIEWTYPE_AIM:
        size += sizeof(ResAimTargetViewUpdaterData);
        break;
    case ResCamera::VIEWTYPE_LOOKAT:
        size += sizeof(ResLookAtTargetViewUpdaterData);
        break;
    case ResCamera::VIEWTYPE_ROTATE:
        size += sizeof(ResRotateViewUpdaterData);
        break;
    default:
        NW_ASSERT(false);
    }

    switch (projType)
    {
    case ResCamera::PROJTYPE_PERSPECTIVE:
        size += sizeof(ResPerspectiveProjectionUpdaterData);
        break;
    case ResCamera::PROJTYPE_FRUSTUM:
        size += sizeof(ResFrustumProjectionUpdaterData);
        break;
    case ResCamera::PROJTYPE_ORTHO:
        size += sizeof(ResOrthoProjectionUpdaterData);
        break;
    default:
        NW_ASSERT(false);
    }

    if (description.isAnimationEnabled && resCamera.GetAnimGroupsCount() > 0)
    {
        AnimGroup::Builder()
            .ResAnimGroup(resCamera.GetAnimGroups(0))
            .UseOriginalValue(true)
            .GetMemorySizeInternal(&size);
    }
}

void Camera::Accept(ISceneVisitor* visitor)
{
    visitor->VisitCamera(this);
    AcceptChildren(visitor);
}

void Camera::UpdateCameraMatrix()
{
    NW_NULL_ASSERT(m_ViewUpdater);
    NW_NULL_ASSERT(m_ProjectionUpdater);

    SceneNode* parentNode = GetParent();

    math::VEC3 cameraPosition(WorldMatrix().GetColumn(3));

    if (parentNode != NULL)
    {
        m_ViewUpdater->Update(&m_ViewMatrix, parentNode->TrackbackWorldMatrix(), cameraPosition);
    }
    else{
        m_ViewUpdater->Update(&m_ViewMatrix, math::MTX34::Identity(), cameraPosition);
    }

    NW_FAILSAFE_IF(math::MTX34Inverse(&m_InverseViewMatrix, &m_ViewMatrix) == 0)
    {
        m_InverseViewMatrix = math::MTX34::Identity();
    }

    m_ProjectionUpdater->Update(&m_ProjectionMatrix, &m_TextureProjectionMatrix);

    NW_FAILSAFE_IF(math::MTX44Inverse(&m_InverseProjectionMatrix, &m_ProjectionMatrix) == 0)
    {
        m_InverseProjectionMatrix = math::MTX44::Identity();
    }
}

const nw::math::VEC3& Camera::GetTargetPosition() const
{
    const ResAimTargetViewUpdater resAim = ResDynamicCast<const ResAimTargetViewUpdater>(GetViewUpdater()->GetResource());
    const ResLookAtTargetViewUpdater resLookAt = ResDynamicCast<const ResLookAtTargetViewUpdater>(GetViewUpdater()->GetResource());

    if (resAim.IsValid())
    {
        return resAim.GetTargetPosition();
    }
    else if (resLookAt.IsValid())
    {
        return resLookAt.GetTargetPosition();
    }
    else{
        NW_ASSERTMSG(false, "View updater isn't \"Aim\" nor \"LookAt\"");
        return resAim.GetTargetPosition();
    }
}

void Camera::SetTargetPosition(const nw::math::VEC3& targetPosition)
{
    ResAimTargetViewUpdater resAim = ResDynamicCast<ResAimTargetViewUpdater>(GetViewUpdater()->GetResource());
    ResLookAtTargetViewUpdater resLookAt = ResDynamicCast<ResLookAtTargetViewUpdater>(GetViewUpdater()->GetResource());

    if (resAim.IsValid())
    {
        resAim.SetTargetPosition(targetPosition);
    }
    else if (resLookAt.IsValid())
    {
        resLookAt.SetTargetPosition(targetPosition);
    }
    else{
        NW_ASSERTMSG(false, "View updater isn't \"Aim\" nor \"LookAt\"");
    }
}

const nw::math::VEC3& Camera::GetUpwardVector() const
{
    const ResLookAtTargetViewUpdater resLookAt = ResDynamicCast<const ResLookAtTargetViewUpdater>(GetViewUpdater()->GetResource());

    NW_ASSERTMSG(resLookAt.IsValid(), "View updater isn't \"LookAt\"");

    return resLookAt.GetUpwardVector();
}

void Camera::SetUpwardVector(const nw::math::VEC3& upwardVector)
{
    ResLookAtTargetViewUpdater resLookAt = ResDynamicCast<ResLookAtTargetViewUpdater>(GetViewUpdater()->GetResource());

    NW_ASSERTMSG(resLookAt.IsValid(), "View updater isn't \"LookAt\"");

    resLookAt.SetUpwardVector(upwardVector);
}

f32 Camera::GetTwist() const
{
    ResAimTargetViewUpdater resAim = ResDynamicCast<ResAimTargetViewUpdater>(GetViewUpdater()->GetResource());

    NW_ASSERTMSG(resAim.IsValid(), "View updater isn't \"LookAt\"");

    return resAim.GetTwist();
}

void Camera::SetTwist(f32 twist)
{
    ResAimTargetViewUpdater resAim = ResDynamicCast<ResAimTargetViewUpdater>(GetViewUpdater()->GetResource());

    NW_ASSERTMSG(resAim.IsValid(), "View updater isn't \"LookAt\"");

    resAim.SetTwist(twist);
}

const nw::math::VEC3& Camera::GetViewRotate() const
{
    const ResRotateViewUpdater resRotate = ResDynamicCast<const ResRotateViewUpdater>(GetViewUpdater()->GetResource());

    NW_ASSERTMSG(resRotate.IsValid(), "View updater isn't \"Rotate\"");

    return resRotate.GetViewRotate();
}

void Camera::SetViewRotate(const nw::math::VEC3& viewRotate)
{
    ResRotateViewUpdater resRotate = ResDynamicCast<ResRotateViewUpdater>(GetViewUpdater()->GetResource());

    NW_ASSERTMSG(resRotate.IsValid(), "View updater isn't \"Rotate\"");

    resRotate.SetViewRotate(viewRotate);
}

void Camera::GetPerspective(f32* fovy, f32* aspectRatio, f32* nearClip, f32* farClip) const
{
    const ResPerspectiveProjectionUpdater resource = ResDynamicCast<const ResPerspectiveProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERTMSG(resource.IsValid(), "Projection updater isn't perspective");

    if (fovy)
    {
        *fovy = resource.GetFovy();
    }
    if (aspectRatio)
    {
        *aspectRatio = resource.GetAspectRatio();
    }
    if (nearClip)
    {
        *nearClip = resource.GetNear();
    }
    if (farClip)
    {
        *farClip = resource.GetFar();
    }
}

void Camera::SetPerspective(f32 fovy, f32 aspectRatio, f32 nearClip, f32 farClip)
{
    ResPerspectiveProjectionUpdater resource = ResDynamicCast<ResPerspectiveProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERTMSG(resource.IsValid(), "Projection updater isn't perspective");

    resource.SetFovy(fovy);
    resource.SetAspectRatio(aspectRatio);
    resource.SetNear(nearClip);
    resource.SetFar(farClip);
}

void Camera::GetFrustum(f32* left, f32* right, f32* bottom, f32* top, f32* nearClip, f32* farClip) const
{
    const ResFrustumProjectionUpdater resource = ResDynamicCast<const ResFrustumProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERTMSG(resource.IsValid(), "Projection updater isn't frustum");

    if (left && right && bottom && top)
    {
        const ut::Rect rect = resource.GetRect();
        if (left)
        {
            *left = rect.left;
        }
        if (right)
        {
            *right = rect.right;
        }
        if (bottom)
        {
            *bottom = rect.bottom;
        }
        if (top)
        {
            *top = rect.top;
        }
    }
    if (nearClip)
    {
        *nearClip = resource.GetNear();
    }
    if (farClip)
    {
        *farClip = resource.GetFar();
    }
}

void Camera::SetFrustum(f32 left, f32 right, f32 bottom, f32 top, f32 nearClip, f32 farClip)
{
    ResFrustumProjectionUpdater resource = ResDynamicCast<ResFrustumProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERTMSG(resource.IsValid(), "Projection updater isn't frustum");

    resource.SetRectWithoutFlip(left, right, bottom, top);
    resource.SetNear(nearClip);
    resource.SetFar(farClip);
}

void Camera::GetOrtho(f32* left, f32* right, f32* bottom, f32* top, f32* nearClip, f32* farClip) const
{
    const ResOrthoProjectionUpdater resource = ResDynamicCast<const ResOrthoProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERTMSG(resource.IsValid(), "Projection updater isn't ortho");

    if (left && right && bottom && top)
    {
        const ut::Rect rect = resource.GetRect();
        if (left)
        {
            *left = rect.left;
        }
        if (right)
        {
            *right = rect.right;
        }
        if (bottom)
        {
            *bottom = rect.bottom;
        }
        if (top)
        {
            *top = rect.top;
        }
    }
    if (nearClip)
    {
        *nearClip = resource.GetNear();
    }
    if (farClip)
    {
        *farClip = resource.GetFar();
    }
}

void Camera::SetOrtho(f32 left, f32 right, f32 bottom, f32 top, f32 nearClip, f32 farClip)
{
    ResOrthoProjectionUpdater resource = ResDynamicCast<ResOrthoProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERTMSG(resource.IsValid(), "Projection updater isn't ortho");

    resource.SetRectWithoutFlip(left, right, bottom, top);
    resource.SetNear(nearClip);
    resource.SetFar(farClip);
}

void Camera::GetFrustum(ut::Rect* rect, f32* nearClip, f32* farClip) const
{
    const ResFrustumProjectionUpdater resource = ResDynamicCast<const ResFrustumProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERTMSG(resource.IsValid(), "Projection updater isn't frustum");

    if (rect)
    {
        *rect = resource.GetRect();
    }
    if (nearClip)
    {
        *nearClip = resource.GetNear();
    }
    if (farClip)
    {
        *farClip = resource.GetFar();
    }
}

void Camera::SetFrustum(const ut::Rect& rect, f32 nearClip, f32 farClip)
{
    SetFrustum(rect.left, rect.right, rect.bottom, rect.top, nearClip, farClip);
}

void Camera::GetOrtho(ut::Rect* rect, f32* nearClip, f32* farClip) const
{
    const ResOrthoProjectionUpdater resource = ResDynamicCast<const ResOrthoProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERTMSG(resource.IsValid(), "Projection updater isn't ortho");

    if (rect)
    {
        *rect = resource.GetRect();
    }
    if (nearClip)
    {
        *nearClip = resource.GetNear();
    }
    if (farClip)
    {
        *farClip = resource.GetFar();
    }
}

void Camera::SetOrtho(const ut::Rect& rect, f32 nearClip, f32 farClip)
{
    SetOrtho(rect.left, rect.right, rect.bottom, rect.top, nearClip, farClip);
}

void Camera::SetFrustum(const Viewport& viewport)
{
    SetFrustum(viewport.GetBound(), viewport.GetDepthNear(), viewport.GetDepthFar());
}

void Camera::SetOrtho(const Viewport& viewport)
{
    SetOrtho(viewport.GetBound(), viewport.GetDepthNear(), viewport.GetDepthFar());
}

f32 Camera::GetNear() const
{
    const ResCameraProjectionUpdater resource = ResDynamicCast<const ResCameraProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERT(resource.IsValid());

    return resource.GetNear();
}

void Camera::SetNear(f32 near)
{
    ResCameraProjectionUpdater resource = ResDynamicCast<ResCameraProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERT(resource.IsValid());

    resource.SetNear(near);
}

f32 Camera::GetFar() const
{
    const ResCameraProjectionUpdater resource = ResDynamicCast<const ResCameraProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERT(resource.IsValid());

    return resource.GetFar();
}

void Camera::SetFar(f32 far)
{
    ResCameraProjectionUpdater resource = ResDynamicCast<ResCameraProjectionUpdater>(GetProjectionUpdater()->GetResource());

    NW_ASSERT(resource.IsValid());

    resource.SetFar(far);
}

Result Camera::StoreOriginal(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    ResCamera original = GetResCamera();
    NW_NULL_ASSERT(original.GetViewUpdater().ptr());
    NW_NULL_ASSERT(original.GetProjectionUpdater().ptr());

    void* cameraMemory = allocator->Alloc(sizeof(ResCameraData));
    NW_NULL_ASSERT(cameraMemory);

    ResCameraData* cameraData = new(cameraMemory) ResCameraData(original.ref());

    cameraData->toViewUpdater = NULL;
    cameraData->toProjectionUpdater = NULL;

    void* viewUpdaterMemory = NULL;
    ResCameraViewUpdaterData* viewUpdaterData = NULL;
    switch (original.GetViewType())
    {
    case ResCamera::VIEWTYPE_AIM:
        viewUpdaterMemory = allocator->Alloc(sizeof(ResAimTargetViewUpdaterData));
        NW_NULL_ASSERT(viewUpdaterMemory);
        viewUpdaterData = new(viewUpdaterMemory) ResAimTargetViewUpdaterData(
            *reinterpret_cast<ResAimTargetViewUpdaterData*>(original.GetViewUpdater().ptr()));
        break;

    case ResCamera::VIEWTYPE_LOOKAT:
        viewUpdaterMemory = allocator->Alloc(sizeof(ResLookAtTargetViewUpdaterData));
        NW_NULL_ASSERT(viewUpdaterMemory);
        viewUpdaterData = new(viewUpdaterMemory) ResLookAtTargetViewUpdaterData(
            *reinterpret_cast<ResLookAtTargetViewUpdaterData*>(original.GetViewUpdater().ptr()));
        break;

    case ResCamera::VIEWTYPE_ROTATE:
        viewUpdaterMemory = allocator->Alloc(sizeof(ResRotateViewUpdaterData));
        NW_NULL_ASSERT(viewUpdaterMemory);
        viewUpdaterData = new(viewUpdaterMemory) ResRotateViewUpdaterData(
            *reinterpret_cast<ResRotateViewUpdaterData*>(original.GetViewUpdater().ptr()));
        break;

    default:
        NW_ASSERT(false);
    }

    cameraData->toViewUpdater.set_ptr(viewUpdaterData);

    void* projectionUpdaterMemory = NULL;
    ResCameraProjectionUpdaterData* projectionUpdaterData = NULL;

    switch (original.GetProjectionType())
    {
    case ResCamera::PROJTYPE_PERSPECTIVE:
        projectionUpdaterMemory = allocator->Alloc(sizeof(ResPerspectiveProjectionUpdaterData));
        NW_NULL_ASSERT(projectionUpdaterMemory);
        projectionUpdaterData = new(projectionUpdaterMemory) ResPerspectiveProjectionUpdaterData(
            *reinterpret_cast<ResPerspectiveProjectionUpdaterData*>(original.GetProjectionUpdater().ptr()));
        break;

    case ResCamera::PROJTYPE_FRUSTUM:
        projectionUpdaterMemory = allocator->Alloc(sizeof(ResFrustumProjectionUpdaterData));
        NW_NULL_ASSERT(projectionUpdaterMemory);
        projectionUpdaterData = new(projectionUpdaterMemory) ResFrustumProjectionUpdaterData(
            *reinterpret_cast<ResFrustumProjectionUpdaterData*>(original.GetProjectionUpdater().ptr()));
        break;

    case ResCamera::PROJTYPE_ORTHO:
        projectionUpdaterMemory = allocator->Alloc(sizeof(ResOrthoProjectionUpdaterData));
        NW_NULL_ASSERT(projectionUpdaterMemory);
        projectionUpdaterData = new(projectionUpdaterMemory) ResOrthoProjectionUpdaterData(
            *reinterpret_cast<ResOrthoProjectionUpdaterData*>(original.GetProjectionUpdater().ptr()));
        break;

    default:
        NW_ASSERT(false);
    }

    cameraData->toProjectionUpdater.set_ptr(projectionUpdaterData);

    m_OriginalValue = ResCamera(cameraData);

    m_OriginalTransform = GetResTransformNode().GetTransform();

    return result;
}

void Camera::DestroyResCamera(nw::os::IAllocator* allocator, ResCamera resCamera)
{
    NW_NULL_ASSERT(allocator);
    if (resCamera.IsValid())
    {
        if (resCamera.GetProjectionUpdater().IsValid())
        {
            allocator->Free(resCamera.GetProjectionUpdater().ptr());
            resCamera.ref().toProjectionUpdater.set_ptr(NULL);
        }
        if (resCamera.GetViewUpdater().IsValid())
        {
            allocator->Free(resCamera.GetViewUpdater().ptr());
            resCamera.ref().toViewUpdater.set_ptr(NULL);
        }

        allocator->Free(resCamera.ptr());
    }
}

void* Camera::GetAnimTargetObject(const anim::ResAnimGroupMember& anim)
{
    switch (anim.GetObjectType())
    {
    case anim::ResAnimGroupMember::OBJECT_TYPE_TRANSFORM:{
            return static_cast<nw::gfx::TransformNode*>(this);
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_VIEW_UPDATER:{
            ResCameraViewUpdaterData* ptr = GetViewUpdater()->GetResource().ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_PROJECTION_UPDATER:{
            ResCameraProjectionUpdaterData* ptr = GetProjectionUpdater()->GetResource().ptr();
            return ptr;
        }

    default:
        NW_ASSERT(false);
        return NULL;
    }
}

Result Camera::Initialize(nw::os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    result |= TransformNode::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= StoreOriginal(allocator);
    result |= CreateAnimGroup(allocator);

    return result;
}

bool Camera::ValidateCameraAnimType(AnimObject* animObject)
{
    if (animObject == NULL)
    {
        return true;
    }

    AnimBlender* blender = ut::DynamicCast<AnimBlender*>(animObject);

    if (blender != NULL)
    {
        const int animObjCount = blender->GetAnimObjectCount();
        for (int i = 0; i < animObjCount; ++i)
        {
            AnimObject* animObj = blender->GetAnimObject(i);

            if (!ValidateCameraAnimType(animObj))
            {
                return false;
            }
        }

        return true;
    }
    else{
        AnimEvaluator* evaluator = ut::DynamicCast<AnimEvaluator*>(animObject);
        NW_NULL_ASSERT(evaluator);

        anim::ResCameraAnim cameraAnim(evaluator->GetAnimData().ptr());

        return (
            (cameraAnim.GetProjectionUpdaterKind() == m_ProjectionUpdater->Kind()) &&
            (cameraAnim.GetViewUpdaterKind() == m_ViewUpdater->Kind()));
    }
}

}
}