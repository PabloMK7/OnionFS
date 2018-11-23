#pragma once
#include "CTRPluginFramework.hpp"

#define SAVE_MAGIC 0x53464E4F
#define SAVE_REVISION 0
#define MAX_SAVE_ENTRIES 50

namespace CTRPluginFramework {

	typedef struct archEntry_s {
		char archName[0x10];
		u64 archHandle;
		u8 type;
		u8 finished;
	} archEntry;
	typedef struct archData_s {
		u32 numEntries;
		archEntry entries[MAX_SAVE_ENTRIES];
	} archData;
	typedef struct saveEntry_s {
		char name[0x1F];
		u8 flags;
	} saveEntry;
	typedef struct saveHdr_s {
		u32 magic;
		u32 version;
		u32 lastLoadedPack;
		u32 numEntries;
	} saveHdr;
	typedef struct saveData_s {
		saveHdr header;
		saveEntry entries[MAX_SAVE_ENTRIES];
	} saveData;

	enum ArchTypes
	{
		ARCH_ROMFS = 1,
		ARCH_SAVE = 2,
	};
	class OnionSave
	{
	public:
		static archData save;
		static saveData settings;
		static bool loadSettings();
		static Result saveSettings();
		static void addArchive(u8* arch, u64 handle);
		static bool getArchive(u16* arch, u8* mode, bool isReadOnly);
		static void initDebug();
		static void debugAppend(std::string);
		static int existArchiveu16(u16* arch);
		static int existArchiveHnd(u64 hnd);
		static void addArchiveHnd(u64 handle, u32 archid);
		static int existArchiveu8(u8* arch);
		static void setupPackPaths();
		static bool removeModEntry(u32 index);
		static u32 getEntries();
		static void getEntryData(u32 index, char** name, u8* flags);
		static void setEntryFlags(u32 index, u8 flags);
		static bool checkEntryExists(const char * name);
		static bool addModEntry(const char* name, u8 flags);
		static u16 romPath[50];
		static u16 dataPath[50];
		static std::string generateByPage(u32 &page, u32 &maxPages, bool mode);
		static bool showRebootMsg();
		static void addModEntryUI();
		static void editEntryById(u32 val);
		static bool needsReboot;
		static File* debugFile;
	private:
		static char savePath[100];
		static Result loadDeafults();
	};
	bool showMsgKbd(std::string text, DialogType digtype);
}