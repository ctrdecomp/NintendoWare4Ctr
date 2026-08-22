// Filename: io_FileStream.cpp
//
// Project: NintendoWare4Ctr

#include <nw/io/io_FileStream.h>

#include <nw/types.h>
#include <nw/assert.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw {
namespace io   {

NW_UT_RUNTIME_TYPEINFO_DEFINITION(FileStream,IOStream);

u32 FileStream::FilePosition::Skip(s32 offset){
    if (offset != 0){
        s64 position = mPosition + offset;
        
        position = ut::Clamp(position, static_cast<s64>(0), static_cast<s64>(mFileSize));
        
        mPosition = static_cast<u32>(position);
    }
    return mPosition;
}

void FileStream::FilePosition::Seek( s32 offset, u32 origin ){
    switch (origin){
    case FILE_STREAM_SEEK_BEGIN:
        mPosition = 0;
        break;
    case FILE_STREAM_SEEK_CURRENT:
        break;
    case FILE_STREAM_SEEK_END:
        mPosition = mFileSize;
        break;
    }
    (void)this->Skip(offset);
}

}
}