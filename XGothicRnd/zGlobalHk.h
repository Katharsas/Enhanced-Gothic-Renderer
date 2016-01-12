#pragma once
#include "ihookable.h"
#include "zEngineFunctions.h"
#include "zDefinitions.h"
#include "zSTRING.h"

class zGlobalHk : public IHookable
{
public:

	/**
	 * Called at the start of the program to apply hooks for this.
	 */
	zGlobalHk();

	/**
	 * Called to get the current FPS
	 */
	static int vidGetFPSRate();

	
	/**
	 * Since the window-proc was inlined, we have to use the WinAPI
	 * to hook their message callback
	 */
	static bool PlaceWndHook(HWND hwnd);

	/**
	* Applications main-windows message callback
	*/
	static LRESULT CALLBACK AppWndProc(HWND hwnd, DWORD msg, WPARAM wParam, LPARAM lParam);

private:

	zEngine::vidGetFPSRate m_vidGetFPSRate;

	// Pointer to the original code of the message callback
	LONG_PTR m_AppWndProc;
};

