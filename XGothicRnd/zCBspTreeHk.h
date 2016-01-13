#pragma once
#include "zEngineFunctions.h"
#include "IHookable.h"

class zCBspTreeHk : public IHookable
{
public:
	zCBspTreeHk(void);

	/**
	 * Called every frame on rendertime
	 */
	static void __fastcall zCBspTree__Render(zCBspTree* thisptr, void* edx);

private:
	zEngine::zCBspTree__Render m_Render;
};

