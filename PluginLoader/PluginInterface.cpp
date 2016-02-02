#include "PluginInterface.h"

#if 0

namespace GPlugin
{
	/** Creates an instance of the plugin */
	GPLUGIN_API GPlugin::IPlugin* InitPlugin()
	{
		return new /** YOUR PLUGIN-CLASS HERE */;
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

#endif