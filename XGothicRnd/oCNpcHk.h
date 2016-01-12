#pragma once
#include "IHookable.h"
#include "zEngineFunctions.h"
#include "zDefinitions.h"
#include "zSTRING.h"

class oCNpc;
class oCNpcHk : public IHookable
{
public:
	/**
	* Called at the start of the program to apply hooks for this.
	*/
	oCNpcHk();

	/**
	* Called whenever the game finshed changing the model of an npc
	*/
	static void __fastcall oCNpc__InitModel(oCNpc* thisptr, void* edx);

	/** Disables an NPC and frees it's memory */
	static void __fastcall oCNPC__Disable(oCNpc* thisptr, void* unknwn);
	
	/** Enables a stored NPC */
	static void __fastcall oCNPC__Enable(oCNpc* thisptr, void* unknwn, float3& position);

private:
	zEngine::GenericThiscall m_InitModel;
	zEngine::oCNPC__Enable m_Enable;
	zEngine::GenericThiscall m_Disable;
};

