#include "pch.h"
#include "zCBspTreeHk.h"
#include "MemoryLocations.h"
#include "Engine.h"
#include "GGame.h"
#include "zEngineHooks.h"
#include "zCBspTree.h"
#include "oCGame.h"
#include "zCWorld.h"
#include "GBspTree.h"
#include "GWorld.h"

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

	// Only draw the main world like this
	if(oCGame::GetGame()->GetWorld()->GetBspTree() == thisptr)
	{
		Engine::Game->OnRender();
	}
	else
	{
		// This should be an inventory-tree. Get a pointer to it's contained world and
		// execute special renderlogic for that
		GBspTree* gTree = GBspTree::QueryFromSource(thisptr);
		if(gTree)
			gTree->GetContainedWorld()->RenderInventoryCell();
	}
}