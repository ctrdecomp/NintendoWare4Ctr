#pragma once

#include <nw/io/io_IOStream.h>

namespace nw   {
namespace io     {

enum{
    FILE_STREAM_SEEK_BEGIN,
    FILE_STREAM_SEEK_CURRENT,
    FILE_STREAM_SEEK_END
};

class FileStream : public IOStream{
public:
    NW_UT_RUNTIME_TYPEINFO;

    virtual u32 GetSize() const = 0;
    virtual void Seek(s32 offset, u32 origin);
    virtual void Cancel(void);
    virtual bool CancelAsync(IOStreamCallback callback, void* arg);
    
    virtual bool CanSeek (void) const = 0;
    virtual bool CanCancel(void) const = 0;
    virtual u32 Tell()  const = 0;
    bool IsEof() const { return Tell() >= GetSize();  }

protected:
    FileStream(): 
        IOStream() 
    {}
    class FilePosition{
    public:
        FilePosition() : mFileSize(0), mPosition(0) {}
        void SetFileSize(u32 fileSize) { mFileSize = fileSize; }
        u32 GetFileSize() const { return mFileSize; }
        
        u32 Skip(s32 offset);
        u32 Append(s32 offset);
        void Seek(s32 offset, u32 origin);
        u32 Tell() const { return mPosition; }
    private:
        u32 mFileSize;
        u32 mPosition;
    };
};


}
}