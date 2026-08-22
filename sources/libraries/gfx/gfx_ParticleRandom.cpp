#include <nw/gfx/gfx_ParticleRandom.h>

namespace nw{
namespace gfx{

asm f32 ParticleRandom::NextFloatSignedOne(){
    LDR      r2, [r0, #__cpp(offsetof(ParticleRandom, mSeed))]
    LDR      r3, mix1
    LDR      r1, mix2
    MLA      r1, r2, r3, r1
    LDR      r3, mask
    STR      r1, [r0, #0]
    AND      r2,r3,r1,LSR #16
    CLZ      r0, r2
    AND      r12, r1, #0x80000000
    LSL      r2, r2, r0
    RSB      r1, r0, #142
    ADD      r2, r12, r2, LSR #8
    ADD      r2, r2, r1, LSL #23
    VMOV     s0, r2

    BX       lr

mix1 DCD      0x343fd
mix2 DCD      0x269ec3
mask DCD      0x00007fff
}

asm f32 ParticleRandom::NextFloatSignedHalf(){
    LDR      r2, [r0, #__cpp(offsetof(ParticleRandom, mSeed))]
    LDR      r3, mixh1
    LDR      r1, mixh2
    MLA      r1, r2, r3, r1
    LDR      r3, maskh
    STR      r1, [r0, #0]
    AND      r2,r3,r1,LSR #16
    CLZ      r0, r2
    AND      r12, r1, #0x80000000
    LSL      r2, r2, r0
    RSB      r1, r0, #141
    ADD      r2, r12, r2, LSR #8
    ADD      r2, r2, r1, LSL #23
    VMOV     s0, r2
    BX       lr

mixh1 DCD      0x343fd
mixh2 DCD      0x269ec3
maskh DCD      0x00007fff
}


}
}