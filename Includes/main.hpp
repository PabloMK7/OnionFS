#pragma once
#include "CTRPluginFramework.hpp"
#include "rt.h"
#include "csvc.h"

#define TOP_DIR "/OnionFS"
#define NUMBER_FILE_OP 9
#define MAJOR_VERSION       2
#define MINOR_VERSION       0
#define REVISION_VERSION    0
#define STRINGIFY(x)        #x
#define TOSTRING(x)         STRINGIFY(x)
#define STRING_VERSION      "[" TOSTRING(MAJOR_VERSION) "." TOSTRING(MINOR_VERSION) "." TOSTRING(REVISION_VERSION) "]"

#define WRITEREMOTE32(addr, val) (*(u32 *)(PA_FROM_VA_PTR(addr)) = (val))

enum fileSystemBits
{
	OPEN_FILE_OP,
	OPEN_DIRECTORY_OP,
	DELETE_FILE_OP,
	RENAME_FILE_OP,
	DELETE_DIRECTORY_OP,
	DELETE_DIRECTORY_RECURSIVE_OP,
	CREATE_FILE_OP,
	CREATE_DIRECTORY_OP,
	RENAME_DIRECTORY_OP
};

typedef u32(*fsRegArchiveTypeDef)(u8*, u32*, u32, u32);
typedef u32(*userFsTryOpenFileTypeDef)(u32, u16*, u32);
typedef u32(*fsMountArchiveTypeDef)(u32*, u32);

typedef u32(*fsu32u16u32)(u32, u16*, u32);
typedef u32(*fsu16)(u16*);
typedef u32(*fsu16u16)(u16*, u16*);
typedef u32(*fsu16u64)(u16*, u64);
typedef u32(*fsu32u16)(u32, u16*);

extern "C" void customBreak(u32 a1, u32 a2, u32 a3);

typedef struct miniHeap_s {
	u8 data[0x10][0x200];
	u8 entries[0x10];
}miniHeap;

namespace CTRPluginFramework {
	using StringVector = std::vector<std::string>;
	extern bool ENABLE_DEBUG;
	u32 fsRegArchiveCallback(u8* path, u32* arch, u32 isAddOnContent, u32 isAlias);
	int  fsOpenArchiveFunc(u32* fsHandle, u64* out, u32 archiveID, u32 pathType, u32 pathData, u32 pathsize);
	int fsFormatSaveData(int *a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, char a11);
	extern miniHeap strHeap;
	extern char g_ProcessTID[17];
	extern RT_HOOK fileOpHooks[NUMBER_FILE_OP];
	extern u32 fileOperationFuncs[NUMBER_FILE_OP];
	extern RT_HOOK regArchiveHook;
	extern u32 fsMountArchive;
	extern LightLock regLock;
	extern LightLock openLock;
	extern bool canSaveRedirect;
	int strlen16(u16* str);
	int fsSetThisSaveDataSecureValue(u32 a1, u64 a2);
	int Obsoleted_5_0_fsSetSaveDataSecureValue(u64 a1, u32 a2, u32 a3, u8 a4);
	int fsSetSaveDataSecureValue(u64 a1, u32 a2, u64 a3, u8 a4);
}
#define	DEBUG(str, ...) {if (ENABLE_DEBUG) { u8* cpybuf = new u8[0x300]; sprintf((char*)cpybuf, str, ##__VA_ARGS__); OnionSave::debugAppend((char*)cpybuf); delete[] cpybuf;}}
#define DEBUGU16(str) {if (ENABLE_DEBUG) {std::string out = "\""; Process::ReadString((u32)str, out, 0x200, StringFormat::Utf16); out += "\""; OnionSave::debugAppend(out); }}
