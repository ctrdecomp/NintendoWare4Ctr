#ifndef NW_SND_ROM_SOUND_ARCHIVE_H_
#define NW_SND_ROM_SOUND_ARCHIVE_H_

#include <nw/snd/snd_SoundArchive.h>
#include <nw/snd/snd_SoundArchiveFileReader.h>
#include <nw/io/io_RomFileStream.h>

namespace nw {
namespace snd {

class RomSoundArchive : public SoundArchive
{
private:
    class RomFileStream;
public:
    RomSoundArchive();

    virtual ~RomSoundArchive();
    virtual size_t detail_GetRequiredStreamBufferSize() const;
    virtual const void* detail_GetFileAddress( FileId fileId ) const { return NULL; }

    bool Open(const char* filePath);
    void Close();

    size_t GetHeaderSize() const { return m_ArchiveReader.GetInfoBlockSize(); }

    bool LoadHeader(void* buffer, unsigned long size);
    size_t GetLabelStringDataSize() const { return m_ArchiveReader.GetStringBlockSize(); }

    bool LoadLabelStringData(void* buffer, unsigned long size);

protected:
    virtual io::FileStream* OpenStream(void* buffer, int size, u32 begin, u32 length);
    virtual io::FileStream* OpenExtStream(void* buffer, int size, const char* extFilePath, u32 begin, u32 length) const;

private:
    bool LoadFileHeader();

    internal::SoundArchiveFileReader m_ArchiveReader;
    nn::fs::FileReader m_FileReader;
    bool m_IsOpened;
};

class RomSoundArchive::RomFileStream : public io::RomFileStream
{
public:
    RomFileStream(const char* path, u32 offset, u32 size );
    RomFileStream(nn::fs::FileReader* fileReader, u32 offset, u32 size );

    virtual s32 Read(void* buf, u32 length);
    virtual void Seek(s32 offset, u32 origin);
    virtual u32 Tell() const { return io::RomFileStream::Tell() - m_Offset; }
    virtual u32 GetSize() const { return m_Size; }

private:
    s32 m_Offset;
    u32 m_Size;
};

} // namespace snd
} // namespace nw


#endif // NW_SND_ROM_SOUND_ARCHIVE_H_