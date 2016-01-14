#include "pch.h"

#include "ddraw.h"
#include "d3d.h"
#include "D3D7/MyDirectDraw.h"
#include "../Shared/Logger.h"
#include "DbgHelp.h"
#include "Hooks.h"
#include "Engine.h"
#include "GGame.h"
#include "SmartHeap.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Imagehlp.lib")

// Signal nvidia drivers that we want the high-performance card on laptops
// TODO: Doesn't seem to work, though!
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

// Function-decleration for DirectDrawCreateEx
static HRESULT (WINAPI *DirectDrawCreateEx_t)(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter);

// Container for all exports of the DDRAW.dll
struct ddraw_dll
{
	HMODULE dll;
	FARPROC	AcquireDDThreadLock;
	FARPROC	CheckFullscreen;
	FARPROC	CompleteCreateSysmemSurface;
	FARPROC	D3DParseUnknownCommand;
	FARPROC	DDGetAttachedSurfaceLcl;
	FARPROC	DDInternalLock;
	FARPROC	DDInternalUnlock;
	FARPROC	DSoundHelp;
	FARPROC	DirectDrawCreate;
	FARPROC	DirectDrawCreateClipper;
	FARPROC	DirectDrawCreateEx;
	FARPROC	DirectDrawEnumerateA;
	FARPROC	DirectDrawEnumerateExA;
	FARPROC	DirectDrawEnumerateExW;
	FARPROC	DirectDrawEnumerateW;
	FARPROC	DllCanUnloadNow;
	FARPROC	DllGetClassObject;
	FARPROC	GetDDSurfaceLocal;
	FARPROC	GetOLEThunkData;
	FARPROC	GetSurfaceFromDC;
	FARPROC	RegisterSpecialCase;
	FARPROC	ReleaseDDThreadLock;
} ddraw;

/** Our hooked DirectDrawCreateEx */
extern "C" HRESULT WINAPI HookedDirectDrawCreateEx(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter) {
	// Initialize main graphics engine, if we don't have one yet
	if(!REngine::RenderingDevice)
	{
		// Initialize rendering-engine
		REngine::InitializeEngine();

		// Create a rendering device
		REngine::RenderingDevice->CreateDevice();

		// Initialize Gothic-dependent Engine
		if(!Engine::Initialize())
			LogErrorBox() << "XRND: Failed to initialize Engine!";
	};

	// Create fake D3D7-Device to get the game starting
	*lplpDD = new MyDirectDraw(NULL);

	return S_OK;
}

__declspec(naked) void FakeAcquireDDThreadLock()			{ _asm { jmp [ddraw.AcquireDDThreadLock] } }
__declspec(naked) void FakeCheckFullscreen()				{ _asm { jmp [ddraw.CheckFullscreen] } }
__declspec(naked) void FakeCompleteCreateSysmemSurface()	{ _asm { jmp [ddraw.CompleteCreateSysmemSurface] } }
__declspec(naked) void FakeD3DParseUnknownCommand()			{ _asm { jmp [ddraw.D3DParseUnknownCommand] } }
__declspec(naked) void FakeDDGetAttachedSurfaceLcl()		{ _asm { jmp [ddraw.DDGetAttachedSurfaceLcl] } }
__declspec(naked) void FakeDDInternalLock()					{ _asm { jmp [ddraw.DDInternalLock] } }
__declspec(naked) void FakeDDInternalUnlock()				{ _asm { jmp [ddraw.DDInternalUnlock] } }
__declspec(naked) void FakeDSoundHelp()						{ _asm { jmp [ddraw.DSoundHelp] } }
// HRESULT WINAPI DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
__declspec(naked) void FakeDirectDrawCreate()				{ _asm { jmp [ddraw.DirectDrawCreate] } }
// HRESULT WINAPI DirectDrawCreateClipper( DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter );
__declspec(naked) void FakeDirectDrawCreateClipper()		{ _asm { jmp [ddraw.DirectDrawCreateClipper] } }
// HRESULT WINAPI DirectDrawCreateEx( GUID FAR * lpGuid, LPVOID *lplpDD, REFIID iid,IUnknown FAR *pUnkOuter );
__declspec(naked) void FakeDirectDrawCreateEx()				{ _asm { jmp [ddraw.DirectDrawCreateEx] } }
// HRESULT WINAPI DirectDrawEnumerateA( LPDDENUMCALLBACKA lpCallback, LPVOID lpContext );
__declspec(naked) void FakeDirectDrawEnumerateA()			{ _asm { jmp [ddraw.DirectDrawEnumerateA] } }
// HRESULT WINAPI DirectDrawEnumerateExA( LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags );
__declspec(naked) void FakeDirectDrawEnumerateExA()			{ _asm { jmp [ddraw.DirectDrawEnumerateExA] } }
// HRESULT WINAPI DirectDrawEnumerateExW( LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags );
__declspec(naked) void FakeDirectDrawEnumerateExW()			{ _asm { jmp [ddraw.DirectDrawEnumerateExW] } }
// HRESULT WINAPI DirectDrawEnumerateW( LPDDENUMCALLBACKW lpCallback, LPVOID lpContext );
__declspec(naked) void FakeDirectDrawEnumerateW()			{ _asm { jmp [ddraw.DirectDrawEnumerateW] } }
__declspec(naked) void FakeDllCanUnloadNow()				{ _asm { jmp [ddraw.DllCanUnloadNow] } }
__declspec(naked) void FakeDllGetClassObject()				{ _asm { jmp [ddraw.DllGetClassObject] } }
__declspec(naked) void FakeGetDDSurfaceLocal()				{ _asm { jmp [ddraw.GetDDSurfaceLocal] } }
__declspec(naked) void FakeGetOLEThunkData()				{ _asm { jmp [ddraw.GetOLEThunkData] } }
__declspec(naked) void FakeGetSurfaceFromDC()				{ _asm { jmp [ddraw.GetSurfaceFromDC] } }
__declspec(naked) void FakeRegisterSpecialCase()			{ _asm { jmp [ddraw.RegisterSpecialCase] } }
__declspec(naked) void FakeReleaseDDThreadLock()			{ _asm { jmp [ddraw.ReleaseDDThreadLock] } }

/** Turns on all exceptions */
void EnableCrashingOnCrashes()
{
	typedef BOOL (WINAPI *tGetPolicy)(LPDWORD lpFlags);
	typedef BOOL (WINAPI *tSetPolicy)(DWORD dwFlags);
	const DWORD EXCEPTION_SWALLOWING = 0x1;

	HMODULE kernel32 = LoadLibraryA("kernel32.dll");
	if(!kernel32)
		return;

	tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32,
		"GetProcessUserModeExceptionPolicy");
	tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32,
		"SetProcessUserModeExceptionPolicy");
	if (pGetPolicy && pSetPolicy)
	{
		DWORD dwFlags;
		if (pGetPolicy(&dwFlags))
		{
			// Turn off the filter
			pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING);
		}
	}
}

/** Extracts all exports of the DDRAW.dll and puts them into teh ddraw-object, in case we need one 
	of its functions again */
void ExtractDDRAWExports()
{
	char infoBuf[MAX_PATH];
	GetSystemDirectoryA(infoBuf, MAX_PATH);
	// We then append \ddraw.dll, which makes the string:
	// C:\windows\system32\ddraw.dll
	// Or syswow64, since we're on x86 here.
	strcat_s(infoBuf, MAX_PATH, "\\ddraw.dll");

	ddraw.dll = LoadLibraryA(infoBuf);     
	if(!ddraw.dll)
		return;

	ddraw.AcquireDDThreadLock			= GetProcAddress(ddraw.dll, "AcquireDDThreadLock");
	ddraw.CheckFullscreen				= GetProcAddress(ddraw.dll, "CheckFullscreen");
	ddraw.CompleteCreateSysmemSurface	= GetProcAddress(ddraw.dll, "CompleteCreateSysmemSurface");
	ddraw.D3DParseUnknownCommand		= GetProcAddress(ddraw.dll, "D3DParseUnknownCommand");
	ddraw.DDGetAttachedSurfaceLcl		= GetProcAddress(ddraw.dll, "DDGetAttachedSurfaceLcl");
	ddraw.DDInternalLock				= GetProcAddress(ddraw.dll, "DDInternalLock");
	ddraw.DDInternalUnlock				= GetProcAddress(ddraw.dll, "DDInternalUnlock");
	ddraw.DSoundHelp					= GetProcAddress(ddraw.dll, "DSoundHelp");
	ddraw.DirectDrawCreate				= GetProcAddress(ddraw.dll, "DirectDrawCreate");
	ddraw.DirectDrawCreateClipper		= GetProcAddress(ddraw.dll, "DirectDrawCreateClipper");
	ddraw.DirectDrawCreateEx			= GetProcAddress(ddraw.dll, "DirectDrawCreateEx");
	ddraw.DirectDrawEnumerateA			= GetProcAddress(ddraw.dll, "DirectDrawEnumerateA");
	ddraw.DirectDrawEnumerateExA		= GetProcAddress(ddraw.dll, "DirectDrawEnumerateExA");
	ddraw.DirectDrawEnumerateExW		= GetProcAddress(ddraw.dll, "DirectDrawEnumerateExW");
	ddraw.DirectDrawEnumerateW			= GetProcAddress(ddraw.dll, "DirectDrawEnumerateW");
	ddraw.DllCanUnloadNow				= GetProcAddress(ddraw.dll, "DllCanUnloadNow");
	ddraw.DllGetClassObject				= GetProcAddress(ddraw.dll, "DllGetClassObject");
	ddraw.GetDDSurfaceLocal				= GetProcAddress(ddraw.dll, "GetDDSurfaceLocal");
	ddraw.GetOLEThunkData				= GetProcAddress(ddraw.dll, "GetOLEThunkData");
	ddraw.GetSurfaceFromDC				= GetProcAddress(ddraw.dll, "GetSurfaceFromDC");
	ddraw.RegisterSpecialCase			= GetProcAddress(ddraw.dll, "RegisterSpecialCase");
	ddraw.ReleaseDDThreadLock			= GetProcAddress(ddraw.dll, "ReleaseDDThreadLock");

	*(void**)&DirectDrawCreateEx_t = (void*)GetProcAddress(ddraw.dll, "DirectDrawCreateEx");
}

/** Entry-point */
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID) {
	if (reason == DLL_PROCESS_ATTACH) 
	{
		// Don't let the old smartheap-library patch the CRT, if compile flags want.
		UnpatchSmartHeap();

		// Turn of exception-filter to get all crashes reported
		EnableCrashingOnCrashes();

		// Start with a fresh log
		Log::Clear();
		LogInfo() << "Starting DDRAW Proxy DLL.";

		ExtractDDRAWExports();

	} else if (reason == DLL_PROCESS_DETACH) {
		FreeLibrary(ddraw.dll);

		REngine::UninitializeEngine();

		LogInfo() << "DDRAW Proxy DLL signing off.\n";
	}
	return TRUE;
};
