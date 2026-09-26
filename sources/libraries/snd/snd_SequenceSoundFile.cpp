// Filename: snd_SequenceSoundFile.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SequenceSoundFile.h>
#include <cstring>      // strlen, strncmp

namespace nw {
namespace snd {
namespace internal {

/* SequenceSoundFile::FileHeader */

const SequenceSoundFile::DataBlock* SequenceSoundFile::FileHeader::GetDataBlock() const
{
    return reinterpret_cast<const DataBlock*>(GetBlock(20480));
}
const SequenceSoundFile::LabelBlock* SequenceSoundFile::FileHeader::GetLabelBlock() const
{
    return reinterpret_cast<const LabelBlock*>(GetBlock(20481));
}

/* SequenceSoundFile::LabelBlockBody */

const SequenceSoundFile::LabelInfo* SequenceSoundFile::LabelBlockBody::GetLabelInfo(int index) const
{
    NW_ASSERT(index < GetLabelCount());
    
    return reinterpret_cast<const LabelInfo*>(
        labelInfoReferenceTable.GetReferedItem(index));
}

const char* SequenceSoundFile::LabelBlockBody::GetLabel( int index ) const
{
    const LabelInfo* labelInfo = GetLabelInfo( index );
    return labelInfo->label;
}

const char* SequenceSoundFile::LabelBlockBody::GetLabelByOffset( u32 offset ) const
{
    for (int i = 0; i < GetLabelCount(); i++)
    {
        const LabelInfo* labelInfo = GetLabelInfo(i);
        if (labelInfo->referToSequenceData.offset == offset)
        {
            return labelInfo->label;
        }
    }
    return NULL;
}

bool SequenceSoundFile::LabelBlockBody::GetOffset(int index, u32* offsetPtr) const
{
    const LabelInfo* labelInfo = GetLabelInfo(index);
    *offsetPtr = labelInfo->referToSequenceData.offset;
    return true;
}

bool SequenceSoundFile::LabelBlockBody::GetOffsetByLabel( const char* label, u32* offsetPtr ) const
{
    const std::size_t labelLength = std::strlen( label );

    for (int i = 0; i < GetLabelCount(); i++)
    {
        const LabelInfo* labelInfo = GetLabelInfo(i);
        if (std::strncmp(label, labelInfo->label, labelLength) == 0)
        {
            *offsetPtr = labelInfo->referToSequenceData.offset;
            return true;
        }
    }
    return false;
}


} // namespace internal
} // namespace snd
} // namespace nw

