#include "pch.h"
#include "zCBspTreeHk.h"
#include "MemoryLocations.h"
#include "Engine.h"
#include "GGame.h"
#include "zEngineHooks.h"

// Init global instance of this object
REGISTER_HOOK(zCBspTreeHk);

zCBspTreeHk::zCBspTreeHk(void)
{
#ifdef HOOK_RENDERING
	m_Render = Hooks::HookFunction<zEngine::zCBspTree__Render>
		(MemoryLocations::Gothic::zCBspTree__Render_void, 
		zCBspTree__Render);
#endif
}

/**
 * Called every frame on rendertime
 */
void zCBspTreeHk::zCBspTree__Render(zCBspTree* thisptr, void* edx)
{
	//GET_HOOK(zCBspTreeHk).m_Render(thisptr);
	Engine::Game->OnRender();
}