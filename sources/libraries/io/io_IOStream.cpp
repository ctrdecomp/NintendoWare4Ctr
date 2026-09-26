// Filename: io_IOStream.cpp
//
// Project: NintendoWare4Ctr

#include <nw/io/io_IOStream.h>

#include <nw/types.h>
#include <nw/assert.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw{
namespace io{

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(IOStream);

s32 IOStream::Read(void* /*buf*/, u32 /*length*/)
{
    NW_ASSERTMSG(CanRead(), "Stream don't support READ function\n");
    return 0;
}

bool IOStream::ReadAsync(void* /*buf*/,u32 /*length*/, IOStreamCallback /*callback*/, void* /*arg*/)
{
    NW_ASSERTMSG(CanRead(), "Stream don't support READ function\n");
    NW_ASSERTMSG(CanAsync(), "Stream don't support ASYNC function\n");
    return false;
}

s32 IOStream::Write(const void* /*buf*/, u32 /*length*/)
{
    NW_ASSERTMSG(CanWrite(), "Stream don't support WRITE function\n");
    return 0;
}


bool IOStream::WriteAsync(const void* /*buf*/, u32 /*length*/, IOStreamCallback /*callback*/, void* /*arg*/)
{
    NW_ASSERTMSG(CanWrite(), "Stream don't support WRITE function\n");
    NW_ASSERTMSG(CanAsync(), "Stream don't support ASYNC function\n");
    return false;
}

s32 IOStream::WaitAsync() const
{
    NW_ASSERTMSG(CanAsync(), "Stream don't support ASYNC function\n" );
    
    while (IsBusy())
    {

    }
    return m_AsyncResult;
}

bool IOStream::IsBusy() const
{
    NW_ASSERTMSG(CanAsync(), "Stream don't support ASYNC function\n");
    return false;
}

}
}