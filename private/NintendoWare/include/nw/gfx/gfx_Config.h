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

#ifndef NW_GFX_CONFIG_H_
#define NW_GFX_CONFIG_H_

// TODO: Be sure to record the macros described in this file on the macro list page.

// Enabling this definition also enables the calculation of Billboard.
#define NW_GFX_BILLBOARD_UPDATE_ENABLED

// This setting enables processing required to fix a problem where scaling was not being applied correctly to billboard nodes if the ScalingRule was a Softimage skeleton.
// 
// #define NW_GFX_BILLBOARD_SOFTIMAGE_ISSUE_WORKAROUND_ENABLED

// Enabling this definition also enables the calculation of VertexLight.
#define NW_GFX_VERTEX_LIGHT_ENABLED

// Enabling this definition also enables the generation of OpenGL's ProgramObject.
// #define NW_GFX_PROGRAM_OBJECT_ENABLED

// Enabling this definition sends the bone index scale to the shader.
// #define NW_GFX_USE_BONE_INDEX_SCALE

// Enabling this definition also enables the updating of the flag after the calculation of WorldMatrix.
// #define NW_GFX_WORLD_MATRIX_FLAG_UPDATE_ENABLED

// If this macro is enabled, setting it in the shader appends a feature that adds an offset to the model's translation value.
// This feature is not supported.
// References: nw::gfx::RenderContext::ModelTranslateOffset
//
//#define NW_GFX_MODEL_TRANSLATE_OFFSET_ENABLED

// If this definition is enabled, the particles affected by compatibility will not change.
// Reduces the amount of memory used for ParticleContext, but the execution speed will drop slightly.
// Slight differences in the results of animation processing will appear depending on whether there are options.
// Builds without options are not recommended.
#define NW_GFX_PARTICLE_COMPAT_1_1

// Enabling this definition allows inspection of the particle updating and the rendering sequence.
//  Outputs a warning if a suspicious processing sequence is discovered.
#ifndef NW_RELEASE
#define NW_CHECK_PARTICLE_PROCESS
#endif

#endif // NW_GFX_CONFIG_H_
