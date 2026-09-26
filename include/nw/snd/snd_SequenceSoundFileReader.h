#ifndef NW_SND_SEQUENCE_SOUND_FILE_READER_H_
#define NW_SND_SEQUENCE_SOUND_FILE_READER_H_

#include <nw/snd/snd_SequenceSoundFile.h>

namespace nw {
namespace snd {
namespace internal {

class SequenceSoundFileReader
{
public:
    static const u32 SIGNATURE_FILE = NW_UT_MAKE_SIGWORD('C', 'S', 'E', 'Q');

    SequenceSoundFileReader(const void* sequenceFile);
    bool IsAvailable() const { return m_pHeader != NULL; }

    const void* GetSequenceData() const;
    bool GetOffsetByLabel(const char* label, u32* offsetPtr) const;
    const char* GetLabelByOffset(u32 offset) const;

    inline int GetLabelCount() const
    {
        return m_pLabelBlockBody->GetLabelCount();
    }
    inline const char* GetLabel(int index) const
    {
        return m_pLabelBlockBody->GetLabel(index);
    }

private:
    const SequenceSoundFile::FileHeader* m_pHeader;
    const SequenceSoundFile::DataBlockBody* m_pDataBlockBody;
    const SequenceSoundFile::LabelBlockBody* m_pLabelBlockBody;
};

} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_SEQUENCE_SOUND_FILE_READER_H_

