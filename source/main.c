
#include "global.h"
#include "string.h"
#include "autogen.h"

/* uncomment this to log filename into debugger output */
// #define LOG_FILES
// #define DEBUG_MODE

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR   3
#define O_CREAT  4
#define REG(x)   (*(volatile u32*)(x))
#define REG8(x)  (*(volatile  u8*)(x))
#define REG16(x) (*(volatile u16*)(x))
#define SW(addr, data)  *(u32*)(addr) = data
#define READU8(addr) *(volatile unsigned char*)(addr)
#define READU16(addr) *(volatile unsigned short*)(addr)
#define READU32(addr) *(volatile unsigned int*)(addr)
#define WRITEU8(addr, data) *(vu8*)(addr) = data
#define WRITEU16(addr, data) *(vu16*)(addr) = data
#define WRITEU32(addr, data) *(vu32*)(addr) = data
#define R_SUCCEEDED(res)   ((res)>=0)
#define R_FAILED(res)      ((res)<0)
#define custompath "yourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyou"
#define defaultfolder "ram:/OnionFS/"
u32 fsMountArchive;
u32 fsRegArchive;
u32 userFsTryOpenFile;
u32 cfgReadBlock = 0x0;


FS_archive sdmcArchive = {0x9, (FS_path){PATH_EMPTY, 1, (u8*)""}};

typedef u32 (*fsRegArchiveTypeDef)(u8*, u32, u32, u32) ;
typedef u32 (*userFsTryOpenFileTypeDef)(u32, u16*, u32);
typedef u32 (*fsMountArchiveTypeDef)(u32*, u32);

PLGLOADER_INFO  *plgloader;
RT_HOOK	regArchiveHook;
RT_HOOK userFsTryOpenFileHook;
u32     fsUserHandle = 0;
u32 maxcodesize;

#ifdef DEBUG_MODE
char debugstring[100];
char debugtempstring[20];
#endif

int     InitFS(void)
{
    static int init = 0;

    if (init != 0)
    {
        svc_closeHandle(fsUserHandle);
        init = 0;
    }

    int res = srv_getServiceHandle(NULL, &fsUserHandle, "fs:USER");

    if (res != 0)
        return (res);

    res = FSUSER_Initialize(fsUserHandle);
    if (res != 0)
        svc_closeHandle(fsUserHandle);
    else
        init = 1;

    return (res);
}

void strCat(u8* src, u8* str) {
	while(*src) {
		src ++;
	}
	while(*str) {
		*src = *str;
		str ++;
		src ++;
	}
	*src = 0;
}

void ustrCat(u16* src, u16* str) {
	while(*src) {
		src ++;
	}
	while(*str) {
		*src = *str;
		str ++;
		src ++;
	}
	*src = 0;
}

u8 archivelist[200];
u8 archivecount = 0;

u8 checkarchive(u8* path, u8 size) {
	u8 i;
	for (i = 0; i < archivecount; i++) {
		if (memcmp(archivelist + (10*i), path, size) == 0)
			return 1;
	}
	return 0;
}

void addarchive(u8* path) {
	u8 len = 0;
	while(*(path + len)) len++;
	if (!(checkarchive(path, len)) && (archivecount < 20)) {
		strCat(archivelist + (10*archivecount), path);
		archivecount++;
	}
}

u32 fsRegArchiveCallback(u8* path, u32 ptr, u32 isAddOnContent, u32 isAlias) {
	u32 ret, ret2;
	static u32 isFisrt = 1;
	u32 sdmcArchive = 0;

	nsDbgPrint("regArchive: %s, %08x, %08x, %08x\n", path, ptr, isAddOnContent, isAlias);
	ret = ((fsRegArchiveTypeDef)regArchiveHook.callCode)(path, ptr, isAddOnContent, isAlias);
	addarchive(path);
	if (isFisrt) {
		isFisrt = 0;
		
		((fsMountArchiveTypeDef)fsMountArchive)(&sdmcArchive, 9);
		#ifdef LOG_FILES
		nsDbgPrint("sdmcArchive: %08x\n", sdmcArchive);
		#endif
		
		if (sdmcArchive) {
			ret2 = ((fsRegArchiveTypeDef)regArchiveHook.callCode)("ram:", sdmcArchive, 0, 0);
			#ifdef LOG_FILES
			nsDbgPrint("regArchive ret: %08x\n", ret2);
			#endif
		}
	}
	return ret;
}

/* 
such convert
much danger
need to be fixed
wow 
*/
void convertUnicodeToAnsi(u16* ustr, u8* buf) {
	while(*ustr != 0) {
		u16 wch = *ustr;
		*buf = (char) wch;
		ustr ++;
		buf ++;
	}
	*buf = 0;
}

void convertAnsiToUnicode(u8* ansi, u16* buf) {
	while(*ansi != 0) {
		u8 ch = *ansi;
		*buf = ch;
		ansi ++;
		buf ++;
	}
	*buf = 0;
}

u16 testFile []= {'r','a','m',':','/','1',0,0};
u16 ustrRom [] = {'r','o','m',':','/'};
u16 ustrRootPath[200];

u8 findu8character(u8* in, char character) {
	u8 i = 0;
	while ((*(in + i)) && (*(in + i) != character)) i++;
	return (*(in + i) == character) * i; //error handling
}

u32 userFsTryOpenFileCallback(u32 a1, u16 * fileName, u32 mode) {
	u16 buf[300];
	u32 ret;


	convertUnicodeToAnsi(fileName, (u8*) buf);
	u8 chara = findu8character((u8*) buf, '/');
#ifdef LOG_FILES
	nsDbgPrint("path: %s\n", buf);
#endif

	if ((chara) && checkarchive((u8*) buf, chara)) {
		// accessing rom:/ file
		buf[0] = 0;
		ustrCat(buf, ustrRootPath);
		ustrCat(buf, &fileName[chara + 1]);
		ret = ((userFsTryOpenFileTypeDef)userFsTryOpenFileHook.callCode)(a1, buf, 1);
#ifdef LOG_FILES
		nsDbgPrint("ret: %08x\n", ret);
#endif
		if (ret == 0) {
			return ret;
		}
	}
	return ((userFsTryOpenFileTypeDef)userFsTryOpenFileHook.callCode)(a1, fileName, mode);
}

u32 findNearestSTMFD(u32 newaddr) {
	u32 i;
	for (i = 0; i < 1024; i ++) {
		newaddr -= 4;
		if (READU16(newaddr + 2) == 0xE92D) {
			return newaddr;
		}
	}
	return 0;
}
void getvalues(void) {
	fsMountArchive = 0x0;
	fsRegArchive = 0x0;
	userFsTryOpenFile = 0x0;
	u32 addr = 0x00100000;
	u32 lastPage = 0;
	u32 ret;
	while (addr < (maxcodesize + 0x00100000)) {
		if (fsMountArchive == 0) {
			if (READU32(addr) == 0xE5970010) {
				if ((READU32(addr + 0x4) == 0xE1CD20D8) && (READU16(addr + 0x8) == 0x0000) && (READU8(addr + 0xA) == 0x8D)) {
					fsMountArchive = findNearestSTMFD(addr);
				}
			} else if (READU32(addr) == 0xE24DD028) {
				if ((READU32(addr + 0x4) == 0xE1A04000) && (READU32(addr + 0x8) == 0xE59F60A8) && (READU32(addr + 0xC) == 0xE3A0C001)) {
					fsMountArchive = findNearestSTMFD(addr);
				}
			}
		}
		if (fsRegArchive == 0) {
			if (READU32(addr) == 0xC82044B4) {
				if (READU32(addr + 0x4) == 0xD8604659) {
					fsRegArchive = findNearestSTMFD(addr);
				}
			}
		}
		if (userFsTryOpenFile == 0) {
			if (READU32(addr + 0xC) == 0xE12FFF3C) {
				if (((READU32(addr) == 0xE1A0100D) || (READU32(addr) == 0xE28D1010)) && (READU32(addr + 4) == 0xE590C000) && ((READU32(addr + 8) == 0xE1A00004) || (READU32(addr + 8) == 0xE1A00005))) {
					userFsTryOpenFile = findNearestSTMFD(addr);
				}
			}
		}
		addr += 0x4;
	}
	return;
}

int comparestring(char *str1, char *str2, int max) {
	int i;
	for(i=0;i<max;i++) {
		if (str1[i] != str2[i]) {
			return 0;
		}
	}
	return 1;
}

void cleargarbage(char *str1, int buffsize) {
	int currchar = 0;
	int foundend = 0;
	while (currchar < buffsize) {
		if ((foundend == 0) && (str1[currchar] == '\0')) {
			foundend = 1;
		}
		if (foundend == 1) {
			memset(str1 + currchar, '\0', buffsize - (currchar+1));
			break;
		}
		currchar++;
	}
}

char currenttid[16];
char layeredfspath[100];
char codepath[100];

void getpath() {
	if (comparestring(custompath,defaultfolder, 5)) {
		memset(layeredfspath, 0, 100);
		strCat(layeredfspath, custompath);
	} else {
		xsprintf(currenttid, "%08X%08X/", plgloader->tid[1], plgloader->tid[0]);
		memset(layeredfspath, 0, 100);
		strCat(layeredfspath, defaultfolder);
		strCat(layeredfspath, currenttid);
	}
}
u32 errorbuf[2];
u64 currposs = 0;

int IFile_Read(Handle *filehandle, u32* readbytes, void* outbuffer, u64 amount) {
	int ret = FSFILE_Read(*filehandle, readbytes, (u32)currposs, outbuffer, amount);
	currposs += amount;
	return ret;
}

void applyCodeIpsPatch(Handle *file, u32 size)
{
    int ret;
    u8 buffer[5];
    u32 total;
	u8* code = (u8*)0x00100000; 

    if(R_FAILED(IFile_Read(file, &total, buffer, 5)) || total != 5 || memcmp(buffer, "PATCH", 5) != 0) goto exit;

    while(R_SUCCEEDED(IFile_Read(file, &total, buffer, 3)) && total == 3)
    {
        if(memcmp(buffer, "EOF", 3) == 0) break;

        u32 offset = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];

        if(R_FAILED(IFile_Read(file, &total, buffer, 2)) || total != 2) break;

        u32 patchSize = (buffer[0] << 8) | buffer[1];

        if(!patchSize)
        {
            if(R_FAILED(IFile_Read(file, &total, buffer, 2)) || total != 2) break;

            u32 rleSize = (buffer[0] << 8) | buffer[1];

            if(offset + rleSize > size) break;

            if(R_FAILED(IFile_Read(file, &total, buffer, 1)) || total != 1) break;

            for(u32 i = 0; i < rleSize; i++)
                code[offset + i] = buffer[0];

            continue;
        }

        if(offset + patchSize > size) break;

        if(R_FAILED(IFile_Read(file, &total, code + offset, patchSize)) || total != patchSize) break;
    }

exit:
	#ifdef DEBUG_MODE
		xsprintf(debugtempstring, "0x%08X", currposs);
		strCat(debugstring, debugtempstring);
	#endif
	currposs = 0;
    return;
}

void loadcode(u8 mode) {
	Handle fileout;
	u64 filesize;
	Handle currprocess = getCurrentProcessHandle();
	initSrv();
	int ret = InitFS();
	exitSrv();
	if (ret != 0) {
		return;
	}
	FSUSER_OpenArchive(fsUserHandle, &sdmcArchive);
	if (!mode) {
		memset(codepath, 0, 100);
		strCat(codepath, &layeredfspath[findu8character(layeredfspath, '/')]);
		strCat(codepath, "code.ips");
		FS_path myPath = FS_makePath(PATH_CHAR, codepath);
		ret = FSUSER_OpenFileDirectly(fsUserHandle, &fileout, sdmcArchive, myPath, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
		if (ret != 0) {
			FSUSER_CloseArchive(fsUserHandle, &sdmcArchive);
			svc_closeHandle(fileout);
			InitFS();
			loadcode(1);
			return;
		}
		ret = rtCheckRemoteMemoryRegionSafeForWrite(getCurrentProcessHandle(), 0x00100000, maxcodesize);
		if (ret != 0) {
			FSFILE_Close(fileout);
			FSUSER_CloseArchive(fsUserHandle, &sdmcArchive);
			InitFS();
			return;
		}
		applyCodeIpsPatch(&fileout, maxcodesize);
		FSFILE_Close(fileout);
		FSFILE_Close(fileout);
		FSUSER_CloseArchive(fsUserHandle, &sdmcArchive);
		InitFS();
		svc_flushProcessDataCache(currprocess, 0x00100000, maxcodesize);
		svc_invalidateProcessDataCache(currprocess, 0x00100000, maxcodesize);
		return;
	} else {
		memset(codepath, 0, 100);
		strCat(codepath, &layeredfspath[findu8character(layeredfspath, '/')]);
		strCat(codepath, "code.bin");
		FS_path myPath = FS_makePath(PATH_CHAR, codepath);
		ret = FSUSER_OpenFileDirectly(fsUserHandle, &fileout, sdmcArchive, myPath, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
		if (ret != 0) {
			FSUSER_CloseArchive(fsUserHandle, &sdmcArchive);
			svc_closeHandle(fileout);
			InitFS();
			return;
		}
		FSFILE_GetSize(fileout, &filesize);
		ret = rtCheckRemoteMemoryRegionSafeForWrite(getCurrentProcessHandle(), 0x00100000, (u32)filesize);
		if (ret != 0) {
			FSFILE_Close(fileout);
			FSUSER_CloseArchive(fsUserHandle, &sdmcArchive);
			InitFS();
			return;
		}
		FSFILE_Read(fileout, NULL, 0, (u32*)0x00100000, (u32)filesize);
		FSFILE_Close(fileout);
		FSUSER_CloseArchive(fsUserHandle, &sdmcArchive);
		InitFS();
		svc_flushProcessDataCache(currprocess, 0x00100000, (u32)filesize);
		svc_invalidateProcessDataCache(currprocess, 0x00100000, (u32)filesize);
		return;
	}
}
u32 getcodesize() { //May not be 100% accurate, but it's something...
	u32 ret, currPage, lastPage = 0;
	u32 addr = 0x00100000;
	while (1) {
		currPage = rtGetPageOfAddress(addr);
		if (currPage != lastPage) {
			lastPage = currPage;
			ret = rtCheckRemoteMemoryRegionSafeForWrite(getCurrentProcessHandle(), addr, 8);
			if (ret != 0) {
				return addr - 0x00100000;
			}
		}
		addr += 0x4;
	}
}

#ifdef ENABLE_LANGEMU
u32 cfgGetRegion = 0;
u32 pCfgHandle = 0;

RT_HOOK cfgReadBlockHook;
RT_HOOK cfgGetRegionHook;

typedef u32 (*cfgReadBlockTypeDef)(u8*, u32, u32);
u32 cfgReadBlockCallback(u8* buf, u32 size, u32 code) {
	u32 ret;
	nsDbgPrint("readblk: %08x, %08x, %08x\n", buf, size, code);
	ret = ((cfgReadBlockTypeDef)cfgReadBlockHook.callCode)(buf, size, code);
	if (code ==  0xA0002) {
		*buf = langCode;
	}	
	return ret;
}


u32 cfgGetRegionCallback(u32* pRegion) {
	*pRegion = regCode;
	nsDbgPrint("region patched\n");
	return 0;
}

void findCfgGetRegion() {
	cfgGetRegion = 0;
	u32 addr = 0x00100004;
	u32 lastPage = 0;
	u32 currPage, ret;
	while(1) {
		currPage = rtGetPageOfAddress(addr);
		if (currPage != lastPage) {
			lastPage = currPage;
			ret = rtCheckRemoteMemoryRegionSafeForWrite(getCurrentProcessHandle(), addr, 8);
			if (ret != 0) {
				nsDbgPrint("endaddr: %08x\n", addr);
				return;
			}
		}
		if (READU32(addr) == pCfgHandle) {
			if (*((u32*)(addr - 0x2c)) == 0xe3a00802) {
				nsDbgPrint("addr: %08x\n", addr);
				cfgGetRegion = findNearestSTMFD(addr);
				return;
			}
		}
		addr += 4;
	}
}

void findCfgReadBlock() {
	cfgReadBlock = 0;
	u32 addr = 0x00100004;
	u32 lastPage = 0;
	u32 currPage, ret;
	while(1) {
		currPage = rtGetPageOfAddress(addr);
		if (currPage != lastPage) {
			lastPage = currPage;
			ret = rtCheckRemoteMemoryRegionSafeForWrite(getCurrentProcessHandle(), addr, 8);
			if (ret != 0) {
				nsDbgPrint("endaddr: %08x\n", addr);
				return;
			}
		}
		if (READU32(addr) == 0x00010082) {
			if (*((u32*)(addr - 4)) == 0xe8bd8010) {
				nsDbgPrint("addr: %08x\n", addr);
				cfgReadBlock = findNearestSTMFD(addr);
				pCfgHandle = *(u32*)(addr + 4);
				return;
			}
		}
		addr += 4;
	}
}
#endif
int main() {
	u32 retv;
	initSharedFunc();
	plgloader = (PLGLOADER_INFO *)0x07000000;
	maxcodesize = getcodesize();
	#ifdef DEBUG_MODE
	memset(debugstring, 0, 100);
	strCat(debugstring, "OnionFS_debug");
	xsprintf(debugtempstring, " code: 0x%08X, 0x%08X", 0x100000, maxcodesize);
	strCat(debugstring, debugtempstring);
	#endif
	memset(archivelist, 0, 200);
	strCat(archivelist, "rom:"); // Since "rom:" is the default, I'll just add it here.
	getpath();
	loadcode(0);
	getvalues();
	cleargarbage(layeredfspath, 100);
	if (((fsMountArchive == 0x0) && (fsRegArchive == 0x0)) && (userFsTryOpenFile == 0x0)) {
		return 0;
	}
#ifdef ENABLE_LAYEREDFS
	strCat(layeredfspath, "romfs/");
	convertAnsiToUnicode((layeredfspath), ustrRootPath);
	rtInitHook(&regArchiveHook, (u32) fsRegArchive, (u32) fsRegArchiveCallback);
	rtEnableHook(&regArchiveHook);
	rtInitHook(&userFsTryOpenFileHook, userFsTryOpenFile, (u32)userFsTryOpenFileCallback);
	rtEnableHook(&userFsTryOpenFileHook);
#endif
#ifdef ENABLE_LANGEMU
	findCfgReadBlock();
	nsDbgPrint("cfgreadblock: %08x\n", cfgReadBlock);
	if (cfgReadBlock == 0) {
		return 0;
	}
	rtInitHook(&cfgReadBlockHook, (u32) cfgReadBlock, (u32) cfgReadBlockCallback);
	rtEnableHook(&cfgReadBlockHook);
	findCfgGetRegion();
	nsDbgPrint("cfggetregion: %08x\n", cfgGetRegion);
	if (cfgGetRegion == 0) {
		return 0;
	}

	rtInitHook(&cfgGetRegionHook, (u32) cfgGetRegion, (u32) cfgGetRegionCallback);
	rtEnableHook(&cfgGetRegionHook);
#endif
	return 0;
}

