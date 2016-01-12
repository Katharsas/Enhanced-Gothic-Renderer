#include "pch.h"
#include "Engine.h"
#include "GGame.h"
#include "../Shared/Logger.h"
#include "zEngineHooks.h"


namespace Engine
{

	/**
	 * Initializes these global objects
	 */
	bool Initialize()
	{
		//LogInfo() << "Placing hooks...";
		//Hooks::ApplyHooks();

		//LogInfo() << &GET_HOOK(zCWorldHk);

		LogInfo() << "Initializing Engine...";
		Game = new GGame();
		
		return true;
	}

	/**
	 * Uninitialize everything
	 */
	void Uninitialize()
	{
		delete Game;
	}

}