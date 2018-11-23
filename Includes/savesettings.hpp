#include "CTRPluginFramework.hpp"
#include "string.h"

#define SAVE_VER 4
#define FILE_MAGIC "C7SV"
#define FILE_PATH "/CTGP-7/resources/CTGP-7.sav"

namespace CTRPluginFramework {
	class SaveHandler {
		public:
			static bool backcamenabled;
			static bool warnitemenabled;
			static bool firstOpening;
			static bool readyToFool;
			static bool LoadSettings();
			static bool SaveSettings();
			static void DefaultSettings();
	};
	struct SaveHandler_hdr {
		u8 magic[4];
		u8 version;
		SaveHandler_hdr() {
			memcpy(magic, FILE_MAGIC, 4);
			version = SAVE_VER;
		}
	} PACKED;
}