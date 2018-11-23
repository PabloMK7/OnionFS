#pragma once
#include "CTRPluginFramework.hpp"
#include "rt.h"
#include "csvc.h"

#define NUMBER_FILE_OP 9

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

namespace CTRPluginFramework {
	using StringVector = std::vector<std::string>;
	u32 fsRegArchiveCallback(u8* path, u32* arch, u32 isAddOnContent, u32 isAlias);
	int fsFormatSaveData(int *a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, char a11);
	u32* findNearestSTMFDptr(u32* newaddr);
	extern RT_HOOK fileOpHooks[NUMBER_FILE_OP];
	extern u32 fileOperationFuncs[NUMBER_FILE_OP];
	extern RT_HOOK regArchiveHook;
	extern u32 fsMountArchive;
	extern LightLock regLock;
	extern bool canSaveRedirect;
	extern char16_t romfsPath[];
	extern char16_t savePath[];
	int strlen16(u16* str);
	bool initOnionFSHooks(u32 textSize);
}