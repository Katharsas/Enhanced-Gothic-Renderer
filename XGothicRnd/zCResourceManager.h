#pragma once
#include "pch.h"
#include "zDefinitions.h"
#include "zEngineFunctions.h"

const float DEFAULT_CACHE_IN_PRIORITY = 0.6f;

class zCResource;
class zCResourceManager
{
public:
	/** Issues a load-request to the worker-thread. These are
		then sorted by the priority you gave in.
		Special case: priority == -1: Texture is immediately cached in
		on the main-thread. */
	zTResourceCacheState CacheIn(zCResource *res, float priority = DEFAULT_CACHE_IN_PRIORITY)
	{
		XCALL(MemoryLocations::Gothic::zCResourceManager__CacheIn_zCResource_p_float);
	}

	/** Returns the global instance of this object */
	static zCResourceManager* GetResourceManager()
	{
		return *(zCResourceManager**)MemoryLocations::Gothic::zCResourceManager_p_zresMan;
	}
};
