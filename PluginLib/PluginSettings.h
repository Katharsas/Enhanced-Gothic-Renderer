#pragma once

namespace GPlugin
{
	/** Possible things this plugin can do.
		Return a bitmask containing these in GP_GetFlags() */
	enum EPluginFlags
	{
		PF_REPLACE_DDRAW = 1,	
	};
};