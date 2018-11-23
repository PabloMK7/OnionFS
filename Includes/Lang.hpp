
#ifndef LANG_HPP
#define LANG_HPP

#include "types.h"
#include "CTRPluginFramework/System/System.hpp"
#include <string>
#include <vector>
#include <tuple>
#include <map>

#define NAME(key)           (Language::GetName(key))
#define NOTE(key)           (Language::GetNote(key))

namespace CTRPluginFramework
{
    using LangMap = std::map<std::string, std::tuple<std::string, std::string>>;
    using LangMapIter = LangMap::iterator;
    class Language
    {        
        using LangTuple = std::tuple<std::string, std::string>;
    public:
        static void     Initialize(void);

        static const std::string &GetName(const std::string &key);
        static const std::string &GetNote(const std::string &key);

        static LangMap&     Japanese;
        static LangMap&     English;
        static LangMap&     French;
        static LangMap&     German;
        static LangMap&     Italian;
        static LangMap&     Spanish;
        static LangMap&     ChineseSimplified;
        static LangMap&     Korean;
        static LangMap&     Dutch;
        static LangMap&     Portugese;
        static LangMap&     Russian;
        static LangMap&     ChineseTraditional;

		static u32* commonMsbtPtr;
		static u32* menuMsbtPtr;

		class MsbtHandler
		{
		public:
			MsbtHandler(u32* ptr);
			u32* getTextPtrById(u32 id);
			void replaceAllIngameText(int offset = 0);
			bool replaceIngameText(u32 id, const u16* text = nullptr);
			bool restoreAllIngameText();
			bool restoreIngameText(u32 id);
			bool applyStringText(u32 id, std::string &str);
			void updatePtr(u32* ptr);
			u32* msbtTablePtr;
			std::vector<u32> msbtOriPtr;
			std::vector<u32> msbtCusPtr;
			std::vector<bool> textEnabled;

		private:

		};

		static void			applyMsbt(char id, Language::MsbtHandler& hnd);
		static MsbtHandler* commonMsbt;
		static MsbtHandler* menuMsbt;
    private:
        static const LangTuple &_FindKey(const std::string &key);
        static void     _ImportLanguage(void);
        static LangMap  _languages[12];
        static u32      _currentLanguage;
    };
}

#endif
