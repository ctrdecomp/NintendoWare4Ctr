#ifndef NW_SND_MEMORY_SOUND_ARCHIVE_H
#define NW_SND_MEMORY_SOUND_ARCHIVE_H

#include <nw/snd/snd_SoundArchiveFileReader.h>
#include <nw/io/io_FileStream.h>

namespace nw {
namespace io {
    class FileStream;
}
namespace snd {

class MemorySoundArchive : public SoundArchive 
{
private:
    class MemoryFileStream;

public:
    MemorySoundArchive();
    virtual ~MemorySoundArchive();

    virtual size_t detail_GetRequiredStreamBufferSize() const;
    virtual const void* detail_GetFileAddress(FileId fileId) const;

    bool Initialize(const void* soundArchiveData);
    void Finalize();

protected:
    virtual io::FileStream* OpenStream(void* buffer, int size, u32 begin,
        u32 length) const;

    virtual io::FileStream* OpenExtStream(void* buffer, int size,
        const char* extFilePath, u32 begin, u32 length) const;

private:
    const void* m_pData;
    internal::SoundArchiveFileReader m_FileReader;
};

class MemorySoundArchive::MemoryFileStream : public io::FileStream
{
public:
    MemoryFileStream(const void* buffer, u32 size);

    virtual bool CanSeek() const { return true; }
    virtual bool CanCancel() const { return true; }
    virtual bool CanAsync() const { return false; }
    virtual bool CanRead() const { return true; }
    virtual bool CanWrite() const { return false; }

    virtual void Close();
    virtual s32  Read(void* buf, u32 length);
    virtual void Seek(s32 offset, u32 origim);
    virtual u32  Tell() const { return m_Position; }
    virtual u32  GetSize() const { return m_Size; }

private:
    const void* m_pBuffer;
    u32 m_Size;
    u32 m_Position;
};

} // namespace snd
} // namespace nw

#endif // NW_SND_MEMORY_SOUND_ARCHIVE_H