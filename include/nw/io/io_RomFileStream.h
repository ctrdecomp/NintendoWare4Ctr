#pragma once

#include <nn/fs.h>
#include <nw/io/io_FileStream.h>

namespace nw {
namespace io {

class RomFileStream : public FileStream{
public:
    NW_UT_RUNTIME_TYPEINFO;

    RomFileStream(const char* path);
    RomFileStream(FileInputStream* openedFileReader, bool closeEnable = true );


    bool Open(const char* path);
    bool Open(FileInputStream* openedFileReader, bool closeEnable = true );

    virtual ~RomFileStream();
    virtual void Close();
    virtual s32 Read( void* buf, u32 length );
    virtual void Seek( s32 offset, u32 origin );
    virtual u32 GetSize() const { return this->mFilePosition.GetFileSize(); }
    virtual u32 Tell() const { return this->mFilePosition.Tell(); }
    virtual bool CanSeek()   const { return true; }
    virtual bool CanCancel() const { return false; }
    virtual bool CanRead()   const { return true; }
    virtual bool CanWrite()  const { return false; }
    virtual bool CanAsync()  const { return false; }

private:
    void Initialize();

    FilePosition mFilePosition;
    FileInputStream mFileReader;
    FileInputStream* mpOpenedFileReader;

    bool mIsAvailable;
    bool mCloseOnDestroyFlag;
    bool mCloseEnableFlag;
};

}
}