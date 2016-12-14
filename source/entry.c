#include "global.h"

extern u32 __c_bss_start;
extern u32 __c_bss_end;

void c_entry(u32* reg) {
	u32 i;

	for (i = __c_bss_start; i < __c_bss_end; i += 4){
		*(vu32*)(i) = 0;
	}
	main();
}

void IRQHandler (void)
{
	
}
