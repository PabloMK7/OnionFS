#include "debug.h"
#ifdef DEBUG_MODE
#include "global.h"
#include "string.h"


extern FS_archive 	debugsdmcArchive;
extern u32      	fsUserHandle;
static Handle   	g_logFile = 0;
static int      	g_init = 0;
static u64      	g_offset = 0;

void    InitLog(void)
{
    if (g_init)
        return;

    // Create OnionFS.log file
    FS_path path = FS_makePath(PATH_CHAR, "/OnionFS.log");
    Result res;
	res = FSUSER_OpenArchive(fsUserHandle, &debugsdmcArchive);
	if (res != 0)
    {
        g_logFile = 0;
        return;
    }
    res = FSUSER_OpenFileDirectly(fsUserHandle, &g_logFile, debugsdmcArchive, path, 7, 0);
    if (res != 0)
    {
        g_logFile = 0;
        return;
    }
	FSFILE_SetSize(g_logFile, 0);
    g_init = 1;
    g_offset = 0;
}

void    ExitLog(void)
{
    svc_closeHandle(g_logFile);
	FSUSER_CloseArchive(fsUserHandle, &debugsdmcArchive);
    g_init = 0;
}

void    WriteLog(const char *log)
{
    if (!g_init)
        return;

    u32     size = (u32)strnlen(log, 0x100);
    u32     bytes;

	if (!FSFILE_Flush(g_logFile)) {
		if (!FSFILE_Write(g_logFile, &bytes, g_offset, (u32 *)log, size, 0x0))
			g_offset += bytes;
	}
}
#endif