// Filename: snd_RomSoundArchive.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_RomSoundArchive.h>
#include <nw/snd/snd_Util.h>

namespace nw {
namespace snd {

RomSoundArchive::RomSoundArchive(): 
    m_IsOpened(false)
{
}

RomSoundArchive::~RomSoundArchive()
{
    Close();
}

bool RomSoundArchive::Open(const char* path)
{
    {
        m_FileReader.Initialize(path);
        m_IsOpened = true;
    }

    {
        bool result = LoadFileHeader();
        if (result == false)
        {
            NW_WARNING(false, "Cannot load header\n");
            return false;
        }
    }

    std::size_t len = std::strlen(path);
    for (int i = static_cast<int>(len) - 1; i >= 0; i--)
    {
        const char ch = path[i];
        if (ch == '/' || ch == '\\')
        {
            char dirBuffer[FILE_PATH_MAX];
            NW_ASSERT(i < FILE_PATH_MAX);
            if (i >= FILE_PATH_MAX)
            {
                return false;
            }

            ut::strncpy(dirBuffer, FILE_PATH_MAX, path, static_cast<u32>(i));
            dirBuffer[i] = '\0';

            SetExternalFileRoot( dirBuffer );
            break;
        }
    }

    return true;
}

void RomSoundArchive::Close()
{
    if (m_IsOpened)
    {
        m_FileReader.Finalize();
        m_ArchiveReader.Finalize();
        m_IsOpened = false;
    }

    Finalize();
}

io::FileStream* RomSoundArchive::OpenStream(void* buffer, int size, u32 begin, u32 length)
{
    if (!m_IsOpened)
    {
        return NULL;
    }
    if (size < sizeof(RomFileStream)) 
    {
        return NULL;
    }
    RomFileStream* stream = new(buffer) RomFileStream(&m_FileReader, begin, length);
    return stream;
}

io::FileStream* RomSoundArchive::OpenExtStream(
    void* buffer,int size, const char* extFilePath,
    u32 begin, u32 length) const
{
    if (!m_IsOpened)
    {
        return NULL;
    }
    if (size < sizeof(RomFileStream)) 
    {
        return NULL;
    }

    RomFileStream* stream = new(buffer) RomFileStream(extFilePath, begin, length);
    return stream;
}

size_t RomSoundArchive::detail_GetRequiredStreamBufferSize() const
{
    return sizeof(RomFileStream);
}

bool RomSoundArchive::LoadFileHeader()
{
    NW_ASSERT( m_IsOpened );

    const unsigned long headerAlignSize = static_cast<unsigned long>(
        ut::RoundUp(sizeof(internal::SoundArchiveFile::FileHeader), 32)
    );
    u8 headerArea[sizeof(internal::SoundArchiveFile::FileHeader) + 32 * 2];
    void* file = ut::RoundUp(headerArea, 32);

    s32 readSize = m_FileReader.Read(file, static_cast<s32>(headerAlignSize));

    if (readSize != headerAlignSize)
    {
        NW_WARNING(false, "RomSoundArchive::LoadFileHeader cannot read file.\n");
        return false;
    }

    m_ArchiveReader.Initialize(file);
    Initialize(&m_ArchiveReader);

    return true;
}

bool RomSoundArchive::LoadHeader(void* buffer, unsigned long size)
{
    NW_ASSERT(m_IsOpened);

    const s32 infoChunkOffset = m_ArchiveReader.GetInfoBlockOffset();
    const u32 infoChunkSize = m_ArchiveReader.GetInfoBlockSize();

    if (size < infoChunkSize)
    {
        NW_WARNING(size >= infoChunkSize, "RomSoundArchive::LoadHeader buffer size is too small.\n");
        return false;
    }

    m_FileReader.Seek(infoChunkOffset, nn::fs::POSITION_BASE_BEGIN);
    s32 readSize = m_FileReader.Read(buffer, static_cast<s32>(infoChunkSize));

    if (readSize != infoChunkSize)
    {
        NW_WARNING(false, "RomSoundArchive::LoadHeader cannot read file.\n");
        return false;
    }

    m_ArchiveReader.SetInfoBlock(buffer);

    return true;
}

bool RomSoundArchive::LoadLabelStringData(void* buffer, unsigned long size)
{
    NW_ASSERT(m_IsOpened);

    const s32 stringBlockOffset = m_ArchiveReader.GetStringBlockOffset();
    const u32 stringBlockSize = m_ArchiveReader.GetStringBlockSize();

    if(stringBlockOffset == internal::Util::Reference::INVALID_OFFSET)
    {
        return false;
    }

    if(size < stringBlockSize)
    {
        NW_WARNING(size >= stringBlockSize, "RomSoundArchive::LoadLabelStringData buffer size is too small.");
        return false;
    }

    m_FileReader.Seek(stringBlockOffset, nn::fs::POSITION_BASE_BEGIN);
    s32 readSize = m_FileReader.Read(buffer, static_cast<s32>(stringBlockSize));

    if(readSize != stringBlockSize)
    {
        NW_WARNING(false, "RomSoundArchive::LoadLabelStringData cannot read file.\n");
        return false;
    }

    m_ArchiveReader.SetStringBlock( buffer);

    return true;
}

/* RomSounrArchive::RomFileStream */

RomSoundArchive::RomFileStream::RomFileStream(const char* path, u32 offset, u32 size): 
    io::RomFileStream(path),
    m_Offset(static_cast<s32>(offset)),
    m_Size(size)
{
    NW_ASSERT(m_Size <= io::RomFileStream::GetSize());
    if(size == 0)
    {
        m_Size = io::RomFileStream::GetSize();
    }
    io::RomFileStream::Seek(m_Offset, io::FILE_STREAM_SEEK_BEGIN);
}


RomSoundArchive::RomFileStream::RomFileStream(
    nn::fs::FileReader* fileReader, u32 offset, u32 size)
: 
    io::RomFileStream(fileReader, false),
    m_Offset(static_cast<s32>(offset)),
    m_Size(size)
{
    NW_ASSERT(m_Size <= io::RomFileStream::GetSize());
    if(size == 0)
    {
        m_Size = io::RomFileStream::GetSize();
    }
    io::RomFileStream::Seek(m_Offset, io::FILE_STREAM_SEEK_BEGIN);
}

s32 RomSoundArchive::RomFileStream::Read(void* buf, u32 length)
{
    NW_ALIGN32_ASSERT(buf);
    NW_ALIGN32_ASSERT(length);

    u32 curPos = io::RomFileStream::Tell();
    if(curPos + length > m_Offset + m_Size) 
    {
        length = static_cast<u32>(ut::RoundUp(m_Offset + m_Size - curPos, 32));
    }
    return io::RomFileStream::Read(buf, length);
}

void RomSoundArchive::RomFileStream::Seek(s32 offset, u32 origin)
{
    switch(origin) 
    {
    case io::FILE_STREAM_SEEK_BEGIN:
        offset += m_Offset;
        break;
    case io::FILE_STREAM_SEEK_CURRENT:
        offset += io::RomFileStream::Tell();
        break;
    case io::FILE_STREAM_SEEK_END:
        offset = m_Offset + static_cast<s32>(m_Size) - offset;
        break;
    default:
        NW_ASSERTMSG(false, "Unsupported Seek origin");
        return;
    }

    if(offset < m_Offset)
    {
        offset = m_Offset;
    }
    else if(offset > m_Offset + static_cast<s32>(m_Size))
    {
        offset = m_Offset + static_cast<s32>(m_Size);
    }

    io::RomFileStream::Seek(offset, io::FILE_STREAM_SEEK_BEGIN);
}

} // namespace snd
} // namespace nw


// ProductSequenceはHawkoniwaSequenceよりも優れています。
//
// 『スーパーマリオ 3Dランド』のデバッグシーケンスはいつ？
//
// ユーザーLuigiFan27がここを訪れました。
//
// これからシーケンス・サウンド・シリーズに取り掛かります。
//
// ルイージファンより
//
// 日本語＝かっこいい！