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

#ifndef NW_GFX_RESPARTICLEEMITTER_H_
#define NW_GFX_RESPARTICLEEMITTER_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>
#include <nw/gfx/res/gfx_ResParticleForm.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResParticleEmitterData : public ResTransformNodeData
{
    nw::ut::ResU32 m_Id;                        //
    nw::ut::BinString toParticleSetPath;        //
    nw::ut::Offset toParticleForm;              //
    nw::ut::ResBool m_IsResourceCopyEnabled;    //
    u8              padding_0[3];               //
    nw::ut::ResF32 m_EmissionRatio;             //
    nw::ut::ResF32 m_EmissionRatioRandom;       //
    nw::ut::ResS32 m_EmissionInterval;          //
    nw::ut::ResF32 m_EmissionIntervalRandom;    //
    nw::ut::ResBool m_EmissionSpanInfinity;     //
    u8              padding_1[3];               //
    nw::ut::ResS32 m_EmissionSpan;              //
    nw::ut::ResS32 m_EmissionStart;             //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
// For resource copying
struct ResParticleEmitterParameterData
{
    nw::ut::ResBool m_IsResourceCopyEnabled;    //
    u8              padding_0[3];               //
    nw::ut::ResF32 m_EmissionRatio;             //
    nw::ut::ResF32 m_EmissionRatioRandom;       //
    nw::ut::ResS32 m_EmissionInterval;          //
    nw::ut::ResF32 m_EmissionIntervalRandom;    //
    nw::ut::ResBool m_EmissionSpanInfinity;     //
    u8              padding_1[3];               //
    nw::ut::ResS32 m_EmissionSpan;              //
    nw::ut::ResS32 m_EmissionStart;             //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResParticleEmitter : public ResTransformNode
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleEmitter) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PEMT') };

    NW_RES_CTOR_INHERIT( ResParticleEmitter, ResTransformNode )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, Id )                     // GetId(), SetId()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRING_DECL( ParticleSetPath )                // GetParticleSetPath()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResParticleForm, ParticleForm)    // GetParticleForm()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( bool, IsResourceCopyEnabled)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionRatio )          // GetEmissionRatio(), SetEmissionRatio()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionRatioRandom )    // GetEmissionRatioRandom(), SetEmissionRatioRandom()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionInterval )       // GetEmissionInterval(), SetEmissionInterval()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionIntervalRandom ) // GetEmissionIntervalRandom(), SetEmissionIntervalRandom()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( bool, EmissionSpanInfinity)   // GetEmissionSpanInfinity(), SetEmissionSpanInfinity()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionSpan)            // GetEmissionSpan(), SetEmissionSpan()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionStart)           // GetEmissionStart(), SetEmissionStart()
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResParticleEmitterParameter : public nw::ut::ResCommon<ResParticleEmitterParameterData>
{
public:
//    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleEmitterParameter) };
//    enum { SIGNATURE = NW_RES_SIGNATURE32('PEMT') };

    NW_RES_CTOR( ResParticleEmitterParameter )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( bool, IsResourceCopyEnabled)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionRatio )          // GetEmissionRatio(), SetEmissionRatio()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionRatioRandom )    // GetEmissionRatioRandom(), SetEmissionRatioRandom()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionInterval )       // GetEmissionInterval(), SetEmissionInterval()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionIntervalRandom ) // GetEmissionIntervalRandom(), SetEmissionIntervalRandom()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( bool, EmissionSpanInfinity)   // GetEmissionSpanInfinity(), SetEmissionSpanInfinity()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionSpan)            // GetEmissionSpan(), SetEmissionSpan()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionStart)           // GetEmissionStart(), SetEmissionStart()
};

//
typedef nw::ut::ResArrayPatricia<ResParticleEmitter>::type  ResEmitterArray;

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESPARTICLEEMITTER_H_
