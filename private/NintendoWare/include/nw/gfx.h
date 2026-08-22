/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_GFX_H_
#define NW_GFX_H_

/* Please see man pages for details 
  
  
*/

/* Please see man pages for details 
  
  
*/

#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/gfx_GfxObject.h>

// Update/render
#include <nw/gfx/gfx_IRenderTarget.h>
#include <nw/gfx/gfx_ISceneUpdater.h>
#include <nw/gfx/gfx_SceneUpdater.h>
#include <nw/gfx/gfx_RenderQueue.h>
#include <nw/gfx/gfx_RenderElement.h>
#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/gfx_MeshRenderer.h>
#include <nw/gfx/gfx_RenderContext.h>
#include <nw/gfx/gfx_SceneContext.h>
#include <nw/gfx/gfx_SceneTraverser.h>
#include <nw/gfx/gfx_SceneInitializer.h>
#include <nw/gfx/gfx_SkeletonUpdater.h>
#include <nw/gfx/gfx_WorldMatrixUpdater.h>
#include <nw/gfx/gfx_BillboardUpdater.h>
#include <nw/gfx/gfx_IMaterialActivator.h>
#include <nw/gfx/gfx_MaterialActivator.h>
#include <nw/gfx/gfx_SimpleMaterialActivator.h>
#include <nw/gfx/gfx_DirectMaterialActivator.h>
#include <nw/gfx/gfx_IMaterialIdGenerator.h>
#include <nw/gfx/gfx_SortingMaterialIdGenerator.h>
#include <nw/gfx/gfx_SceneEnvironment.h>
#include <nw/gfx/gfx_ShaderBinaryInfo.h>

// Scene object
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/gfx_Material.h>
#include <nw/gfx/gfx_SceneNode.h>
#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_UserRenderNode.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_StandardSkeleton.h>
#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/gfx_SceneBuilder.h>
#include <nw/gfx/gfx_SceneHelper.h>

#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/gfx/gfx_ShaderProgram.h>

// Scene environment settings
#include <nw/gfx/gfx_SceneEnvironmentSetting.h>
#include <nw/gfx/gfx_LightSet.h>

// Fog
#include <nw/gfx/gfx_Fog.h>

// Lights
#include <nw/gfx/gfx_Light.h>
#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_VertexLight.h>
#include <nw/gfx/gfx_HemiSphereLight.h>

// Camera
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_CameraProjectionUpdater.h>
#include <nw/gfx/gfx_FrustumProjectionUpdater.h>
#include <nw/gfx/gfx_OrthoProjectionUpdater.h>
#include <nw/gfx/gfx_PerspectiveProjectionUpdater.h>
#include <nw/gfx/gfx_CameraViewUpdater.h>
#include <nw/gfx/gfx_AimTargetViewUpdater.h>
#include <nw/gfx/gfx_LookAtTargetViewUpdater.h>
#include <nw/gfx/gfx_RotateViewUpdater.h>

// Viewport
#include <nw/gfx/gfx_Viewport.h>

// Particle
#include <nw/gfx/gfx_ParticleCollection.h>
#include <nw/gfx/gfx_ParticleContext.h>
#include <nw/gfx/gfx_ParticleEmitter.h>
#include <nw/gfx/gfx_ParticleModel.h>
#include <nw/gfx/gfx_ParticleRandom.h>
#include <nw/gfx/gfx_ParticleSceneUpdater.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_ParticleShape.h>
#include <nw/gfx/gfx_ParticleTime.h>
#include <nw/gfx/gfx_ParticleUtil.h>

// Resource class
#include <nw/gfx/res/gfx_ResGraphicsFile.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/res/gfx_ResProceduralTexture.h>
#include <nw/gfx/res/gfx_ResFragmentShader.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResMesh.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResShader.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>
#include <nw/gfx/res/gfx_ResCamera.h>
#include <nw/gfx/res/gfx_ResLight.h>
#include <nw/gfx/res/gfx_ResFog.h>
#include <nw/gfx/res/gfx_ResVertex.h>
#include <nw/gfx/res/gfx_ResUtil.h>
#include <nw/gfx/res/gfx_ResSceneEnvironmentSetting.h>

// Particle resources
#include <nw/gfx/res/gfx_ResParticleAnimation.h>
#include <nw/gfx/res/gfx_ResParticleAnimationOption.h>
#include <nw/gfx/res/gfx_ResParticleCollection.h>
#include <nw/gfx/res/gfx_ResParticleEmitter.h>
#include <nw/gfx/res/gfx_ResParticleForm.h>
#include <nw/gfx/res/gfx_ResParticleInitializer.h>
#include <nw/gfx/res/gfx_ResParticleModel.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>
#include <nw/gfx/res/gfx_ResParticleShapeBuilder.h>
#include <nw/gfx/res/gfx_ResParticleUpdater.h>

// Animation
#include <nw/gfx/gfx_AnimAdder.h>
#include <nw/gfx/gfx_AnimBinding.h>
#include <nw/gfx/gfx_AnimBlender.h>
#include <nw/gfx/gfx_AnimEvaluator.h>
#include <nw/gfx/gfx_AnimGroup.h>
#include <nw/gfx/gfx_AnimInterpolator.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_AnimOverrider.h>
#include <nw/gfx/gfx_BaseAnimEvaluator.h>
#include <nw/gfx/gfx_TransformAnimAdder.h>
#include <nw/gfx/gfx_TransformAnimBlendOp.h>
#include <nw/gfx/gfx_TransformAnimEvaluator.h>
#include <nw/gfx/gfx_TransformAnimInterpolator.h>
#include <nw/gfx/gfx_TransformAnimInterpolatorLite.h>
#include <nw/gfx/gfx_TransformAnimOverrider.h>
#include <nw/gfx/gfx_SharedAnimCache.h>

#endif /* NW_GFX_H_ */

