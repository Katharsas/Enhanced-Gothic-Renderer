#include "pch.h"
#include "zCVobHk.h"
#include "MemoryLocations.h"
#include "zEngineHooks.h"
#include "MemoryLocations.h"
#include "zCVob.h"
#include "GVobObject.h"
#include "GWorld.h"

// Init global instance of this object
REGISTER_HOOK(zCVobHk);

/** Called when this vob is about to change the visual */
void __fastcall zCVobHk::zCVob__SetVisual(zCVob* thisptr, void* edx, zCVisual* visual)
{
	if(!visual && thisptr->GetVobObject())
	{
		GWorld* wld = GWorld::QueryFromSource(thisptr->GetHomeWorld());

		if(wld)
		{
			wld->RemoveVob(thisptr);

			// Remove vob from world
			delete thisptr->GetVobObject();
		}
	}

	GET_HOOK(zCVobHk).m_SetVisual(thisptr, visual);
}


#if GAME_VERSION == VERSION_2_6_FIX

/**
* Called whenever a vob moved
*/
void __fastcall zCVobHk::zCVob__EndMovement(zCVob* thisptr, void* edx, zBOOL transformChanged)
{
	GVobObject* vobj = thisptr->GetVobObject();
	GVobObject* vobjq = GVobObject::QueryFromSource(thisptr);
	//if(vobj != vobjq)
	//	LogWarn() << "zCVob has wrong pointer to GVobObject!";

	// PB didn't use this parameter really carefully. It's way faster to simply check the transform
	// a couple of times more than needed. Vobs for particles have this set to true for example, even if they 
	// don't really need it!
	if(transformChanged && vobj && vobj->GetWorldMatrix() == thisptr->GetWorldMatrix())
	{
		transformChanged = FALSE;	
	}

	GET_HOOK(zCVobHk).m_EndMovement(thisptr, transformChanged);

	/*__try{
		GET_HOOK(zCVobHk).m_EndMovement(thisptr, transformChanged);
	}
	__except(true)
	{
		auto fn = [](){LogWarn() << "Exception while EndMovement!";};
		fn();
	}*/
	// G2 actually checks if the transforms changed and passes this as parameter
	// But not always, so do another check. Also G1 doesn't check this at all
	if (vobj && (transformChanged || vobj->GetWorldMatrix() != thisptr->GetWorldMatrix()))
	{
		vobj->UpdateVob();
	}
}

#else

/**
* Called whenever a vob moved
*/
void __fastcall zCVobHk::zCVob__EndMovement(zCVob* thisptr, void* edx)
{
	GET_HOOK(zCVobHk).m_EndMovement(thisptr);

	// Check if the vob actually changed. G1 calls this function for all vobs, even if it
	// didn't change anything
	if (thisptr->GetVobObject() && thisptr->GetVobObject()->GetWorldMatrix() != thisptr->GetWorldMatrix())
	{
		thisptr->GetVobObject()->UpdateVob();
	}
}

#endif

/**
* Called at the start of the program to apply hooks for this.
*/
zCVobHk::zCVobHk()
{
#if GAME_VERSION == VERSION_2_6_FIX
	m_EndMovement = Hooks::HookFunction<zEngine::zCVob__EndMovement>
		(MemoryLocations::Gothic::zCVob__EndMovement_int,
			zCVob__EndMovement);
#else
	m_EndMovement = Hooks::HookFunction<zEngine::zCVob__EndMovement>
		(MemoryLocations::Gothic::zCVob__EndMovement_void,
			zCVob__EndMovement);
#endif

	m_SetVisual = Hooks::HookFunction<zEngine::zCVob__SetVisual>
		(MemoryLocations::Gothic::zCVob__SetVisual_zCVisual_p,
			zCVob__SetVisual);
}