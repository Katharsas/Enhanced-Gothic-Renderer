#include "pch.h"
#include "zGlobalHk.h"
#include "Engine.h"
#include "GGame.h"

// Init global instance of this object
REGISTER_HOOK(zGlobalHk);

/**
* Called at the start of the program to apply hooks for this.
*/
zGlobalHk::zGlobalHk()
{
		m_vidGetFPSRate = Hooks::HookFunction<zEngine::vidGetFPSRate>
		(MemoryLocations::Gothic::vidGetFPSRate_void, 
		vidGetFPSRate);
}

/**
* Called whenever the game loads a world from file
*/
int zGlobalHk::vidGetFPSRate()
{
	// Use our counter as assistance, when the original one comes to its limit
	/*int fps = GET_HOOK(zGlobalHk).m_vidGetFPSRate();

	if(fps == 256)
		fps = (int)(Engine::Game->GetFramesPerSecond()+0.5f);*/

	int fps = (int)(Engine::Game->GetFramesPerSecond()+0.5f);

	return fps;
}

bool zGlobalHk::PlaceWndHook(HWND hwnd)
{
	GET_HOOK(zGlobalHk).m_AppWndProc = GetWindowLongPtrA(hwnd, GWL_WNDPROC);
	SetWindowLongPtrA(hwnd, GWL_WNDPROC, (LONG)AppWndProc);

	return true;
}

/**
* Applications main-windows message callback
*/
LRESULT CALLBACK zGlobalHk::AppWndProc(HWND hwnd, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	// Call our own proc-code
	Engine::Game->OnWindowMessage(hwnd, msg, wParam, lParam);

	// Call the games wnd-proc
	return CallWindowProc((WNDPROC)GET_HOOK(zGlobalHk).m_AppWndProc, hwnd, msg, wParam, lParam);
}