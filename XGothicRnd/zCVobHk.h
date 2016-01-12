#pragma once
#include "pch.h"
#include "IHookable.h"
#include "zEngineFunctions.h"
#include "zDefinitions.h"
#include "zSTRING.h"


class zCVobHk : public IHookable
{
public:
	/**
	* Called at the start of the program to apply hooks for this.
	*/
	zCVobHk();

	/** Called when this vob is about to change the visual */
	static void __fastcall zCVob__SetVisual(zCVob* thisptr, void* edx, zCVisual* visual);

	/**
	* Called whenever a vob moved
	*/
#if GAME_VERSION == VERSION_2_6_FIX
	static void __fastcall zCVob__EndMovement(zCVob* thisptr, void* edx, zBOOL transformChanged);
#else
	static void __fastcall zCVob__EndMovement(zCVob* thisptr, void* edx); // One parameter less for G1
#endif
private:
	zEngine::zCVob__EndMovement m_EndMovement;
	zEngine::zCVob__SetVisual m_SetVisual;
};
