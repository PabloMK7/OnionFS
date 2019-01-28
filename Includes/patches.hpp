#pragma once
#include "CTRPluginFramework.hpp"

namespace CTRPluginFramework {
	class Patches
	{
	public:
		static void applyCodeBinPatch();
		static void applyCodeIpsPatch();
	};
}