#include "global.h"



void initSharedFunc() {

	INIT_SHARED_FUNC(showDbg, 0);
	INIT_SHARED_FUNC(nsDbgPrint, 1);
	INIT_SHARED_FUNC(plgRegisterMenuEntry, 2);
	INIT_SHARED_FUNC(plgGetSharedServiceHandle, 3);
	INIT_SHARED_FUNC(plgRequestMemory, 4);
	INIT_SHARED_FUNC(plgRegisterCallback, 5);
	INIT_SHARED_FUNC(xsprintf, 6);
}
