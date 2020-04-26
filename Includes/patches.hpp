#pragma once
#include "CTRPluginFramework.hpp"

namespace CTRPluginFramework {
	class Patches
	{
	public:
		static void applyCodeBinPatch();
		static void applyCodeIpsPatch();
		// static void applySectionsPatch();
		// static void dumpSectionsPatch();
	};
}