#pragma once

class GGame;
namespace Engine
{
	/**
	 * Initializes these global objects
	 */
	bool Initialize();

	/**
	 * Uninitialize everything
	 */
	void Uninitialize();

	__declspec(selectany) GGame* Game;
};