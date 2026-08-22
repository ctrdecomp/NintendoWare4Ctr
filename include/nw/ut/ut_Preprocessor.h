#pragma once

#define NW_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);                \
    void operator=(const TypeName&)