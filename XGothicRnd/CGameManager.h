#pragma once
#include "pch.h"
#include "zEngineFunctions.h"

class CGameManager
{
public:
	
	/** Loads the savegame at the given slot */
	void Read_Savegame(int slot)
	{
		XCALL(MemoryLocations::Gothic::CGameManager__Read_Savegame_int);
	}

	/** Returns a pointer to the current game-manager */
	static CGameManager* GetCGameManager()
	{
		return *(CGameManager**)MemoryLocations::Gothic::CGameManager_p_gameMan;
	}
};