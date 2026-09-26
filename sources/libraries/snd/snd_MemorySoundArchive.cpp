// Filename: snd_MemorySoundArchive.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_MemorySoundArchive.h>
#include <cstring>

namespace nw {
namespace snd {

MemorySoundArchive::MemorySoundArchive(): 
    m_pData(NULL) 
{
}

MemorySoundArchive::~MemorySoundArchive()
{ 
}

bool MemorySoundArchive::Initialize(const void* soundArchiveData) 
{
    NW_NULL_ASSERT(soundArchiveData);
    NW_ALIGN4_ASSERT(soundArchiveData);
    m_FileReader.Initialize(soundArchiveData);
    SoundArchive::Initialize(&m_FileReader);

    const unsigned long infoBlockOffset = m_FileReader.GetInfoBlockOffset();
    const unsigned long infoBlockSize = m_FileReader.GetInfoBlockSize();

    m_FileReader.SetInfoChunk(ut::AddOffsetToPtr(soundArchiveData, infoBlockOffset));

    const unsigned long stringBlockOffset = m_FileReader.GetStringBlockOffset();
    const unsigned long stringBlockSize = m_FileReader.GetStringBlockSize();

    if (stringBlockOffset == 0xffffffff || stringBlockSize == 0xffffffff)
    {

    }
    else
    {
        m_FileReader.SetStringBlock(ut::AddOffsetToPtr(soundArchiveData, stringBlockOffset));
    }

    m_pData = soundArchiveData;

    return true;
}

void MemorySoundArchive::Finalize() 
{
    m_pData = NULL;
    m_FileReader.Finalize();
    SoundArchive::Finalize();
}

const void* MemorySoundArchive::detail_GetFileAddress(FileId fileId) const 
{
    FileInfo fileInfo;
    if (!detail_ReadFileInfo(fileId, &fileInfo))
    {
        return NULL;
    }

    return ut::AddOffsetToPtr(m_pData,
        fileInfo.offsetFromFileBlockHead + m_FileReader.GetFileBlockOffset());
}

MemorySoundArchive::MemoryFileStream::MemoryFileStream(const void* buffer, u32 size): 
    m_pBuffer(buffer), 
    m_Size(size), 
    m_Position(0) 
{
}

io::FileStream* MemorySoundArchive::OpenStream(void* buffer, int size, u32 begin, u32 length) const 
{
    if (m_pData == NULL) 
    {
        return NULL;
    }

    if (size < sizeof(MemoryFileStream)) 
    {
        return NULL;
    }

    return new (buffer) MemoryFileStream(ut::AddOffsetToPtr(m_pData, begin), length);
}

io::FileStream* MemorySoundArchive::OpenExtStream(void* buffer, int size,
    const char* extFilePath, u32 begin, u32 length) const 
{

NW_UNUSED_VARIABLE(buffer);
NW_UNUSED_VARIABLE(size);
NW_UNUSED_VARIABLE(extFilePath);
NW_UNUSED_VARIABLE(begin);
NW_UNUSED_VARIABLE(length);

    NW_WARNING(false, "Cannot OpenExtStream for MemorySoundArchive\n");

    return NULL;
}

void MemorySoundArchive::MemoryFileStream::Close() 
{
    m_pBuffer = NULL;
    m_Size = 0;
    m_Position = 0;
}

s32 MemorySoundArchive::MemoryFileStream::Read(void* pDst, u32 size) 
{
    u32 bytesRead = ut::Min<u32>(size, m_Size - m_Position);
    std::memcpy(pDst, ut::AddOffsetToPtr(m_pBuffer, m_Position), bytesRead);

    return bytesRead;
}

void MemorySoundArchive::MemoryFileStream::Seek(s32 offset, u32 origin) 
{
    switch (origin) 
    {
    case SEEK_SET: 
    {
        m_Position = offset;
        break;
    }

    case SEEK_CUR: 
    {
        m_Position += offset;
        break;
    }

    case SEEK_END: 
    {
        m_Position = m_Size - offset;
        break;
    }

    default: 
    {
        NW_WARNING("Unsupported Seek origin");
        return;
    }
    }
}

} // namespace snd
} // namespace nw