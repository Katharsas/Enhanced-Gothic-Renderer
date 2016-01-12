#include "pch.h"
#include "zCVisualHk.h"
#include "MemoryLocations.h"
#include "zEngineHooks.h"
#include "MemoryLocations.h"
#include "GGame.h"
#include "Engine.h"
#include "zCVisual.h"
#include "GVisual.h"
#include "GMainResources.h"
#include <REngine.h>
#include <RResourceCache.h>

// Init global instance of this object
REGISTER_HOOK(zCVisualHk);

/**
 * Called whenever the game loads a visual
 */
zCVisual* zCVisualHk::zCVisual__LoadVisual(const zSTRING& name)
{
	zCVisual* r = GET_HOOK(zCVisualHk).m_LoadVisual(name);

	//LogInfo() << "Loaded visual: " << name.ToChar() << " (0x" << r << ")";

	// Models are handled differently
	if(r && r->GetVisualType() != zCVisual::VT_MODEL)
	{
		// Create and register the extension-visual
		GVisual::CreateExtensionVisual(r);
	}

	return r;
}

/**
 * Called at the start of the program to apply hooks for this.
 */
zCVisualHk::zCVisualHk()
{
	m_LoadVisual = Hooks::HookFunction<zEngine::zCVisual__LoadVisual>
		(MemoryLocations::Gothic::zCVisual__LoadVisual_zSTRING_const_r, 
		zCVisual__LoadVisual);
}