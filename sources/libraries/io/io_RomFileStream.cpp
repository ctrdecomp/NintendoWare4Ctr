// Filename: io_RomFileStream.cpp
//
// Project: NintendoWare4Ctr

#include <nw/io/io_RomFileStream.h>

namespace nw {
namespace io {

RomFileStream::RomFileStream(const char* path)
{
    this->Initialize();
    this->Open(path);
}

RomFileStream::RomFileStream(FileReader* openedFileReader,  bool closeEnable)
{
    this->Initialize();
    this->Open(openedFileReader, closeEnable);
}

RomFileStream::~RomFileStream()
{
    if (m_CloseOnDestroyFlag)
    {
        this->Close();
    }
}

bool RomFileStream::Open(FileReader* openedFileReader, bool closeEnable)
{
    if (m_CloseOnDestroyFlag)
    {
        Close();
    }

    m_pOpenedFileReader = openedFileReader;
    m_FilePosition.SetFileSize(m_pOpenedFileReader->GetSize());
    m_FilePosition.Seek(0, FILE_STREAM_SEEK_BEGIN);

    m_CloseOnDestroyFlag = false;
    m_CloseEnableFlag = closeEnable;
    m_IsAvailable = true;
    return true;
}

bool RomFileStream::Open(const char* path)
{
    NW_NULL_ASSERT(path);

    m_FileReader.Initialize(path);
    m_pOpenedFileReader = &m_FileReader;
    m_FilePosition.SetFileSize(m_pOpenedFileReader->GetSize());
    m_FilePosition.Seek(0, FILE_STREAM_SEEK_BEGIN);
    m_CloseOnDestroyFlag = true;
    m_CloseEnableFlag = true;
    m_IsAvailable = true;
    return true;
}

void RomFileStream::Initialize()
{
    m_IsAvailable = false;
    m_CloseOnDestroyFlag = false;
}

void RomFileStream::Close()
{
    if (m_CloseEnableFlag && m_IsAvailable)
    {
        m_pOpenedFileReader->Finalize();
        m_IsAvailable = false;
    }
}

s32 RomFileStream::Read(void* buf, u32 length)
{
    m_pOpenedFileReader->Seek(m_FilePosition.Tell(), nn::fs::POSITION_BASE_BEGIN);
    s32 readBytes = m_pOpenedFileReader->Read(buf, length);
    if (readBytes > 0)
    {
        m_FilePosition.Skip(readBytes);
    }
    return readBytes;
}

void RomFileStream::Seek(s32 offset, u32 origin)
{
    m_FilePosition.Seek(offset, origin);
}

}
}