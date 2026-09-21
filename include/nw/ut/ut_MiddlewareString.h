#ifndef NW_UT_MIDDLEWARESTRING_H
#define NW_UT_MIDDLEWARESTRING_H

#include <nw/assert.h>
#include <nw/version.h>
#include <nn/module.h>

#define NW_MACROTO_STR_IMPL(x)          #x
#define NW_MACROTO_STR(macroArg)  NW_MACROTO_STR_IMPL(macroArg)

#define NW_MODULE_SYMBOL(module_name) "NW4C_" NW_MACROTO_STR(NW_VERSION_MAJOR) "_" NW_MACROTO_STR(NW_VERSION_MINOR) "_" NW_MACROTO_STR(NW_VERSION_MICRO) "_" #module_name
#define NW_PUT_MODULE_SYMBOL(module_name)                                            \
NN_MAKE_MODULE(moduleInfo, "NINTENDO", NW_MIDDLEWARE_SYMBOL(module_name));         \
NN_REFER_MODULE(moduleInfo);                                                         \

#endif // NINTENDOWARE_UT_MIDDLEWARESTRING_H
