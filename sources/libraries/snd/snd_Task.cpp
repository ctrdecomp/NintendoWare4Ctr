// Filename: snd_Task.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Task.h>

namespace nw {
namespace snd {
namespace internal {

Task::Task(): 
    m_Event(true),
    m_Status(STATUS_FREE),
    m_Id(0)
{
    m_Event.Signal();
}

Task::~Task()
{
    NW_ASSERT(m_Status != STATUS_APPEND && m_Status != STATUS_EXECUTE);
}

} // namespace internal
} // namespace snd
} // namespace nw