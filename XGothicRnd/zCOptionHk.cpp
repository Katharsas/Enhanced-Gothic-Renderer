#include "pch.h"
#include "zCOptionHk.h"

// Init global instance of this object
REGISTER_HOOK(zCOptionHk);

/**
* Called at the start of the program to apply hooks for this.
*/
zCOptionHk::zCOptionHk()
{
		m_ReadBool = Hooks::HookFunction<zEngine::zCOption__ReadBool>
		(MemoryLocations::Gothic::zCOption__ReadBool_zSTRING_const_r_char_const_p_int, 
		zCOption__ReadBool);
}

/**
* Called whenever the game loads a world from file
*/
int __fastcall zCOptionHk::zCOption__ReadBool(zCOption* thisptr, void* edx, zSTRING const & section, char const* variable, int defValue)
{
	if (_stricmp(variable, "gameAbnormalExit") == 0)
	{
		// No VDFS bullshit when testing
		return 0;
	}/*
	else if(_stricmp(variable, "zSkyDome") == 0)
	{
		// No fancy skydome for now
		// TODO
		return 0;
	}*/

	return GET_HOOK(zCOptionHk).m_ReadBool(thisptr, section, variable, defValue);
}