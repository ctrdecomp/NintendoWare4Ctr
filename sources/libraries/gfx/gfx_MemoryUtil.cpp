

#include <nw/gfx/gfx_MemoryUtil.h>

namespace nw{
namespace gfx{
namespace internal{

asm void copy_asm_declarations()
{
    MACRO
    COPY8WORD
    LDM r1!,{r2-r5}
    PLD [r1, #0x40]
    LDM r1!,{r9-r12}
    STM r0!,{r2-r5}
    STM r0!,{r9-r12}
    MEND

    MACRO
    COPY6WORD
    LDM r1!,{r2-r5}
    LDM r1!,{r11-r12}
    STM r0!,{r2-r5}
    STM r0!,{r11-r12}
    MEND

    MACRO
    COPY4WORD
    LDM r1!,{r2-r3}
    LDM r1!,{r11-r12}
    STM r0!,{r2-r3}
    STM r0!,{r11-r12}
    MEND

    MACRO
    COPY2WORD
    LDM r1!,{r2}
    LDM r1!,{r3}
    STM r0!,{r2}
    STM r0!,{r3}
    MEND

    MACRO
    COPYNWORD $val
    PUSH {r4-r5,r9-r11}

    LCLA count
count SETA $val
    WHILE count >= 8
count SETA count-8
    COPY8WORD
    WEND

    IF count == 2
        COPY2WORD
    ELSE
      IF count == 4
          COPY4WORD
      ELSE
          IF count == 6
              COPY6WORD
          ENDIF
      ENDIF
    ENDIF

    POP {r4-r5,r9-r11}
    BX          lr
    MEND
}

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