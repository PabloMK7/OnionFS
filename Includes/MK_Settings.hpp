
#include "CTRPluginFramework.hpp"

#define ONLINESETVER 2

namespace CTRPluginFramework {
    typedef struct _Coord3D {
        float x;
        float y;
        float z;
    } Coord3D;

    typedef struct _STGIEntry { // Second entry: custom settings
        u8 NrLaps; //bit0: force minimap
        u8 PolePosition;
        u8 Unknown1;
        u8 Unknown2;
        u32 FlareColor;
        u32 FlareAlpha;
    } STGIEntry;

    enum KartUnlockParts
    {
        BODY_STD = 0xFFFFFFFF,
        BODY_RALLY = 0xFFFFFFFF,
        BODY_RBN = 0xFFFFFFFF,
        BODY_EGG = 0x0,
        BODY_DSH = 0x4,
        BODY_CUC = 0x3,
        BODY_KPC = 0x07,
        BODY_BOAT = 0x01,
        BODY_HNY = 0x06,
        BODY_SABO = 0x0A,
        BODY_GNG = 0x05,
        BODY_PIPE = 0x08,
        BODY_TRN = 0x0B,
        BODY_CLD = 0x02,
        BODY_RACE = 0x09,
        BODY_JET = 0x0C,
        BODY_GOLD = 0x0D
    };

    enum DriverUnlock
    {
        BOWSER = 0xFFFFFFFF,
        DAISY = 0x0,
        DONKEY = 0xFFFFFFFF,
        HONEYQUEEN = 0x05,
        KOOPATROOPA = 0xFFFFFFFF,
        LAKITU = 0x07,
        LUIGI = 0xFFFFFFFF,
        MARIO = 0xFFFFFFFF,
        METAL = 0x03,
        MII = 0x08,
        PEACH = 0xFFFFFFFF,
        ROSALINA = 0x02,
        SHYGUY = 0x04,
        TOAD = 0xFFFFFFFF,
        WARIO = 0x01,
        WIGGLER = 0x06,
        YOSHI = 0xFFFFFFFF
    };

    enum GameState
    {
        None = 0,
        Enter = 1,
        Exit = 2,
    };

    struct OnlineSettings
    {
        u8  ver{0};
        u16 speedandcount{0};
        u32 enabledTracks{0};
        u8  checksum{0};
    } PACKED;

    enum GameRegion {
        JAPAN = 3,
        EUROPE = 1,
        AMERICA = 2
    };

    enum GameRevision {
        REV0_11 = 1,
        REV1 = 2
    };

    struct LaunchSettings {
        u32 region;
        u32 revision;
    };

    class MarioKartSettings {
        private:
            static bool calculateRaceCond();
            static char onlineCode[33];
            static int getCodeByChar(char c, u8 pos);
            static u64 decodeFromStr(std::u16string &s);
            static char getCharByCode(u8 num, u8 pos);
            static u32 oldStrptr;

        public:
            static Coord3D* playerCoord;
            //Coord3D* blueShellCoord;
            static u8* _currgamemode;
            static u32 region; // 1 EUR, 2 USA, 3 JPN
            static u32 revision; // 1 rev0 1.1, 2 rev1
            static u32 lastLoadedMenu; // 0x1A Main online; 0x27 community list; 0x31 confirm community; 0x2B - 0x2C enter code; 0x1c - worldwide character; 0x20 vr screen
            static u32 origChangeMapInst;
            //
            static u32 currNumberTracks;
            static u32* currNumberTracksPtr;
            static u32* currCommNumberTracksPtr;
            static u32* changeMapCallPtr;
            //
            static float* localMapPtr;
            //
            static bool isRaceGoing;
            //
            static bool getBlueShellCoordPointer();
            static bool getPlayerCoordPointer();

            static void getLaunchInfo();
            static bool isCompatible(); //true success, false fail
            static bool isGameInRace(); // true in race, false not in race
            static void updateRaceGoing();
            static u8 hasGameChangedMode(); // return a game state of the curr state once

            static void encodeFromVal(char* out, u64 in);
            static u8 getOnlinechecksum(OnlineSettings* onlineset);
            static void applycommsettings(u32* commdescptr);
            static void changeNumberRounds(u32 newval);
            static void loadDefaultSetOnline();
            static void loadCustomSetOnline(OnlineSettings* onlineset);
            static void restoreComTextPtr(u32* strptr = nullptr);
            static void restoreViewTextPtr(u32* strptr = nullptr);
            static u16* commDisplayText;
            static u32* oldStrloc;
            static u16 cc_selector_backup_val;
            static bool cc_selector_backup_en;
            static u32* (*getSaveManagerFuncptr)(void);
            static STGIEntry** (*getStageInfoFuncptr)(void);
            static void lockBottomMapChange(bool lock);
            static void unlockKartBody(KartUnlockParts kartID);
            static void unlockDriver(DriverUnlock driverID);
            //
            static void kmpConstructCallback();
            static u16 getSTGIEntryCount(STGIEntry** s);
    };
    bool checkCompTID(u64 tid);
    u32 SafeRead32(u32 addr);
}
