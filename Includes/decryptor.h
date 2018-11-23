#include "3ds.h"

#ifdef __cplusplus
extern "C" {
#endif
void decryptdata(const u8* in, u32 size, u32 key, u8* out);
u8 rotl(u8 value, u8 shift);
#ifdef __cplusplus
}
#endif