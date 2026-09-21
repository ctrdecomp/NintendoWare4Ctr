#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw{
namespace gfx{

class Material;

class IMaterialIdGenerator : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(IMaterialIdGenerator);

public:
    NW_UT_RUNTIME_TYPEINFO;
    
    virtual void Accept(Material* material) = 0;
    virtual void Generate() = 0;

protected:
    IMaterialIdGenerator(nw::os::IAllocator* allocator) : 
        GfxObject(allocator) {}

};

}
}