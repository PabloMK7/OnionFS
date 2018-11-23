#include "CTRPluginFramework.hpp"
#include "Lang.hpp"
#include <bitset>
#define globalCTInfo_size (sizeof(globalCTInfo)/sizeof(CTInfo))
using StringVector = std::vector<std::string>;

namespace CTRPluginFramework
{
    struct CTInfo {
        u8 textID;
        std::u16string szsFilename;
        std::u16string sndFilename;
        std::string name;
    };

    extern CTInfo globalCTInfo[8][4];

    class CourseManager {
        private:
            static std::string generateTitle(int val, StringVector &cupNames);
            static void KbdByCup(int cup, StringVector &cupNames);
        public:
            static void applyNamesBySave();
			static bool letFileLoad(u16 * filename);
            static std::string getCupName(int cup);
            static std::bitset<globalCTInfo_size> trackSetMap;
            static std::bitset<globalCTInfo_size> trackSetBackup;
            static bool getTrackEnabled(int cup, int slot);
            static void setTrackEnabled(int cup, int slot, bool state);
            static void entryCallback(MenuEntry* entry);
            static CTInfo* getEntryByTextID(u32 value);
			static bool startCheckingTracks;
    };
}
