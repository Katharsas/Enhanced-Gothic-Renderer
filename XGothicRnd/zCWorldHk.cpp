#include "pch.h"
#include "zCWorldHk.h"
#include "MemoryLocations.h"
#include "zEngineHooks.h"
#include "MemoryLocations.h"
#include "GGame.h"
#include "Engine.h"
#include "GWorld.h"

// Init global instance of this object
REGISTER_HOOK(zCWorldHk);

/**
 * Called whenever the game loads a world from file
 */
int __fastcall zCWorldHk::zCWorld__LoadWorld(zCWorld* thisptr, void* edx, zSTRING const & file, zTWorldLoadMode mode)
{
	Engine::Game->OnLoadWorld(thisptr, file.ToChar(), mode);

	int r = GET_HOOK(zCWorldHk).m_LoadWorld(thisptr, file, mode);

	// Notify game
	Engine::Game->OnWorldLoaded(thisptr, file.ToChar(), mode);

	return r;
}

/** 
* Called when the game added a vob to a world
*/
void __fastcall zCWorldHk::zCWorld__VobAddedToWorld(zCWorld* thisptr, void* edx, zCVob* vob)
{
	GET_HOOK(zCWorldHk).m_VobAddedToWorld(thisptr, vob);

	GWorld::GetFromSource(thisptr)->AddVob(vob);
}

/**
* Called when the game removed a vob from a world
*/
void __fastcall zCWorldHk::zCWorld__VobRemovedFromWorld(zCWorld* thisptr, void* edx, zCVob* vob)
{
	GET_HOOK(zCWorldHk).m_VobRemovedFromWorld(thisptr, vob);

	GWorld::GetFromSource(thisptr)->RemoveVob(vob);
}

/**
* Called to completely remove everything from the world
*/
void __fastcall zCWorldHk::zCWorld__DisposeWorld(zCWorld* thisptr, void* edx)
{
	GET_HOOK(zCWorldHk).m_DisposeWorld(thisptr);
}

/**
 * Called at the start of the program to apply hooks for this.
 */
zCWorldHk::zCWorldHk()
{
	m_LoadWorld = Hooks::HookFunction<zEngine::zCWorld__LoadWorld>
		(MemoryLocations::Gothic::zCWorld__LoadWorld_zSTRING_const_r_zCWorld__zTWorldLoadMode, 
		zCWorld__LoadWorld);

	m_VobAddedToWorld = Hooks::HookFunction<zEngine::zCWorld__VobAddedToWorld>
		(MemoryLocations::Gothic::zCWorld__VobAddedToWorld_zCVob_p, 
		zCWorld__VobAddedToWorld);

	m_VobRemovedFromWorld = Hooks::HookFunction<zEngine::zCWorld__VobRemovedFromWorld>
		(MemoryLocations::Gothic::zCWorld__VobRemovedFromWorld_zCVob_p, 
			zCWorld__VobRemovedFromWorld);

	m_DisposeWorld = Hooks::HookFunction<zEngine::GenericThiscall>
		(MemoryLocations::Gothic::zCWorld__DisposeWorld_void, 
			zCWorld__DisposeWorld);
}