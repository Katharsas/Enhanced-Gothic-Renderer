#pragma once
#include "ihookable.h"
#include "zEngineFunctions.h"
#include "zDefinitions.h"
#include "zSTRING.h"

class zCOptionHk :
	public IHookable
{
public:

	/**
	 * Called at the start of the program to apply hooks for this.
	 */
	zCOptionHk();

	/**
	 * Called whenever the game loads a world from file
	 */
	static int __fastcall zCOption__ReadBool(zCOption* thisptr, void* edx, zSTRING const & section, char const* variable, int defValue);

private:

	zEngine::zCOption__ReadBool m_ReadBool;
};

