#include "pch.h"
#include "oCNpcHk.h"
#include "MemoryLocations.h"
#include "zEngineHooks.h"
#include "MemoryLocations.h"
#include "GGame.h"
#include "Engine.h"
#include "zCVisual.h"
#include "GVisual.h"
#include "GWorld.h"
#include "GMainResources.h"
#include <REngine.h>
#include <RResourceCache.h>
#include "zCVob.h"
#include "oCNpc.h"
#include "GModelVisual.h"
#include "GVobObject.h"

// Init global instance of this object
REGISTER_HOOK(oCNpcHk);

void UpdateModel(oCNpc* npc)
{
	GWorld* wld = GWorld::GetFromSource(npc->GetHomeWorld());

	// Handle this as if the whole visual changed, remove and add the vob again, recreate the visual.
	// This will update everything, but isn't the fastest way but a very robust one. 
	// Since these don't happen often it should be okay, though.

	GModelVisual* vis = (GModelVisual*)GModelVisual::QueryFromSource(npc->GetVisual());
	GVobObject* vob = GVobObject::QueryFromSource(npc);

	if(!vis)
		LogInfo() << "Updating NPC with NULL visual!";

	if(wld && vob)
	{
		if(!wld->RemoveVob(npc))
			return; // Vob wasn't in the world

		vob = GVobObject::QueryFromSource(npc);
	}

	// Update GModel. Each zCModel has only one oCNpc it is used by. Has to be done after the vob was removed, so the vob 
	// doesn't operate on a deleted visual
	vis = (GModelVisual*)GModelVisual::QueryFromSource(npc->GetVisual());
	delete vis;
	vis = (GModelVisual*)GVisual::CreateExtensionVisual(npc->GetVisual());

	if(wld)
		wld->AddVob(npc);
}

/**
* Called whenever the game finshed changing the model of an npc
*/
void oCNpcHk::oCNpc__InitModel(oCNpc* thisptr, void* edx)
{
	// This sets the meshlibs and finishes up model initialization
	GET_HOOK(oCNpcHk).m_InitModel(thisptr);

	// Don't need to do any work if we don't have a visual yet (load-time)
	if (!thisptr->GetVisual() || !thisptr->GetHomeWorld())
		return;

	UpdateModel(thisptr);

	//GASSERT((GVobObject::QueryFromSource(thisptr) != nullptr), "NPC-Vob not in the world after InitModel!");
	GASSERT((GModelVisual::QueryFromSource(thisptr->GetVisual()) != nullptr), "NPC-Visual not created after InitModel!");
}

/** Enables a stored NPC */
void __fastcall oCNpcHk::oCNPC__Enable(oCNpc* thisptr, void* unknwn, float3& position)
{
	GET_HOOK(oCNpcHk).m_Enable(thisptr, position);
	
	//UpdateModel(thisptr);

	GASSERT((GVobObject::QueryFromSource(thisptr) != nullptr), "NPC-Vob not in the world after Enable!");
	GASSERT((GModelVisual::QueryFromSource(thisptr->GetVisual()) != nullptr), "NPC-Visual not created after Enable!");
}

/** Disables an NPC and frees it's memory */
void __fastcall oCNpcHk::oCNPC__Disable(oCNpc* thisptr, void* unknwn)
{
	GWorld* wld = GWorld::GetFromSource(thisptr->GetHomeWorld());

	if(wld && !thisptr->IsAPlayer())
	{
		GModelVisual* vis = (GModelVisual*)GModelVisual::QueryFromSource(thisptr->GetVisual());

		GVobObject* vob = GVobObject::QueryFromSource(thisptr);
		wld->RemoveVob(thisptr);
		
		// Delete, in case RemoveVob didn't already do it
		vob = GVobObject::QueryFromSource(thisptr);
		delete vob;

		// Update GModel. Each zCModel has only one oCNpc it is used by. Has to be done after the vob was removed, so the vob 
		// doesn't operate on a deleted visual
		vis = (GModelVisual*)GModelVisual::QueryFromSource(thisptr->GetVisual());
		delete vis;
	
		GASSERT((GVobObject::QueryFromSource(thisptr) == nullptr), "NPC-Vob not deleted after Disable!");
		GASSERT((GModelVisual::QueryFromSource(thisptr->GetVisual()) == nullptr), "NPC-Visual not deleted after Disable!");
	}

	GET_HOOK(oCNpcHk).m_Disable(thisptr);
}

/**
* Called at the start of the program to apply hooks for this.
*/
oCNpcHk::oCNpcHk()
{
	m_InitModel = Hooks::HookFunction<zEngine::GenericThiscall>
		(MemoryLocations::Gothic::oCNpc__InitModel_void,
			oCNpc__InitModel);

	m_Enable = Hooks::HookFunction<zEngine::oCNPC__Enable>
		(MemoryLocations::Gothic::oCNpc__Enable_zVEC3_r,
			oCNPC__Enable);

	m_Disable = Hooks::HookFunction<zEngine::GenericThiscall>
		(MemoryLocations::Gothic::oCNpc__Disable_void,
			oCNPC__Disable);
}