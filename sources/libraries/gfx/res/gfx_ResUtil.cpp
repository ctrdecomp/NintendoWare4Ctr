#include <nw/gfx/res/gfx_ResUtil.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>

namespace nw {
namespace gfx {
namespace res {

Result SetupReferenceLut(ResReferenceLookupTable resReferenceLut, ResGraphicsFile graphicsFile)
{
    Result result = RESOURCE_RESULT_OK;

    if (resReferenceLut.IsValid())
    {
        ResImageLookupTable resImageLut = ResDynamicCast<ResImageLookupTable>(resReferenceLut.GetTargetLut());
        if (!resImageLut.IsValid())
        {
            ::std::pair<ResLookupTable, bool> referenceResult;
            referenceResult = GetReferenceLutTarget(resReferenceLut, graphicsFile);

            if (!referenceResult.second)
            {
                result |= Result::MASK_FAIL_BIT;
                result |= RESOURCE_RESULT_NOT_FOUND_LUT;
            }
        }
    }

    return result;
}

::std::pair<ResLookupTable, bool> 
GetReferenceLutTarget(ResReferenceLookupTable referenceLut, ResGraphicsFile graphicsFile)
{
    bool isExisted = false;
    const char* path = referenceLut.GetPath();
    const char* tableName = referenceLut.GetTableName();
    int lutSetNum = graphicsFile.GetLutSetsCount();
    
    ResLookupTableSet resLutSet = graphicsFile.GetLutSets(path);

    if (resLutSet.IsValid())
    {
        ResImageLookupTable resLut = ResDynamicCast<ResImageLookupTable>(resLutSet.GetSamplers(tableName));

        if (resLut.IsValid())
        {
            referenceLut.ref().toTargetLut.set_ptr(reinterpret_cast<const void*>(resLut.ptr()));
            isExisted = true;
        }
    }

    return ::std::make_pair(referenceLut.GetTargetLut(), isExisted);
}

::std::pair<ResTexture, bool>
GetReferenceTextureTarget(ResReferenceTexture referenceTexture, ResGraphicsFile graphicsFile)
{
    bool isExisted = false;
    const char* path = referenceTexture.GetPath();
    ResTexture resTexture = graphicsFile.GetTextures(path);
    
    if (resTexture.IsValid())
    {
        ResReferenceTexture refer = ResDynamicCast<ResReferenceTexture>(resTexture);

        if (refer.IsValid())
        {
            ::std::pair<ResTexture, bool> referenceResult;
            referenceResult = GetReferenceTextureTarget(refer, graphicsFile);
            if (referenceResult.second)
            {
                referenceTexture.ref().toTargetTexture.set_ptr(reinterpret_cast<const void*>(referenceResult.first.ptr()));
                isExisted = true;
            }
        }
        else{
            referenceTexture.ref().toTargetTexture.set_ptr(reinterpret_cast<const void*>(resTexture.ptr()));
            isExisted = true;
        }
    }

    return ::std::make_pair(referenceTexture.GetTargetTexture(), isExisted);
}

::std::pair<ResShader, bool>
GetReferenceShaderTarget(ResReferenceShader referenceShader, ResGraphicsFile graphicsFile)
{
    bool isExisted = false;
    const char* path = referenceShader.GetPath();
    ResShader resShader = graphicsFile.GetShaders(path);

    if (resShader.IsValid())
    {
        ResReferenceShader refer = ResDynamicCast<ResReferenceShader>(resShader);

        if (refer.IsValid())
        {
            ::std::pair<ResShader, bool> referenceResult;
            referenceResult = GetReferenceShaderTarget(refer, graphicsFile);
            if (referenceResult.second)
            {
                referenceShader.ref().toTargetShader.set_ptr(reinterpret_cast<const void*>(referenceResult.first.ptr()));
                isExisted = true;
            }
        }
        else{
            referenceShader.ref().toTargetShader.set_ptr(reinterpret_cast<const void*>(resShader.ptr()));
            isExisted = true;
        }
    }

    return ::std::make_pair(referenceShader.GetTargetShader(),isExisted);
}

}
}
}