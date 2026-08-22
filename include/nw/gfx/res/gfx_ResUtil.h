#pragma once

#include <utility>

namespace nw { /* nw */
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

Result SetupReferenceLut(ResReferenceLookupTable resReferenceLut, ResGraphicsFile graphicsFile);

::std::pair<ResLookupTable, bool>
    GetReferenceLutTarget(ResReferenceLookupTable referenceLut, ResGraphicsFile graphicsFile);

::std::pair<ResTexture,bool>
    GetReferenceTextureTarget(ResReferenceTexture referenceTexture, ResGraphicsFile graphicsFile);

::std::pair<ResShader,bool>
    GetReferenceShaderTarget(ResReferenceShader referenceShader, ResGraphicsFile graphicsFile);

}
}
}