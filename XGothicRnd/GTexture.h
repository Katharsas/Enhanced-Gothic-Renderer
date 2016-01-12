#pragma once
#include "GzObjectExtension.h"
#include "zDefinitions.h"

class zCTexture;
class MyDirectDrawSurface7;

class GTexture : public GzObjectExtension<zCTexture, GTexture>
{
public:
	GTexture(zCTexture* sourceObject);
	~GTexture(void);

	/** Called just after the game created locked the underlaying surface. */
	void OnSurfaceLocked(MyDirectDrawSurface7* surface);

	/** Called when the game decided to cache out the surface */
	void OnSurfaceDeleted();

	/** Called when the connected surface was unlocked, giving a pointer to the memory
		the game filled with image-data. May get called from an other thread! */
	void OnSurfaceUnlocked(void* imageData, unsigned int sizeInBytes, std::vector<void*> mipData = std::vector<void*>());

	/** Returns a pointer to the surface associated with this object,
		only if the texture is currently cached in */
	MyDirectDrawSurface7* GetSurface();

	/** Returns the actual usable texture of the renderer */
	RTexture* GetTexture(){return m_Texture;}

	/** Returns whether the underlaying texture-object is fully initialized */
	bool IsFullyInitialized();

	/** Caches the texture or simply returns the cache-state if already in process or done */
	zTResourceCacheState CacheIn(bool threaded=true);
private:
	RTexture* m_Texture;
};

