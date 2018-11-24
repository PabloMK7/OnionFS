#include "save.hpp"
#include "main.hpp"
#include "ctrulib/util/utf.h"
#include "Unicode.h"

namespace CTRPluginFramework {
	archData OnionSave::save = {0};
	saveData OnionSave::settings = { 0 };
	char OnionSave::savePath[100] = {0};
	u16 OnionSave::romPath[50] = { 0 };
	u16 OnionSave::dataPath[50] = { 0 };
	File* OnionSave::debugFile = nullptr;
	bool OnionSave::needsReboot = false;

	LightLock debugLock = { 0 };
	Result OnionSave::loadDeafults() {
		DEBUG("Config file not found, loading defaults.\n")
		settings.header.magic = SAVE_MAGIC;
		settings.header.version = SAVE_REVISION;
		settings.header.numEntries = 0;
		settings.header.lastLoadedPack = 0;
		OnionSave::saveSettings();
		return addModEntry(g_ProcessTID + 8, ARCH_ROMFS);
	}

	bool checkFolderExists(u16* name) {
		FS_Archive sdmcArchive;
		Handle dirHandle;
		Result ret = 0;
		ret = FSUSER_OpenArchive(&sdmcArchive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
		if (ret) return false;
		ret = FSUSER_OpenDirectory(&dirHandle, sdmcArchive, fsMakePath(PATH_UTF16, name));
		FSDIR_Close(dirHandle);
		FSUSER_CloseArchive(sdmcArchive);
		if (ret) {
			return false;
		}
		return true;
	}

	bool createDirectory(char* name) {
		return Directory::Create(name) == 0;
	}

	Result OnionSave::saveSettings() {
		DEBUG("Trying to save settings.\n");
		FS_Archive sdmcArchive;
		Handle fileHandle;
		if (!savePath[0]) {
			sprintf(savePath, "%s%s%s", TOP_DIR"/", g_ProcessTID, ".onionfs");
		}
		if (!checkFolderExists((u16*)u"/OnionFS")) createDirectory((char*)TOP_DIR);
		File::Remove(savePath);
		File savFile(savePath, File::RWC);
		if (!savFile.IsOpen()) {
			DEBUG("Failed to save settings.\n");
			return -1;
		}
		DEBUG("Settings saved.\n");
		u32 writeSize = sizeof(saveHdr) + settings.header.numEntries * sizeof(saveEntry);
		savFile.Write(&settings, writeSize);
		return 0;
	}

	void strcatu16(u16* dest, char* s1, char* s2, char* s3) {
		while (*s1) *dest++ = *s1++;
		while (*s2) *dest++ = *s2++;
		while (*s3) *dest++ = *s3++;
		*dest = '\0';
	}

	int strcmpu8u16(char* ptr1, u16* ptr2) {
		int i = 0;
		u16 char1;
		u16 char2;
		do {
			char1 = (u16)(ptr1[i]);
			char2 = ptr2[i++];
		} while (char2 == char1 && ptr1[i] && ptr2[i] != u':');
		return char1 - char2;
	}

	int strcmpdot(char* ptr1, char* ptr2) {
		int i = 0;
		char char1;
		char char2;
		do {
			char1 = ptr1[i];
			char2 = ptr2[i++];
		} while (char2 == char1 && ptr1[i] && ptr2[i] != ':');
		return char1 - char2;
	}

	void strcpydot(char* dst, char* src, int n) {
		while (*src != ':' && n) {
			*dst++ = *src++;
			n--;
		}
		*dst = '\0';
	}
#define PA_PTR(addr)            
	void OnionSave::initDebug() {
		if ((((*(vu32 *)((u32)(0x10146000) | 1 << 31))) ^ 0xFFF) & Key::DPadUp) {
			ENABLE_DEBUG = true;
		}
		if (ENABLE_DEBUG) {
			LightLock_Init(&debugLock);
			std::string debugfile = std::string(TOP_DIR"/") + std::string(g_ProcessTID) + std::string("debug.txt");
			File::Remove(debugfile);
			debugFile = new File(debugfile, File::RWC);
			if (!debugFile->IsOpen()) debugFile = nullptr;
			debugAppend("OnionFS " STRING_VERSION " debug file, hello! :3\n----------------------------------------\n\n");
		}
	}
	
	void OnionSave::debugAppend(std::string data) {
		if (!debugFile || !ENABLE_DEBUG) return;
		LightLock_Lock(&debugLock);
		debugFile->Write(data.c_str(), data.size());
		debugFile->Flush();
		LightLock_Unlock(&debugLock);
	}

	int OnionSave::existArchiveu16(u16 * arch)
	{
		for (int i = 0; i < save.numEntries; i++) {
			if (strcmpu8u16(save.entries[i].archName, arch) == 0) {
				if (save.entries[i].finished) return i;
				else return -1;
			}
		}
		return -1;
	}

	int OnionSave::existArchiveHnd(u64 hnd)
	{
		for (int i = 0; i < save.numEntries; i++) {
			if (save.entries[i].archHandle == hnd) {
				return i;
			}
		}
		return -1;
	}

	int OnionSave::existArchiveu8(u8 * arch)
	{
		for (int i = 0; i < save.numEntries; i++) {
			if (strcmpdot(save.entries[i].archName, (char*)arch) == 0) {
				if (save.entries[i].finished) return i;
				else return -1;
			}
		}
		return -1;
	}

	void OnionSave::addArchiveHnd(u64 handle, u32 archid) {
		if (archid != 4) {
			DEBUG("Rejected %016llX, archiveID: 0x%08X\n", handle, archid);
			return;
		}
		if (existArchiveHnd(handle) != -1) return;
		if (save.numEntries >= MAX_SAVE_ENTRIES) {
			DEBUG("Archive buffer is full.\n");
			return;
		}
		DEBUG("Adding save archive: %016lX\n", handle);
		save.entries[save.numEntries].type = ARCH_SAVE;
		save.entries[save.numEntries].archHandle = handle;
		save.entries[save.numEntries++].finished = 0;
	}

	void OnionSave::addArchive(u8* arch, u64 handle)
	{
		char newbuf[10];
		if (ENABLE_DEBUG) strcpydot(newbuf, (char*)arch, 10);
		if (arch[0] == '$') {
			DEBUG("Rejected \"%s\"\n", newbuf, (u32)(handle >> 32), (u32)handle);
			return; //Archives starting with $ are used by the game internally, usually mii data.
		}
		if (existArchiveu8(arch) == -1) {
			int hndpos = existArchiveHnd(handle);
			if (hndpos == -1) {
				if ((u32)(handle) > 0x100000 && (u32)(handle) < 0x20000000) { // rom archives doesn't have a fsarchive handle, it has a pointer in its place.
					DEBUG("Added \"%s\" as romfs archive!\n", newbuf);
					if (save.numEntries >= MAX_SAVE_ENTRIES) {
						DEBUG("Archive buffer is full.\n");
						return;
					}
					save.entries[save.numEntries].type = ARCH_ROMFS;
					save.entries[save.numEntries].archHandle = ~0x0;
					strcpydot((save.entries[save.numEntries].archName), (char*)arch, sizeof(save.entries[0].archName));
					save.entries[save.numEntries++].finished = 1;
					return;
				}
				else {
					DEBUG("Rejected \"%s\" with handle: 0x%08X%08X\n", newbuf, (u32)(handle >> 32), (u32)handle);
					return;
				}
			}
			DEBUG("Added \"%s\" with handle: 0x%08X%08X\n", newbuf, (u32)(handle >> 32), (u32)handle);
			if (save.entries[hndpos].finished == 1) return;
			strcpydot((save.entries[hndpos].archName), (char*)arch, sizeof(save.entries[0].archName));
			save.entries[hndpos].finished = 1;
		}
	}

	u32 OnionSave::getEntries() {
		return settings.header.numEntries;
	}

	void OnionSave::getEntryData(u32 index, char ** name, u8 * flags)
	{
		if (index >= settings.header.numEntries) return;
		*name = settings.entries[index].name;
		*flags = settings.entries[index].flags;
	}

	void OnionSave::setEntryFlags(u32 index, u8 flags)
	{
		if (index >= settings.header.numEntries) return;
		DEBUG("Changing %s flags to 0x%02X\n", settings.entries[index].name, flags);
		settings.entries[index].flags = flags;
	}

	bool OnionSave::removeModEntry(u32 index) {
		if (index >= settings.header.numEntries || settings.header.numEntries == 1) return false;
		DEBUG("Removing entry %s\n", settings.entries[index].name);
		for (int i = index; i < settings.header.numEntries - 1; i++) {
			memcpy(&(settings.entries[i]), &(settings.entries[i + 1]), sizeof(saveEntry));
		}
		settings.header.numEntries--;
		if (settings.header.lastLoadedPack > index) settings.header.lastLoadedPack--;
		if (settings.header.lastLoadedPack >= settings.header.numEntries) settings.header.lastLoadedPack = settings.header.numEntries - 1;
		return true;
	}

	bool OnionSave::checkEntryExists(const char* name) {
		for (int i = 0; i < settings.header.numEntries; i++)
			if (strcmp(settings.entries[i].name, name) == 0) return true;
		return false;
	}
	bool OnionSave::addModEntry(const char* name, u8 flags) {
		if (!name) return false;
		if (settings.header.numEntries >= MAX_SAVE_ENTRIES) return false;
		flags &= 0x7;
		if (strlen(name) + 1 > sizeof(settings.entries[0].name)) return false;
		if (checkEntryExists(name)) return false;
		DEBUG("Adding mod entry %s with flags 0x%02X\n", name, (u32)flags);
		strcpy(settings.entries[settings.header.numEntries].name, name);
		settings.entries[settings.header.numEntries].flags = flags;
		char* dirName = static_cast<char *>(::operator new(0x200));
		if (!dirName) return false;
		//
		strcpy(dirName, (char*)TOP_DIR"/");
		strcat(dirName, name);
		createDirectory(dirName);
		//
		strcpy(dirName, (char*)TOP_DIR"/");
		strcat(dirName, name);
		strcat(dirName, (char*)"/romfs");
		createDirectory(dirName);
		//
		strcpy(dirName, (char*)TOP_DIR"/");
		strcat(dirName, name);
		strcat(dirName, (char*)"/save");
		createDirectory(dirName);		
		delete[] dirName;
		settings.header.numEntries++;
		OnionSave::saveSettings();
		return true;
	}

	bool showMsgKbd(std::string text, DialogType digtype) {
		Keyboard kbd(text);
		StringVector opts;
		switch (digtype)
		{
		case CTRPluginFramework::DialogType::DialogOk:
			opts = { "Ok" };
			break;
		case CTRPluginFramework::DialogType::DialogOkCancel:
			opts = { "Ok", "Cancel" };
			break;
		case CTRPluginFramework::DialogType::DialogYesNo:
			opts = { "Yes", "No" };
			break;
		default:
			break;
		}
		kbd.Populate(opts);
		return kbd.Open() == 0;
	}

	std::string OnionSave::generateByPage(u32 &page, u32 &maxPages, bool mode) //max 12 lines
	{
		maxPages = ((settings.header.numEntries -1) / 9) + 1;
		if (page >= maxPages) page = maxPages - 1;
		if (page < 0) page = 0;
		std::string out;
		if (mode) out = "Entries: (Page " + std::to_string(page+1) + " / " + std::to_string(maxPages) + " )  " << Color::LimeGreen << "Entry currently in use." << Color::White;
		else out = "Choose Entry: (Press " FONT_B " to exit.)";
		out.append("\n--------------------------------------\n");
		int i = page * 9;
		int j = settings.header.numEntries - i;
		if (j > 9) j = 9;
		int remain = 9 - j;
		for (int k = 0; k < j; i++, k++) {
			Color numb = (i == settings.header.lastLoadedPack) ? Color::ForestGreen : Color::Gray;
			Color norm = (i == settings.header.lastLoadedPack) ? Color::LimeGreen : Color::White;
			out.append(" " << numb << std::to_string(i+1) << ": " << norm << std::string(settings.entries[i].name) << "\n");
		}
		for (int k = 0; k < remain; k++) out.append("\n");
		out.append("" << Color::White << "--------------------------------------");
		return out;
	}

	bool OnionSave::showRebootMsg() {
		if (needsReboot) return true;
		return needsReboot = showMsgKbd("Doing this action will reboot the console after\nexiting the OnionFS config. Are you sure?", DialogType::DialogOkCancel);
	}

	bool entryNameCompareCallback(const void* input, std::string &error) {
		std::string in = *reinterpret_cast<const std::string *>(input);
		if (in.size() >= sizeof(OnionSave::settings.entries[0].name)) {
			error = "Name can only be " + std::to_string(sizeof(OnionSave::settings.entries[0].name) - 1) +" characters long.";
			return false;
		}
		for (std::string::const_iterator s = in.begin(); s != in.end(); ++s) {
			if (!(isalnum(*s) || *s == ' ' || *s == '_' || *s == '-' || *s == '#')) {
				error = "Only \"a-z\", \"A-Z\", \"0-9\", \"space\", \"_\", \"#\" and \"-\" are allowed.";
				return false;
			}
		}
		return true;
	}

	void OnionSave::addModEntryUI() {
		Keyboard kbd("Input the new entry name.\n\nPress Enter to confirm.\nPress " FONT_B " to cancel.");
		kbd.SetCompareCallback(entryNameCompareCallback);
		std::string name;
		bool retry = true;
		while (retry) {
			int ret = kbd.Open(name);
			if (ret != 0) return;
			if (checkEntryExists(name.c_str())) {
				showMsgKbd("An entry with the same name already exists for\nthis game, please chose a different name.", DialogType::DialogOk);
			}
			else {
				bool doIt = true;
				if (Directory::IsExists(TOP_DIR "/" + name)) {
					doIt = showMsgKbd("An entry with the same name exists in the SD.\nThis could be an entry for another game or a\nremoved entry.\n\nWould you like to use it anyways?\n\n" << Color::Yellow << "WARNING: " << Color::White << "If it is for another game, make sure both game's saves are compatible with each\nother, otherwise the redirected save may get\ncorrupted.", DialogType::DialogYesNo);
				}
				if (doIt) {
					bool ret2 = addModEntry(name.c_str(), 0);
					if (ret2) {
						showMsgKbd("The following entry:\n--------------------------------------\n" << Color::Gray << std::to_string(OnionSave::settings.header.numEntries) << ": " << Color::White << name << "\n--------------------------------------\nhas been added.\n\nYou can now config its settings from\n\"Manage Entries\".", DialogType::DialogOk);
					}
					else {
						showMsgKbd("Cannot add more entries.\nMaximum number of entries: " TOSTRING(MAX_SAVE_ENTRIES), DialogType::DialogOk);
					}
					retry = false;
				}
			}
		}
	}

	void OnionSave::editEntryById(u32 val)
	{
		std::string enSlid = Color::LimeGreen << "\u2282\u25CF";
		std::string disSlid = Color::Red << "\u25CF\u2283";
		std::string title;
		bool loop = true;
		Keyboard kbd("dummy");
		StringVector opts;
		kbd.CanAbort(false);
		while (loop) {
			title = "Editing single entry:\n\n";
			title.append("Index: " << Color::Gray << std::to_string(val + 1) << Color::White << "\n");
			title.append("Name: " << Color::Gray << std::string(settings.entries[val].name) << Color::White << "\n\n");
			title.append("1: ROMFS Redirection: " << ((settings.entries[val].flags & ARCH_ROMFS) ? (Color::LimeGreen << "Enabled") : (Color::Red << "Disabled")) << Color::White << "\n");
			title.append("2: SAVE Redirection: " << ((settings.entries[val].flags & ARCH_SAVE) ? (Color::LimeGreen << "Enabled") : (Color::Red << "Disabled")) << Color::White << "\n");
			opts.clear();
			opts.push_back(std::string("1: ") << ((settings.entries[val].flags & ARCH_ROMFS) ? enSlid : disSlid)); 
			opts.push_back(std::string("2: ") << ((settings.entries[val].flags & ARCH_SAVE) ? enSlid : disSlid));
			opts.push_back("Remove");
			if (settings.header.lastLoadedPack != val) {
				opts.push_back("Use Entry");
			}
			else {
				title.append("\n" << Color::LimeGreen << "This entry is currently in use." << Color::White << "\n");
			}
			kbd.GetMessage() = title;
			opts.push_back("Back");
			kbd.Populate(opts);
			int chose;
			switch (chose = kbd.Open())
			{
			case 0:
			{
				bool ret = true;
				if (settings.header.lastLoadedPack == val) {
					ret = showRebootMsg();
				}
				if (ret) {
					settings.entries[val].flags ^= ARCH_ROMFS;
				}
				break;
			}
			case 1:
			{
				bool ret = true;
				if (settings.header.lastLoadedPack == val) {
					ret = showRebootMsg();
				}
				if (ret) {
					settings.entries[val].flags ^= ARCH_SAVE;
				}
				break;
			}
			case 2:
			{
				if (!showMsgKbd("Are you sure you want to delete this entry?\nThe actual files won't be removed.", DialogType::DialogYesNo)) break;
				bool ret = true;
				if (settings.header.lastLoadedPack == val) {
					ret = showRebootMsg();
				}
				if (ret) {
					bool ret2 = removeModEntry(val);
					if (!ret2) {
						showMsgKbd("Cannot remove this entry if it is the only one.", DialogType::DialogOk);
					}
					else {
						loop = false;
					}
				}
				break;
			}
			default:
			{
				if (settings.header.lastLoadedPack == val) chose++;
				if (chose == 3) {
					if (showRebootMsg()) {
						settings.header.lastLoadedPack = val;
						loop = false;
					}
					break;
				}
				else {
					loop = false;
					break;
				}
			}
			}
		}
	}

	void OnionSave::setupPackPaths() {
		strcatu16(OnionSave::romPath, (char*)"ram:" TOP_DIR"/", settings.entries[settings.header.lastLoadedPack].name, (char*)"/romfs/");
		strcatu16(OnionSave::dataPath, (char*)"ram:" TOP_DIR"/", settings.entries[settings.header.lastLoadedPack].name, (char*)"/save/");
		if (!Directory::IsExists(TOP_DIR "/" << std::string(settings.entries[settings.header.lastLoadedPack].name))) Directory::Create(TOP_DIR "/" << std::string(settings.entries[settings.header.lastLoadedPack].name));
		if (!checkFolderExists(OnionSave::romPath + 4)) Directory::Create(TOP_DIR "/" << std::string(settings.entries[settings.header.lastLoadedPack].name) << "/romfs");
		if (!checkFolderExists(OnionSave::dataPath + 4)) Directory::Create(TOP_DIR "/" << std::string(settings.entries[settings.header.lastLoadedPack].name) << "/save");
	}
	
	bool OnionSave::getArchive(u16 * arch, u8* mode, bool isReadOnly)
	{
		int entry = existArchiveu16(arch);
		if (entry == -1) return false;
		u8 flag = save.entries[entry].type;
		*mode = flag;
		u8 romode = !((flag & ARCH_ROMFS) && isReadOnly); //Only allow functions without the readonly flag
		return romode && flag & settings.entries[settings.header.lastLoadedPack].flags;
	}

	bool OnionSave::loadSettings() {
		initDebug();
		DEBUG("Trying to load settings.\n");
		Result ret = 0;
		if (!savePath[0]) {
			sprintf(savePath, "%s%s%s", TOP_DIR"/", g_ProcessTID, ".onionfs");
		}
		File savFile(savePath, File::READ);
		if (!savFile.IsOpen()) {
			ret = loadDeafults();
			if (ret) {
				return false;
			}
			return true;
		}
		savFile.Read(&settings, sizeof(saveData));
		savFile.Close();
		if (settings.header.magic == SAVE_MAGIC && settings.header.version == SAVE_REVISION && settings.header.numEntries > 0) {
			if (settings.header.numEntries > MAX_SAVE_ENTRIES) settings.header.numEntries = MAX_SAVE_ENTRIES;
			if (settings.header.lastLoadedPack > MAX_SAVE_ENTRIES - 1) settings.header.lastLoadedPack = 0;
			DEBUG("Settings loaded.\n")
			return true;
		}
		else {
			ret = loadDeafults();
			if (ret) {
				return false;
			}
			return true;
		}
	}
}