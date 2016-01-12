#pragma once
#include "pch.h"
#include "IHookable.h"
#include "zEngineFunctions.h"
#include "zDefinitions.h"
#include "zSTRING.h"


class zCWorldHk : public IHookable
{
public:
	/**
	 * Called at the start of the program to apply hooks for this.
	 */
	zCWorldHk();

	/**
	 * Called whenever the game loads a world from file
	 */
	static int __fastcall zCWorld__LoadWorld(zCWorld* thisptr, void* edx, zSTRING const & file, zTWorldLoadMode mode);

	/** 
	 * Called when the game added a vob to a world
	 */
	static void __fastcall zCWorld__VobAddedToWorld(zCWorld* thisptr, void* edx, zCVob* vob);

	/**
	 * Called when the game removed a vob from a world
	 */
	static void __fastcall zCWorld__VobRemovedFromWorld(zCWorld* thisptr, void* edx, zCVob* vob);

	/**
	 * Called to completely remove everything from the world
	 */
	static void __fastcall zCWorld__DisposeWorld(zCWorld* thisptr, void* edx);
private:
	zEngine::zCWorld__LoadWorld m_LoadWorld;
	zEngine::zCWorld__VobAddedToWorld m_VobAddedToWorld;
	zEngine::zCWorld__VobRemovedFromWorld m_VobRemovedFromWorld;
	zEngine::GenericThiscall m_DisposeWorld;
};
