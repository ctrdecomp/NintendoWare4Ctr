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

#ifndef NW_GFX_RESUTIL_H_
#define NW_GFX_RESUTIL_H_

namespace nw {
namespace gfx {

class Result;

namespace res {

class ResLookupTable;
class ResTexture;
class ResShader;
class ResReferenceLookupTable;
class ResReferenceTexture;
class ResReferenceShader;
class ResGraphicsFile;

//
Result SetupReferenceLut(ResReferenceLookupTable resReferenceLut, ResGraphicsFile graphicsFile);

//
::std::pair<ResLookupTable, bool>
    GetReferenceLutTarget(ResReferenceLookupTable referenceLut, ResGraphicsFile graphicsFile);

//
::std::pair<ResTexture,bool>
    GetReferenceTextureTarget(ResReferenceTexture referenceTexture, ResGraphicsFile graphicsFile);

//
::std::pair<ResShader,bool>
    GetReferenceShaderTarget(ResReferenceShader referenceShader, ResGraphicsFile graphicsFile);

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESUTIL_H_
