

#include <nw/gfx/gfx_MemoryUtil.h>

namespace nw{
namespace gfx{
namespace internal{

#define DECL_SUB(size) \
asm u32* FastWordCopyAsm_##size(u32* /* dst */, u32* /* src */) \
{ \
    COPYNWORD size \
}

DECL_SUB(2)
DECL_SUB(4)
DECL_SUB(6)
DECL_SUB(8)
DECL_SUB(10)
DECL_SUB(12)
DECL_SUB(14)
DECL_SUB(16)
DECL_SUB(18)
DECL_SUB(20)
DECL_SUB(22)
DECL_SUB(24)
DECL_SUB(26)
DECL_SUB(28)
DECL_SUB(30)
DECL_SUB(32)
DECL_SUB(34)
DECL_SUB(36)
DECL_SUB(38)
DECL_SUB(78)

#undef DECL_SUB

}
}
}