#pragma once
#include <stdlib.h>
#include <types.h>
#include <ctrulib/result.h>
#include <ctrulib/svc.h>
#include <ctrulib/srv.h>
#include <ctrulib/synchronization.h>
#include <ctrulib/ipc.h>
#ifdef __cplusplus
extern "C" {
#endif
Result ptmSysmInit(void);
Result PTMSYSM_RebootAsync(u64 timeout);
void ptmSysmExit(void);
#ifdef __cplusplus
}
#endif