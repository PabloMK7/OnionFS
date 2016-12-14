#ifndef SHARED_FUNC_H
#define SHARED_FUNC_H

#if IS_PLUGIN
#define INIT_SHARED_FUNC(name,id) rtGenerateJumpCode(((NS_CONFIG*)(NS_CONFIGURE_ADDR))->sharedFunc[id], (void*) name);rtFlushInstructionCache((void*) name, 8);
#else
#define INIT_SHARED_FUNC(name,id) (g_nsConfig->sharedFunc[id] = (u32) name)
#endif

void initSharedFunc();
u32 plgRegisterMenuEntry(u32 catalog, char* title, void* callback) ;
u32 plgGetSharedServiceHandle(char* servName, u32* handle);
u32 plgRequestMemory(u32 size);
u32 plgRegisterCallback(u32 type, void* callback, u32 param0);

void showDbg(u8* fmt, u32 v1, u32 v2);
void nsDbgPrint (const char*	fmt,	...	);

#endif