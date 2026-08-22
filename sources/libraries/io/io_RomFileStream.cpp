// Filename: io_RomFileStream.cpp
//
// Project: NintendoWare4Ctr

#include <nw/io/io_RomFileStream.h>

namespace nw {
namespace io {

RomFileStream::RomFileStream( const char* path ){
    this->Initialize();
    this->Open(path);
}

RomFileStream::RomFileStream(FileInputStream* openedFileReader,  bool closeEnable){
    this->Initialize();
    this->Open(openedFileReader, closeEnable);
}

RomFileStream::~RomFileStream(){
    if (mCloseOnDestroyFlag){
        this->Close();
    }
}

bool RomFileStream::Open(FileInputStream* openedFileReader, bool closeEnable){
    if (mCloseOnDestroyFlag){
        this->Close();
    }

    mpOpenedFileReader = openedFileReader;
    this->mFilePosition.SetFileSize(this->mpOpenedFileReader->GetSize());
    this->mFilePosition.Seek(0, FILE_STREAM_SEEK_BEGIN);

    mCloseOnDestroyFlag = false;
    mCloseEnableFlag = closeEnable;
    mIsAvailable = true;
    return true;
}

bool RomFileStream::Open(const char* path){
    NW_NULL_ASSERT(path);

    this->mFileReader.Initialize(path);
    mpOpenedFileReader = &mFileReader;
    this->mFilePosition.SetFileSize(this->mpOpenedFileReader->GetSize());
    this->mFilePosition.Seek(0, FILE_STREAM_SEEK_BEGIN);
    mCloseOnDestroyFlag = true;
    mCloseEnableFlag = true;
    mIsAvailable = true;
    return true;
}

void RomFileStream::Initialize(){
    mIsAvailable = false;
    mCloseOnDestroyFlag = false;
}

void RomFileStream::Close(){
    if (mCloseEnableFlag && mIsAvailable){
        mpOpenedFileReader->Finalize();
        mIsAvailable = false;
    }
}

s32 RomFileStream::Read( void* buf, u32 length ){
    mpOpenedFileReader->Seek(this->mFilePosition.Tell(), nn::fs::POSITION_BASE_BEGIN);
    s32 readBytes = this->mpOpenedFileReader->Read(buf, length);
    if (readBytes > 0){
        this->mFilePosition.Skip( readBytes );
    }
    return readBytes;
}

void RomFileStream::Seek( s32 offset, u32 origin ){
    this->mFilePosition.Seek(offset, origin);
}

}
}