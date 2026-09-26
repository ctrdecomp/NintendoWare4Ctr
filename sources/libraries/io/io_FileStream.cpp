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

NW_UT_RUNTIME_TYPEINFO_DEFINITION(FileStream, IOStream);

void FileStream::Seek( s32 /*offset*/, u32 /*origin*/ )
{ 
    NW_ASSERTMSG(CanSeek(), "Stream don't support SEEK function\n");
}

void FileStream::Cancel()
{
    NW_ASSERTMSG(CanCancel(), "Stream don't support CANCEL function\n");
}

bool FileStream::CancelAsync(IOStreamCallback /*callback*/, void* /*arg*/ )
{
    NW_ASSERTMSG(CanCancel(), "Stream don't support CANCEL function\n");
    NW_ASSERTMSG(CanAsync(),  "Stream don't support ASYNC function\n");
    return true;
}

u32 FileStream::FilePosition::Skip(s32 offset)
{
    if (offset != 0)
    {
        s64 position = m_Position + offset;
        
        position = ut::Clamp(position, static_cast<s64>(0), static_cast<s64>(m_FileSize));
        
        m_Position = static_cast<u32>(position);
    }
    return m_Position;
}

u32 FileStream::FilePosition::Append(s32 offset)
{
    s64 position = m_Position + offset;
    
    if (position < 0)
    {
        m_Position = 0;
    }
    else
    {
        m_Position = static_cast<u32>(position);
        m_FileSize = ut::Max(m_Position, m_FileSize);
    }
    return m_Position;
}

void FileStream::FilePosition::Seek(s32 offset, u32 origin)
{
    switch (origin)
    {
    case FILE_STREAM_SEEK_BEGIN:
        m_Position = 0;
        break;
    case FILE_STREAM_SEEK_CURRENT:
        break;
    case FILE_STREAM_SEEK_END:
        m_Position = m_FileSize;
        break;
    }
    (void)this->Skip(offset);
}

} // namespace io
} // namespace nw