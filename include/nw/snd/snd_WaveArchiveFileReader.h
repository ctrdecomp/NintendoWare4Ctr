#ifndef NW_SND_WAVE_ARCHIVE_FILE_READER_H_
#define NW_SND_WAVE_ARCHIVE_FILE_READER_H_

#include <nw/snd/snd_WaveArchiveFile.h>
#include <nw/ut/ut_BinaryFileFormat.h>

namespace nw { 
namespace snd { 
namespace internal {

class WaveArchiveFileReader
{
public:
    static const u32 SIGNATURE_FILE = NW_UT_MAKE_SIGWORD('C', 'W', 'A', 'R');
    static const u32 SIGNATURE_WARC_TABLE;

    WaveArchiveFileReader(const void* pWaveArchiveFile, bool isIndividual = false);

    void InitializeFileTable();
    bool IsAvailable() const { return m_pHeader != NULL; }

    u32 GetWaveFileCount() const;
    u32 GetWaveFileSize(u32 waveIndex) const;
    u32 GetWaveFileOffsetFromFileHead(u32 waveIndex) const;

    const void* GetWaveFile(u32 waveIndex) const;

    const void* SetWaveFile(u32 waveIndex, const void* pWaveFile);
    bool IsLoaded(u32 waveIndex) const
    {
        if (GetWaveFile(waveIndex) != NULL)
        {
            return true;
        }
        return false;
    }

    bool HasIndividualLoadTable() const;

private:
    struct IndividualLoadTable
    {
        const void* waveFile[1];
    };


    const void* GetWaveFileForWhole(u32 waveIndex) const
    {
        u32 offset = m_pInfoBlockBody->GetOffsetFromFileBlockBody(waveIndex);
        return ut::AddOffsetToPtr(&m_pHeader->GetFileBlock()->body, offset);
    }
    
    const void* GetWaveFileForIndividual(u32 waveIndex) const
    {
        return m_pLoadTable->waveFile[ waveIndex ];
    }

    const WaveArchiveFile::FileHeader*      m_pHeader;
    const WaveArchiveFile::InfoBlockBody*   m_pInfoBlockBody;
    IndividualLoadTable*                    m_pLoadTable;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_WAVE_ARCHIVE_FILE_READER_H_
