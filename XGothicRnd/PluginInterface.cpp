#include "pch.h"
#include "../Shared/PluginInterface.h"
#include "Hooks.h"
#include "zEngineHooks.h"
#include <REngine.h>
#include "Engine.h"
#include "GGame.h"
#include "D3D7\MyDirectDraw.h"

#pragma comment(lib, "dbghelp.lib")

namespace GPlugin
{
	class EnhancedRendererPlugin : public IPlugin
	{
	public:
		/** Called right after the game started */
		virtual bool OnStartup()
		{
			ApplyHooks();
			return true;
		}

		/** Called right before the game closes */
		virtual bool OnShutdown()
		{
			return true;
		}

	private:

		/** Hooks DirectDrawCreateEx to supply our own device */
		void ApplyHooks()
		{
			HMODULE ddraw = LoadLibrary("ddraw.dll");
			GASSERT(ddraw != nullptr, "Failed to load ddraw.dll!");

			FARPROC createEx = GetProcAddress(ddraw, "DirectDrawCreateEx");
			GASSERT(createEx != nullptr, "Failed to find DirectDrawCreateEx!");

			// Set the hook...
			Hooks::HookDirect(createEx, hooked_DirectDrawCreateEx);
		}

		/** Hooked version of DirectDrawCreateEx */
		static HRESULT WINAPI hooked_DirectDrawCreateEx(GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter) {

			// Initialize main graphics engine, if we don't have one yet
			if(!RAPI::REngine::RenderingDevice)
			{
				// Initialize rendering-engine
				RAPI::REngine::InitializeEngine();

				// Create a rendering device
				RAPI::REngine::RenderingDevice->CreateDevice();

				// Initialize Gothic-dependent Engine
				if(!Engine::Initialize())
					LogErrorBox() << "XRND: Failed to initialize Engine!";
			};

			// Create fake D3D7-Device to get the game starting
			*lplpDD = new MyDirectDraw(NULL);

			return S_OK;
		}
	};

	/** Creates an instance of the plugin */
	GPLUGIN_API GPlugin::IPlugin* InitPlugin()
	{
		return new EnhancedRendererPlugin;
	}

	/** Deletes the instance of the plugin */
	GPLUGIN_API void ClosePlugin(GPlugin::IPlugin* plugin)
	{
		delete plugin;
	}

	/** Returns the API-Version of this plugins interface */
	GPLUGIN_API APIVersion GetAPIVersion()
	{
		return PLUGIN_API_VERSION;
	}
};