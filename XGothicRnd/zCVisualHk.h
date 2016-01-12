#pragma once
#include "pch.h"
#include "IHookable.h"
#include "zEngineFunctions.h"
#include "zDefinitions.h"
#include "zSTRING.h"


class zCVisualHk : public IHookable
{
public:
	/**
	 * Called at the start of the program to apply hooks for this.
	 */
	zCVisualHk();

	/**
	 * Called whenever the game loads a visual
	 */
	static class zCVisual* zCVisual__LoadVisual(const zSTRING& name);

private:
	zEngine::zCVisual__LoadVisual m_LoadVisual;
};
