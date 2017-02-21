
#include "global.h"
#include "string.h"
#include "autogen.h"

/* uncomment this to log filename into debugger output */
// #define LOG_FILES 

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
#define custompath "yourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyourpathyou"
#define defaultfolder "ram:/OnionFS/"
u32 fsMountArchive;
u32 fsRegArchive;
u32 userFsTryOpenFile;
u32 cfgReadBlock = 0x0;

Handle fsUserHandle;
FS_archive sdmcArchive = {0x9, (FS_path){PATH_EMPTY, 1, (u8*)""}};

typedef u32 (*fsRegArchiveTypeDef)(u8*, u32, u32, u32) ;
typedef u32 (*userFsTryOpenFileTypeDef)(u32, u16*, u32);
typedef u32 (*fsMountArchiveTypeDef)(u32*, u32);


RT_HOOK	regArchiveHook;
RT_HOOK userFsTryOpenFileHook;

u32 fsRegArchiveCallback(u8* path, u32 ptr, u32 isAddOnContent, u32 isAlias) {
	u32 ret, ret2;
	static u32 isFisrt = 1;
	u32 sdmcArchive = 0;

	nsDbgPrint("regArchive: %s, %08x, %08x, %08x\n", path, ptr, isAddOnContent, isAlias);
	ret = ((fsRegArchiveTypeDef)regArchiveHook.callCode)(path, ptr, isAddOnContent, isAlias);
	if (isFisrt) {
		isFisrt = 0;
		
		((fsMountArchiveTypeDef)fsMountArchive)(&sdmcArchive, 9);
		nsDbgPrint("sdmcArchive: %08x\n", sdmcArchive);
		
		if (sdmcArchive) {
			ret2 = ((fsRegArchiveTypeDef)regArchiveHook.callCode)("ram:", sdmcArchive, 0, 0);
			nsDbgPrint("regArchive ret: %08x\n", ret2);
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

u16 testFile []= {'r','a','m',':','/','1',0,0};
u16 ustrRom [] = {'r','o','m',':','/'};
u16 ustrRootPath[200];
char tmpstr[200];

int findcharacter(u16* in, u16 character) {
	u16 i = 0;
	while (*(in + i)) {
		if (*(in + i) == character)
			return i;
		i++;
	}
	return 0; //error
}

u32 userFsTryOpenFileCallback(u32 a1, u16 * fileName, u32 mode) {
	u16 buf[300];
	u32 ret;

#ifdef LOG_FILES
	convertUnicodeToAnsi(fileName, (u8*) buf);
	nsDbgPrint("path: %s\n", buf);
#endif

//	if (memcmp(ustrRom, fileName, sizeof(ustrRom)) == 0) {
		// accessing rom:/ file
		buf[0] = 0;
		ustrCat(buf, ustrRootPath);
		ustrCat(buf, &fileName[findcharacter(fileName, (u16)'/') + 1]);
		ret = ((userFsTryOpenFileTypeDef)userFsTryOpenFileHook.callCode)(a1, buf, 1);
#ifdef LOG_FILES
		nsDbgPrint("ret: %08x\n", ret);
#endif
		if (ret == 0) {
			return ret;
		}
//	}
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
	u32 addr = 0x00100004;
	u32 lastPage = 0;
	u32 currPage, ret;
	while (1) {
		currPage = rtGetPageOfAddress(addr);
		if (currPage != lastPage) {
			lastPage = currPage;
			ret = rtCheckRemoteMemoryRegionSafeForWrite(getCurrentProcessHandle(), addr, 8);
			if (ret != 0) {
				return;
			}
		}
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
void concatstring(char *str1, char *str2) {
	int i;
	int lastchar1 = 0;
	int lastchar2 = 0;
	while (str1[lastchar1] != '\0') lastchar1++;
	while (str2[lastchar2] != '\0') lastchar2++;
	for (i=0;i<(lastchar1+lastchar2);i++) {
		str1[lastchar1+i] = str2[i];
	}	
}

void cleargarbage(char *str1) {
	int currchar = 0;
	int foundend = 0;
	int buffsize = sizeof(str1);
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

void getpath() {
	if (comparestring(custompath,defaultfolder, 5)) {
		memset(layeredfspath, 0, 100);
		concatstring(layeredfspath, custompath);
	} else {
		PLGLOADER_INFO  *plgloader;
		plgloader = (PLGLOADER_INFO *)0x07000000;
		xsprintf(currenttid, "%08X%08X/", plgloader->tid[1], plgloader->tid[0]);
		memset(layeredfspath, 0, 100);
		concatstring(layeredfspath, defaultfolder);
		concatstring(layeredfspath, currenttid);
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
	getpath();
	getvalues();
	cleargarbage(layeredfspath);
	if (((fsMountArchive == 0x0) && (fsRegArchive == 0x0)) && (userFsTryOpenFile == 0x0)) {
		return 0;
	}
#ifdef ENABLE_LAYEREDFS
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

