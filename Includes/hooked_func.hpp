#include "3DS.h"
#include "types.h"

extern "C" void getmsbtptrfromobj(u32* objref);
extern "C" Result gameGetSrvHandle(Handle* out, u32* name, u64 str_len);