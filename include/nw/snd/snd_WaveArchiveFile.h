#ifndef NW_SND_WAVE_ARCHIVE_FILE_H_
#define NW_SND_WAVE_ARCHIVE_FILE_H_

#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/snd/snd_Util.h>

namespace nw { 
namespace snd { 
namespace internal {

struct WaveArchiveFile
{
    struct InfoBlock;
    struct FileBlock;

    static const int BLOCK_SIZE = 2;

    struct FileHeader : public ut::BinaryFileHeader
    {
        Util::ReferenceWithSize toBlocks[BLOCK_SIZE];

        const InfoBlock* GetInfoBlock() const;
        const FileBlock* GetFileBlock() const;

        u32 GetInfoBlockSize() const;
        u32 GetFileBlockSize() const;
        u32 GetInfoBlockOffset() const;
        u32 GetFileBlockOffset() const;

    private:
        const Util::ReferenceWithSize* GetReferenceBy( u16 typeId ) const;
    };

    struct InfoBlockBody
    {
        Util::Table<Util::ReferenceWithSize> table;

        u32 GetWaveFileCount() const { return table.count; }
        u32 GetSize(u32 index) const{ return table.item[index].size; }
        u32 GetOffsetFromFileBlockBody(u32 index) const{ return table.item[index].offset; }

        static const u32 INVALID_OFFSET = 0xffffffff;
    };

    struct InfoBlock
    {
        ut::BinaryBlockHeader   header;
        InfoBlockBody           body;
    };

    struct FileBlockBody
    {
    };

    struct FileBlock
    {
        ut::BinaryBlockHeader   header;
        FileBlockBody           body;
    };
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_WAVE_ARCHIVE_FILE_H_