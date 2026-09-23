// Filename: anim_ResAnimGroup.cpp
//
// Project: NintendoWare4Ctr

#include <nw/anim/res/anim_ResAnimGroup.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_Light.h>
#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/gfx_VertexLight.h>
#include <nw/gfx/gfx_HemiSphereLight.h>
#include <nw/gfx/gfx_Fog.h>

using namespace nw::gfx;

namespace nw {
namespace anim {
namespace res {

void ResAnimGroupMember::SetValueForType(void* object, const void* value) const
{
    NW_NULL_ASSERT(object);
    NW_NULL_ASSERT(value);

    switch(GetObjectType())
    {
    case OBJECT_TYPE_BONE:
        ResStaticCast<ResBoneMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_MATERIAL_COLOR:
        ResStaticCast<ResMaterialColorMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_TEXTURE_SAMPLER:
        ResStaticCast<ResTextureSamplerMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_TEXTURE_MAPPER:
        ResStaticCast<ResTextureMapperMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_BLEND_OPERATION:
        ResStaticCast<ResBlendOperationMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_TEXTURE_COORDINATOR:
        ResStaticCast<ResTextureCoordinatorMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_MODEL:
        ResStaticCast<ResModelMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_MESH:
        ResStaticCast<ResMeshMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_MESH_NODE_VISIBILITY:
        ResStaticCast<ResMeshNodeVisibilityMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_TRANSFORM:
        ResStaticCast<ResTransformMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_VIEW_UPDATER:
        ResStaticCast<ResViewUpdaterMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_PROJECTION_UPDATER:
        ResStaticCast<ResProjectionUpdaterMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_LIGHT:
        ResStaticCast<ResLightMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_FRAGMENT_LIGHT:
        ResStaticCast<ResFragmentLightMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_AMBIENT_LIGHT:
        ResStaticCast<ResAmbientLightMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_VERTEX_LIGHT:
        ResStaticCast<ResVertexLightMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_HEMISPHERE_LIGHT:
        ResStaticCast<ResHemiSphereLightMember>(*this).SetValue(object, value);
        break;

    case OBJECT_TYPE_FOG:
        ResStaticCast<ResFogMember>(*this).SetValue(object, value);
        break;

    default:
        NW_ASSERT(false);
        break;
    }
}

void ResBoneMember::SetValue(void* object, const void* value) const
{
    CalculatedTransform* bone =
        reinterpret_cast<CalculatedTransform*>(object);
    const math::Transform3* transform =
        reinterpret_cast<const math::Transform3*>(value);

    bone->SetTransform(*transform);
    bone->UpdateScaleFlags();
    bone->UpdateRotateFlags();
    bone->UpdateTranslateFlags();
    bone->UpdateCompositeFlags();
}

void ResMaterialColorMember::SetValue(void* object, const void* value) const
{
    ResMaterialColor materialColor(object);
    const ut::FloatColor& color = *reinterpret_cast<const ut::FloatColor*>(value);

    switch(GetMemberType())
    {
    case MEMBER_TYPE_EMISSION:
        materialColor.SetEmission(color);
        break;

    case MEMBER_TYPE_AMBIENT:
        materialColor.SetAmbient(color.r, color.g, color.b);
        break;

    case MEMBER_TYPE_DIFFUSE:
        materialColor.SetDiffuse(color);
        break;

    case MEMBER_TYPE_SPECULAR0:
        materialColor.SetSpecular0(color);
        break;

    case MEMBER_TYPE_SPECULAR1:
        materialColor.SetSpecular1(color);
        break;

    case MEMBER_TYPE_CONSTANT0:
        materialColor.SetConstant0(color);
        break;

    case MEMBER_TYPE_CONSTANT1:
        materialColor.SetConstant1(color);
        break;

    case MEMBER_TYPE_CONSTANT2:
        materialColor.SetConstant2(color);
        break;

    case MEMBER_TYPE_CONSTANT3:
        materialColor.SetConstant3(color);
        break;

    case MEMBER_TYPE_CONSTANT4:
        materialColor.SetConstant4(color);
        break;

    case MEMBER_TYPE_CONSTANT5:
        materialColor.SetConstant5(color);
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResTextureSamplerMember::SetValue(void* object, const void* value) const
{
    ResPixelBasedTextureMapper mapper(object);
    const ut::FloatColor& color = *reinterpret_cast<const ut::FloatColor*>(value);

    NW_NULL_ASSERT(ResDynamicCast<ResStandardTextureSampler>(mapper.GetSampler()).ptr());

    ResStandardTextureSampler sampler(mapper.ref().toSampler.to_ptr());

    switch(GetMemberType())
    {
    case MEMBER_TYPE_BORDER_COLOR:
        sampler.SetBorderColor(color);
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResTextureMapperMember::SetValue(void* object, const void* value) const
{
    ResPixelBasedTextureMapper mapper(object);
    const ut::Offset& offset = *reinterpret_cast<const ut::Offset*>(value);

    ResReferenceTexture texture(offset.to_ptr());

    switch(GetMemberType())
    {
    case MEMBER_TYPE_TEXTURE:
        mapper.SetTexture(texture.GetTargetTexture());
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResBlendOperationMember::SetValue(void* object, const void* value) const
{
    ResBlendOperation operation(object);
    const ut::FloatColor& color = *reinterpret_cast<const ut::FloatColor*>(value);

    switch(GetMemberType())
    {
    case MEMBER_TYPE_BLEND_COLOR:
        operation.SetBlendColor(color);
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResTextureCoordinatorMember::SetValue(void* object, const void* value) const
{
    ResTextureCoordinator coordinator(object);

    switch(GetMemberType())
    {
    case MEMBER_TYPE_SCALE:
        {
            const math::VEC2& scale = *reinterpret_cast<const math::VEC2*>(value);
            coordinator.SetScale(scale);
        }
        break;

    case MEMBER_TYPE_ROTATE:
        {
            float rotate = *reinterpret_cast<const float*>(value);
            coordinator.SetRotate(rotate);
        }
        break;

    case MEMBER_TYPE_TRANSLATE:
        {
            const math::VEC2& translate = *reinterpret_cast<const math::VEC2*>(value);
            coordinator.SetTranslate(translate);
        }
        break;

    default:
        NW_ASSERT(false);
    }

    coordinator.SetDirty(true);
}

void ResModelMember::SetValue(void* object, const void* value) const
{
    Model* model = reinterpret_cast<Model*>(object);
    
    switch(GetMemberType())
    {
    case MEMBER_TYPE_BRANCH_VISIBLE:
        {
            const bool visibility = *static_cast<const bool*>(value);
            model->SetBranchVisible(visibility);
        }
        break;
    case MEMBER_TYPE_VISIBLE:
        {
            const bool visibility = *static_cast<const bool*>(value);
            model->SetVisible(visibility);
        }
        break;
    default:
        NW_ASSERT(false);
    }
}

void ResMeshMember::SetValue(void* object, const void* value) const
{
    ResMesh mesh(object);
    
    const bool visibility = *static_cast<const bool*>(value);
    mesh.SetVisible(visibility);
}

void ResMeshNodeVisibilityMember::SetValue(void* object, const void* value) const
{
    ResMeshNodeVisibility nodeVisibility(object);
    
    const bool visibility = *static_cast<const bool*>(value);
    nodeVisibility.SetVisible(visibility);
}

void ResTransformMember::SetValue(void* object, const void* value) const
{
    TransformNode* transformNode = reinterpret_cast<TransformNode*>(object);

    switch(GetMemberType())
    {
    case MEMBER_TYPE_TRANSFORM:
        {
            transformNode->Transform().SetTransform(*reinterpret_cast<const math::Transform3*>(value));
            transformNode->Transform().EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
        }
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResViewUpdaterMember::SetValue(void* object, const void* value) const
{
    ResCameraViewUpdater viewUpdater(object);

    switch(viewUpdater.GetTypeInfo())
    {
    case ResAimTargetViewUpdater::TYPE_INFO:
        {
            ResAimTargetViewUpdater aimUpdater(object);

            switch(GetMemberType())
            {
            case MEMBER_TYPE_TARGET_POSITION:
                {
                    const math::VEC3& position = *reinterpret_cast<const math::VEC3*>(value);
                    aimUpdater.SetTargetPosition(position);
                }
                break;

            case MEMBER_TYPE_TWIST:
                {
                    float twist = *reinterpret_cast<const float*>(value);
                    aimUpdater.SetTwist(twist);
                }
                break;

            default:
                NW_ASSERT(false);
            }
        }
        break;

    case ResLookAtTargetViewUpdater::TYPE_INFO:
        {
            ResLookAtTargetViewUpdater lookAtUpdater(object);

            switch(GetMemberType())
            {
            case MEMBER_TYPE_TARGET_POSITION:
                {
                    const math::VEC3& position = *reinterpret_cast<const math::VEC3*>(value);
                    lookAtUpdater.SetTargetPosition(position);
                }
                break;

            case MEMBER_TYPE_UPWARD_VECTOR:
                {
                    const math::VEC3& upVector = *reinterpret_cast<const math::VEC3*>(value);
                    lookAtUpdater.SetUpwardVector(upVector);
                }
                break;

            default:
                NW_ASSERT(false);
            }
        }
        break;

    case ResRotateViewUpdater::TYPE_INFO:
        {
            ResRotateViewUpdater rotateUpdater(object);

            switch(GetMemberType())
            {
            case MEMBER_TYPE_VIEW_ROTATE:
                {
                    const math::VEC3& rotate = *reinterpret_cast<const math::VEC3*>(value);
                    rotateUpdater.SetViewRotate(rotate);
                }
                break;

            default:
                NW_ASSERT(false);
            }
        }
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResProjectionUpdaterMember::SetValue(void* object, const void* value) const
{
    ResCameraProjectionUpdater projectionUpdater(object);

    switch(projectionUpdater.GetTypeInfo())
    {
    case ResPerspectiveProjectionUpdater::TYPE_INFO:
        {
            ResPerspectiveProjectionUpdater perspectiveUpdater(object);
            float floatValue = *reinterpret_cast<const float*>(value);

            switch(GetMemberType())
            {
            case MEMBER_TYPE_NEAR:
                perspectiveUpdater.SetNear(floatValue);
                break;

            case MEMBER_TYPE_FAR:
                perspectiveUpdater.SetFar(floatValue);
                break;

            case MEMBER_TYPE_FOVY:
                perspectiveUpdater.SetFovy(floatValue);
                break;

            case MEMBER_TYPE_ASPECT_RATIO:
                perspectiveUpdater.SetAspectRatio(floatValue);
                break;

            default:
                NW_ASSERT(false);
            }
        }
        break;

    case ResOrthoProjectionUpdater::TYPE_INFO:
        {
            ResOrthoProjectionUpdater orthoUpdater(object);
            float floatValue = *reinterpret_cast<const float*>(value);

            switch(GetMemberType())
            {
            case MEMBER_TYPE_NEAR:
                orthoUpdater.SetNear(floatValue);
                break;

            case MEMBER_TYPE_FAR:
                orthoUpdater.SetFar(floatValue);
                break;

            case MEMBER_TYPE_ASPECT_RATIO:
                {
                    ResProjectionRect rect = orthoUpdater.GetRect();
                    rect.Set(floatValue, rect.GetHeight(), rect.GetCenter());
                    orthoUpdater.SetRect(rect);
                }
                break;

            case MEMBER_TYPE_HEIGHT:
                {
                    ResProjectionRect rect = orthoUpdater.GetRect();
                    rect.Set(rect.GetAspectRatio(), floatValue, rect.GetCenter());
                    orthoUpdater.SetRect(rect);
                }
                break;

            case MEMBER_TYPE_CENTER:
                {
                    const math::VEC2& center = *reinterpret_cast<const math::VEC2*>(value);

                    ResProjectionRect rect = orthoUpdater.GetRect();
                    rect.Set(rect.GetAspectRatio(), rect.GetHeight(), center);
                    orthoUpdater.SetRect(rect);
                }
                break;

            default:
                NW_ASSERT(false);
            }
        }
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResLightMember::SetValue(void* object, const void* value) const
{
    ResLight light(object);
    bool boolValue = *static_cast<const bool*>(value);

    switch (GetMemberType())
    {
    case MEMBER_TYPE_IS_LIGHT_ENABLED:
        light.SetLightEnabled(boolValue);
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResFragmentLightMember::SetValue(void* object, const void* value) const
{
    ResFragmentLight light(object);
    const ut::FloatColor& color = *reinterpret_cast<const ut::FloatColor*>(value);

    switch (GetMemberType())
    {
    case MEMBER_TYPE_AMBIENT:
        light.SetAmbient(color);
        break;

    case MEMBER_TYPE_DIFFUSE:
        light.SetDiffuse(color);
        break;

    case MEMBER_TYPE_SPECULAR0:
        light.SetSpecular0(color);
        break;

    case MEMBER_TYPE_SPECULAR1:
        light.SetSpecular1(color);
        break;

    case MEMBER_TYPE_DIRECTION:
        {
            math::VEC3 direction = *reinterpret_cast<const math::VEC3*>(value);
            direction.Normalize();
            light.SetDirection(direction);
        }
        break;

    case MEMBER_TYPE_DISTANCE_ATTENUATION_START:
        {
            float start = *reinterpret_cast<const float*>(value);
            light.SetDistanceAttenuationStart(start);
        }
        break;

    case MEMBER_TYPE_DISTANCE_ATTENUATION_END:
        {
            float end = *reinterpret_cast<const float*>(value);
            light.SetDistanceAttenuationEnd(end);
        }
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResAmbientLightMember::SetValue(void* object, const void* value) const
{
    ResAmbientLight light(object);
    const ut::FloatColor& color = *reinterpret_cast<const ut::FloatColor*>(value);

    switch (GetMemberType())
    {
    case MEMBER_TYPE_AMBIENT:
        light.SetAmbient(color);
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResVertexLightMember::SetValue(void* object, const void* value) const
{
    ResVertexLight light(object);
    const ut::FloatColor& color = *reinterpret_cast<const ut::FloatColor*>(value);
    float floatValue = *static_cast<const float*>(value);

    switch (GetMemberType())
    {
    case MEMBER_TYPE_AMBIENT:
        light.SetAmbient(color);
        break;

    case MEMBER_TYPE_DIFFUSE:
        light.SetDiffuse(color);
        break;

    case MEMBER_TYPE_DIRECTION:
        {
            math::VEC3 direction = *reinterpret_cast<const math::VEC3*>(value);
            direction.Normalize();
            light.SetDirection(direction);
        }
        break;

    case MEMBER_TYPE_DISTANCE_ATTENUATION_CONSTANT:
        {
            math::VEC3 attenuation = light.GetDistanceAttenuation();
            attenuation.x = floatValue;
            light.SetDistanceAttenuation(attenuation);
        }
        break;

    case MEMBER_TYPE_DISTANCE_ATTENUATION_LINEAR:
        {
            math::VEC3 attenuation = light.GetDistanceAttenuation();
            attenuation.y = floatValue;
            light.SetDistanceAttenuation(attenuation);
        }
        break;

    case MEMBER_TYPE_DISTANCE_ATTENUATION_QUADRATIC:
        {
            math::VEC3 attenuation = light.GetDistanceAttenuation();
            attenuation.z = floatValue;
            light.SetDistanceAttenuation(attenuation);
        }
        break;

    case MEMBER_TYPE_SPOT_EXPONENT:
        {
            math::VEC2 spotFactor = light.GetSpotFactor();
            spotFactor.x = floatValue;
            light.SetSpotFactor(spotFactor);
        }
        break;

    case MEMBER_TYPE_SPOT_CUTOFF_ANGLE:
        {
            math::VEC2 spotFactor = light.GetSpotFactor();
            spotFactor.y = floatValue;
            light.SetSpotFactor(spotFactor);
        }
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResHemiSphereLightMember::SetValue(void* object, const void* value) const
{
    ResHemiSphereLight light(object);
    const ut::FloatColor& color = *reinterpret_cast<const ut::FloatColor*>(value);

    switch (GetMemberType())
    {
    case MEMBER_TYPE_GROUND_COLOR:
        light.SetGroundColor(color);
        break;

    case MEMBER_TYPE_SKY_COLOR:
        light.SetSkyColor(color);
        break;

    case MEMBER_TYPE_DIRECTION:
        {
            math::VEC3 direction = *reinterpret_cast<const math::VEC3*>(value);
            direction.Normalize();
            light.SetDirection(direction);
        }
        break;

    case MEMBER_TYPE_LERP_FACTOR:
        {
            float floatValue = *static_cast<const float*>(value);
            light.SetLerpFactor(floatValue);
        }
        break;

    default:
        NW_ASSERT(false);
    }
}

void ResFogMember::SetValue(void* object, const void* value) const
{
    ResFog fog(object);

    switch (GetMemberType())
    {
    case MEMBER_TYPE_COLOR:
        {
            const ut::FloatColor& color = *reinterpret_cast<const ut::FloatColor*>(value);
            fog.SetColor(color);
        }
        break;

    default:
        NW_ASSERT(false);
    }
}

} // namespace res
} // namespace anim
} // namespace nw