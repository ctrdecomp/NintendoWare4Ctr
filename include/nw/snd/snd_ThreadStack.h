#ifndef NW_SND_THREAD_STACK_H_
#define NW_SND_THREAD_STACK_H_

#include <nw/types.h>

namespace nw {
namespace snd {
namespace internal {

class ThreadStack
{
public:
    ThreadStack(): 
        m_Addr(NULL), m_Size(0) 
    {
    }
    ~ThreadStack() { Finalize(); }

    void Initialize( uptr ptr, size_t size );
    void Finalize();

    uptr GetStackBottom() const { return m_Addr + m_Size; }
    uptr GetBaseAddress() const { return m_Addr; }

private:
    uptr    m_Addr;
    size_t  m_Size;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_THREAD_STACK_H_