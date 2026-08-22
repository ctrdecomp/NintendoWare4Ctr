// Filename: ut_ResDictionary.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_ResDictionary.h>

namespace nw{ 
namespace ut{ 

ResDicPatriciaData::ResDicNodeData* ResDicPatricia::Get(const ResName rhs) const{
    size_t len = rhs.GetLength();
    const char* s = rhs.GetName();
    const ResDataType& r = ref();

    const ResDicPatriciaData::ResDicNodeData* p;
    const ResDicPatriciaData::ResDicNodeData* x;

    p = &r.data[0];
    x = &r.data[p->idxLeft];

    while(p->ref > x->ref){
        p = x;

        u32 wd = u32(x->ref) >> 3;
        u32 pos = u32(x->ref) & 7;

        if (wd < len && ((s[wd] >> pos) & 1)){
            x = &r.data[x->idxRight];
        }
        else{
            x = &r.data[x->idxLeft];
        }
    }
    if (rhs == ResName(((u8*)&r + x->ofsString - sizeof(u32)))){
        return const_cast<ResDicPatriciaData::ResDicNodeData*>(x);
    }
    return NULL;
}



}
}