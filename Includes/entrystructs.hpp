#ifndef _ENTRYSTRUCTS_H
#define _ENTRYSTRUCTS_H
#include "CTRPluginFramework.hpp"
namespace CTRPluginFramework {
    struct SpeedValues
    {
        std::string unit;
        float magnitude;
        float maxgraphspeed;
        float maxgraphangle;
    };
    struct CCValues
    {
        u32             offset;
        float           original[45];
        float           other_original[3];
        CCValues(void) {
            offset = 0x15370002;
            memset(original, 0, sizeof(float) * 45);
            memset(other_original, 0, sizeof(u32) * 3);
        }
    };

    struct  CCSettings
    {
        bool            first;
        bool          enabled;
        u16             value;
        CCSettings(void)
        {
            first = true;
            enabled = false;
            value = 0;
        }
    } PACKED;
    struct  SpeedSettings
    {
        bool            first;
        bool            enabled;
        bool            graphical;
        u8              unit;
        SpeedSettings(void)
        {
            first = true;
            enabled = false;
            graphical = false;
            unit = 0;
        }
    } PACKED;
    extern MenuEntry *ccselectorentry;
    extern MenuEntry *speedometerentry;
    extern MenuEntry *backcamentry;
    extern MenuEntry *warnitementry;
    extern MenuEntry *comcodegenentry;
	extern MenuEntry *courmanentry;
	extern MenuEntry *numbRoundsEntry;
	extern MenuEntry *zoomLocalEntry;
}
#endif