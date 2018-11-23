#ifndef CHEATS_H
#define CHEATS_H


// RELEASE BUILD
#define RELEASE_BUILD
//

#include "CTRPluginFramework.hpp"
#include "Lang.hpp"
#ifndef RELEASE_BUILD
	#include "OSDManager.hpp"
#endif



namespace CTRPluginFramework
{
	#ifndef RELEASE_BUILD
		#define TRACE  { OSDManager["trace"].SetScreen(true).SetPos(10,50) = std::string(__FUNCTION__) << ":" << __LINE__; Sleep(Seconds(2)); }
		#define XTRACE(id, posx, posy, str, ...) { OSDManager[id].SetScreen(true).SetPos(posx,posy) = std::string(__FUNCTION__) << ": " << __LINE__ << Utils::Format(str, __VA_ARGS__); Sleep(Seconds(0.04f)); }
		#define NOXTRACEPOS(id, posx, posy, str, ...) { OSDManager[id].SetScreen(true).SetPos(posx,posy) = Utils::Format(str, __VA_ARGS__); Sleep(Seconds(0.04f)); }
		#define NOXTRACE(id, str, ...) { OSDManager[id].SetScreen(true).SetPos(10,50) = Utils::Format(str, __VA_ARGS__); Sleep(Seconds(0.04f)); }
	
	#else
		#define TRACE
		#define XTRACE
		#define NOXTRACE
		#define NOXTRACEPOS
	#endif

	#define ISGAMEONLINE (g_isOnlineMode & 0x01)

	using StringVector = std::vector<std::string>;

	extern bool gi_isUpdated;
	extern bool ge_isUpdated;

	u32 socinitcallback(Handle* soc_handle, Handle memhandle, u32 size);
	u32 socexitcallback();

	void	menucallback();

	void 	warnItemUse(MenuEntry *entry);

	void	speedometer_apply(MenuEntry *entry);
	void 	speedometer();
	void 	speedometersettings(MenuEntry *entry);
	
	void    ccselector_apply(MenuEntry *entry);
	void    ccselector(MenuEntry *entry);
	void    ccselectorsettings(MenuEntry *entry);
	
	void openNTRmenu(MenuEntry *entry);
	
	void backwardscam(MenuEntry *entry);
	
	void createcommcode(MenuEntry *entry);
	void foolsCallBack(MenuEntry *entry);

	void onMenuChangeCallback(u32 menuID);

	void changeRoundNumber(MenuEntry *entry);

	void enhancedlocalmap(MenuEntry *entry);
	void enhancedlocalmap_apply(bool mode, u8 force = 0);
}
#endif
