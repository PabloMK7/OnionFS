u32 getCurrentProcessId();
u32 getCurrentProcessHandle();
u32 protectRemoteMemory(Handle hProcess, void* addr, u32 size);
u32 protectMemory(void* addr, u32 size);