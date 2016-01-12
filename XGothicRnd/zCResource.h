#pragma once
#include "zCObject.h"
#include "zCResourceManager.h"

struct zCCriticalSection
{
	int __vtbl;
	CRITICAL_SECTION m_CriticalSection;
};

class zCResource : public zCObject
{
public:

	struct zResFlags
	{					
		byte CacheState	: 2; // Maps to zTResourceCacheState
		byte CacheOutLock : 1;
		byte CacheClassIndex : 8;
		byte ManagedByResMan : 1;
		unsigned short CacheInPriority : 16;
		byte CanBeCachedOut	: 1;
	};

	/**
	 * Returns the flags of this resource 
	 */
	struct zResFlags& GetFlags()
	{
		return m_Flags;
	}

	/**
	 * Returns the current cache state. A resource could be loading
	 * on the worker-thread for example 
	 */
	zTResourceCacheState GetCacheState()
	{
		return (zTResourceCacheState)m_Flags.CacheState;
	}

	/**
	 * Requests a cache-in for this resource. If the resource is already
	 * loaded, will refresh the timestamps
	 */
	zTResourceCacheState CacheIn(float priority)
	{
		if (GetCacheState()==zRES_CACHED_IN)
		{
			TouchTimeStamp();
		} else if(GetCacheState()==zRES_CACHED_OUT)
		{
			// Cache the texture, overwrite priority if wanted.
			zCResourceManager::GetResourceManager()->CacheIn(this, priority);
		}

		return GetCacheState();
	}

	void TouchTimeStamp()
	{
		XCALL(MemoryLocations::Gothic::zCResource__TouchTimeStamp_void);
	}

	void TouchTimeStampLocal()
	{
		XCALL(MemoryLocations::Gothic::zCResource__TouchTimeStampLocal_void);
	}

protected:
	zCResource* m_NextResource;
	zCResource* m_PrevResource;
	DWORD m_LastTimeUsed;

	zCCriticalSection m_StateChangeGuard;

	zResFlags m_Flags;
};