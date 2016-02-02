#include <windows.h>
#include <string>
#include <vector>
#include "../Shared/PluginInterface.h"

// Function-decleration for DirectDrawCreateEx
typedef HRESULT (WINAPI *DirectDrawCreateEx_t)(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter);

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

/** Loaded plugins */
std::vector<std::pair<HMODULE, GPlugin::IPlugin*>> g_LoadedPlugins;

/** Loads the given plugin */
bool LoadPlugin(const char* file)
{
	HMODULE plugin = LoadLibrary(file);
	if(!plugin)
		return false;

	// Call the startup function to get an object
	GPlugin::InitPlugin_t init = (GPlugin::InitPlugin_t)GetProcAddress(plugin, "InitPlugin");
	GPlugin::ClosePlugin_t close = (GPlugin::ClosePlugin_t)GetProcAddress(plugin, "ClosePlugin");
	FARPROC ddrawCreateEx = GetProcAddress(plugin, "DirectDrawCreateEx");

	if(!ddrawCreateEx && (!init || !close))
		return false;

	// Only use interface if this was built for it
	GPlugin::IPlugin* pluginInterface = nullptr;
	if(init)
	{
		pluginInterface = init();

		if(!pluginInterface->OnStartup())
		{
			// Plugin decided that it should not be loaded
			close(pluginInterface);
			FreeLibrary(plugin);

			return true;
		}
	}

	// Save in plugin-stash
	g_LoadedPlugins.push_back(std::make_pair(plugin, pluginInterface));
	return true;
}

/** Loads all plugins in the given folder */
bool LoadPlugins(const char* dir)
{
	WIN32_FIND_DATAA ffd;
	HANDLE hFind = FindFirstFile((std::string(dir) + "*.dll").c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind) 
		return false;

	do
	{
		// Load the plugin, if this is a dll-file
		std::string name = ffd.cFileName;
		if(name.find(".dll") != std::string::npos)
		{
			std::string n = std::string(dir) + ffd.cFileName;

			// Perform load
			OutputDebugString("Loading Plugin: ");
			OutputDebugString(n.c_str());
			OutputDebugString("\n");
			LoadPlugin(n.c_str());
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	return true;
}



/** Our hooked DirectDrawCreateEx */
extern "C" HRESULT WINAPI HookedDirectDrawCreateEx(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter) {

	static bool s_pluginsLoaded = false;
	if(!s_pluginsLoaded)
	{
		// Some DLLs expect to be in the system-folder (Current directoy is one below system)
		SetCurrentDirectory("system\\");

		// Load plugins
		LoadPlugins("Plugins\\");

		// Reset to where we were
		SetCurrentDirectory("..\\");

		s_pluginsLoaded = true;

		// Call this again, in case we got hooked
		return HookedDirectDrawCreateEx(lpGuid, lplpDD, iid, pUnkOuter);
	}

	// Check if one of the plugins implements the DirectDrawCreateEx-Function
	// Note: Only the first occurence of this is used
	for(auto& p : g_LoadedPlugins)
	{
		FARPROC ddcEx = GetProcAddress(p.first, "DirectDrawCreateEx");

		if(ddcEx)
		{
			DirectDrawCreateEx_t oProc = (DirectDrawCreateEx_t)ddcEx;
			return oProc(lpGuid, lplpDD, iid, pUnkOuter);	
		}
	}

	DirectDrawCreateEx_t oProc = (DirectDrawCreateEx_t)ddraw.DirectDrawCreateEx;
	return oProc(lpGuid, lplpDD, iid, pUnkOuter);	
}

#pragma region ProxyFunctions
__declspec(naked) void FakeAcquireDDThreadLock()			{ _asm { jmp [ddraw.AcquireDDThreadLock] } }
__declspec(naked) void FakeCheckFullscreen()				{ _asm { jmp [ddraw.CheckFullscreen] } }
__declspec(naked) void FakeCompleteCreateSysmemSurface()	{ _asm { jmp [ddraw.CompleteCreateSysmemSurface] } }
__declspec(naked) void FakeD3DParseUnknownCommand()			{ _asm { jmp [ddraw.D3DParseUnknownCommand] } }
__declspec(naked) void FakeDDGetAttachedSurfaceLcl()		{ _asm { jmp [ddraw.DDGetAttachedSurfaceLcl] } }
__declspec(naked) void FakeDDInternalLock()					{ _asm { jmp [ddraw.DDInternalLock] } }
__declspec(naked) void FakeDDInternalUnlock()				{ _asm { jmp [ddraw.DDInternalUnlock] } }
__declspec(naked) void FakeDSoundHelp()						{ _asm { jmp [ddraw.DSoundHelp] } }
__declspec(naked) void FakeDirectDrawCreate()				{ _asm { jmp [ddraw.DirectDrawCreate] } }
__declspec(naked) void FakeDirectDrawCreateClipper()		{ _asm { jmp [ddraw.DirectDrawCreateClipper] } }
__declspec(naked) void FakeDirectDrawCreateEx()				{ _asm { jmp [ddraw.DirectDrawCreateEx] } }
__declspec(naked) void FakeDirectDrawEnumerateA()			{ _asm { jmp [ddraw.DirectDrawEnumerateA] } }
__declspec(naked) void FakeDirectDrawEnumerateExA()			{ _asm { jmp [ddraw.DirectDrawEnumerateExA] } }
__declspec(naked) void FakeDirectDrawEnumerateExW()			{ _asm { jmp [ddraw.DirectDrawEnumerateExW] } }
__declspec(naked) void FakeDirectDrawEnumerateW()			{ _asm { jmp [ddraw.DirectDrawEnumerateW] } }
__declspec(naked) void FakeDllCanUnloadNow()				{ _asm { jmp [ddraw.DllCanUnloadNow] } }
__declspec(naked) void FakeDllGetClassObject()				{ _asm { jmp [ddraw.DllGetClassObject] } }
__declspec(naked) void FakeGetDDSurfaceLocal()				{ _asm { jmp [ddraw.GetDDSurfaceLocal] } }
__declspec(naked) void FakeGetOLEThunkData()				{ _asm { jmp [ddraw.GetOLEThunkData] } }
__declspec(naked) void FakeGetSurfaceFromDC()				{ _asm { jmp [ddraw.GetSurfaceFromDC] } }
__declspec(naked) void FakeRegisterSpecialCase()			{ _asm { jmp [ddraw.RegisterSpecialCase] } }
__declspec(naked) void FakeReleaseDDThreadLock()			{ _asm { jmp [ddraw.ReleaseDDThreadLock] } }
#pragma endregion

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
}

/** Entry-point */
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) 
	{
		// Extract addresses of the original ddraw.dll
		ExtractDDRAWExports();

	} else if (reason == DLL_PROCESS_DETACH) {
		FreeLibrary(ddraw.dll);

		// Clear plugins
		for(auto& p : g_LoadedPlugins)
		{
			GPlugin::ClosePlugin_t close = (GPlugin::ClosePlugin_t)GetProcAddress(p.first, "ClosePlugin");

			if(close && p.second)
			{
				// Call ending-method
				p.second->OnShutdown();

				// Delete the plugin-interface
				close(p.second);
			}

			FreeLibrary(p.first);
		}
	}
	return TRUE;
};
