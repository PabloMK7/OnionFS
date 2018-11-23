#pragma once
#include "CTRPluginFramework.hpp"

namespace CTRPluginFramework {
    class PatternManager
    {
    using   PatternCallback = bool(*)(u32);
    public:

        class PatternEntry
        {
            friend class PatternManager;
        public:
            PatternEntry(u8* pattern, u32 size, u32 key, PatternCallback cb);
            ~PatternEntry(void);
        private:
            u8*     decPat;
            size_t  patsize;
            PatternCallback callback;
        };

        PatternManager(void) = default;
        ~PatternManager(void);

        void    Add(PatternEntry* entry);
        void    Remove(PatternEntry* entry);
        void    Perform(void);

    private:
        std::vector<PatternEntry*> entries{};

        void    FreeAllEntries();
        void    FreeEntry(int i);
    };
}
