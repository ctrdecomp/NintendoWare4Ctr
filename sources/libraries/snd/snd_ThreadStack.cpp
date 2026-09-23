// Filename: snd_ThreadStack.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_ThreadStack.h>
#include <nw/Assert.h>

namespace nw {
namespace snd {
namespace internal {

void ThreadStack::Initialize(uptr ptr, size_t size)
{
    NW_ASSERT(!m_Addr);

    m_Addr = ptr;
    m_Size = size;
}

void ThreadStack::Finalize()
{
    m_Addr = 0;
    m_Size = 0;
}

} // namespace internal
} // namespace snd
} // namespace nw