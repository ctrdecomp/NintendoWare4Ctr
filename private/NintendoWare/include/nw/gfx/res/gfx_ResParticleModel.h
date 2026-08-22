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

#ifndef NW_GFX_RESPARTICLEMODEL_H_
#define NW_GFX_RESPARTICLEMODEL_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
//
struct ResParticleModelData : public ResModelData
{
    nw::ut::ResS32 m_ParticleSetsTableCount;    //
    nw::ut::Offset toParticleSetsTable;         //
};


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
class ResParticleModel : public ResModel
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleModel) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PMDL') };
    enum { BINARY_REVISION = REVISION_RES_EMITTER };

    NW_RES_CTOR_INHERIT( ResParticleModel, ResModel )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResParticleSet, ParticleSets ) // GetParticleSets(int idx), GetParticleSetsCount()

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void    Setup();

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    template<typename TFunction>
    void ForeachParticleSet(TFunction function)
    {
        if (this->IsValid())
        {
            for (s32 i = 0; i < this->GetParticleSetsCount(); ++i)
            {
                ResParticleSet resParticleSet = this->GetParticleSets(i);
                function(resParticleSet);
            }
        }
    }
};

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESPARTICLEMODEL_H_
