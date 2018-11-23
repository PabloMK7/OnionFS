#include "ptm.h"

static int ptmSysmRefCount;
static Handle ptmSysmHandle;

Result ptmSysmInit(void)
{
	if (AtomicPostIncrement(&ptmSysmRefCount)) return 0;
	Result res = srvGetServiceHandle(&ptmSysmHandle, "ptm:sysm");
	if (R_FAILED(res)) AtomicDecrement(&ptmSysmRefCount);
	return res;
}

void ptmSysmExit(void)
{
	if (AtomicDecrement(&ptmSysmRefCount)) return;
	svcCloseHandle(ptmSysmHandle);
}

Result PTMSYSM_RebootAsync(u64 timeout)
{
	Result ret;
	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = IPC_MakeHeader(0x409, 2, 0); // 0x04090080
	cmdbuf[1] = timeout & 0xffffffff;
	cmdbuf[2] = (timeout >> 32) & 0xffffffff;

	if (R_FAILED(ret = svcSendSyncRequest(ptmSysmHandle)))return ret;

	return (Result)cmdbuf[1];
}