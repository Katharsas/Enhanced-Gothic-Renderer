#pragma once
#include <string>
#include "zDefinitions.h"
#include "BasicTimer.h"
#include "GConstantBuffers.h"

/**
 * Game-managing class. Handles world creation and destruction, as well as hooks and other critical stuff
 */

class GWorld;
class RBuffer;
class GMainResources;
class GGame
{
public:
	GGame(void);
	~GGame(void);

	/**
	 * Initializes basic resources like default pipeline states
	 */
	bool Initialize();

	/**
	 * Called when the game started to load a new world
	 */
	void OnLoadWorld(zCWorld* world, const std::string& file, zTWorldLoadMode mode);

	/**
	 * Called when a zCWorld was loaded
	 */
	void OnWorldLoaded(zCWorld* world, const std::string& file, zTWorldLoadMode mode);

	/**
	 * Called when the world got disposed 
	 */
	void OnWorldDisposed(zCWorld* world);

	/**
	 * Called when a new frame should be rendered
	 */
	void OnRender();

	/**
	 * Called when a new frame started
	 */
	void OnFrameStart();

	/**
	 * Called when the frame ended
	 */
	void OnFrameEnd();

	/**
	 * Called when we got a message from the games window
	 */
	void OnWindowMessage(HWND hwnd, DWORD msg, WPARAM wParam, LPARAM lParam);

	/**
	 * Called when we got a keydown or keyup-event
	 */
	void OnKeyEvent(byte key, bool pressed);

	/**
	 * Puts the given function into a queue so it can be executed when the code reaches a safe place.
	 * This is very useful for operations which may have something active on an other thread.
	 * These are executed right after beginning rendering a new frame.
	 */
	void QueueSafeFunction(std::function<void()> fn);

	/**
	 * Returns the time the last frame took in ms
	 */
	float GetFrameTimeMS();

	/**
	 * Returns the Frames per second we are rendering at
	 */
	float GetFramesPerSecond();

	/**
	 * Returns the general resources needed to render the game
	 */
	GMainResources* GetMainResources(){return m_MainResources;};

	/**
	 * Adds a single line to this frames debug-output
	 */
	void AddFrameDebugLine(const std::string& line);

	/**
	 * Draws the statistics of the renderer using ingame-methods
	 */
	void DrawStatistics();

	/** Returns the currently active main world */
	GWorld* GetActiveWorld()
	{
		return m_ActiveWorld;
	}

	/** Gets the current profiler-data and formats it into a string */
	std::string FormatProfilerData();
private:

	/**
	 * Puts the sky-parameters into the constantbuffer 
	 */
	void ExtractSkyParameters(ConstantBuffers::PerFrameConstantBuffer& cb);

	/**
	 * Kills all loaded visuals. Needs to be called before loading, since gothic reuses it's objects
	 * and I don't want to.
	 */
	void DeleteLoadedVisualExtensions();

	/** 
	 * Executes the function stored as "save"
	 */
	void ExecuteSaveFunctions();

	/**
	 * Currently active main world
	 */
	GWorld* m_ActiveWorld;

	/**
	 * High precision-timer for our own FPS calculation, since Gothics only goes up to 255
	 */
	BasicTimer m_FPSTimer;

	/**
	 * General resources needed to render the game
	 */
	GMainResources* m_MainResources;

	/**
	 * The debugoutput to draw at the end of this frame. 
	 * This string will be cleared when a new frame starts. Push text to it using AddFrameDebugLine()! */
	std::string m_FrameDebugLines;

	/**
	 * Code to execute when we got to a safe position
	 */
	std::vector<std::function<void()>> m_SaveFunctions;
};

