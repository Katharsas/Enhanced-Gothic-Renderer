#include "pch.h"
#include "GTexture.h"
#include <REngine.h>
#include <RResourceCache.h>
#include "zCTexture.h"
#include "zCResourceManager.h"

int g_TotalSize = 0;

GTexture::GTexture(zCTexture* sourceObject) : GzObjectExtension<zCTexture, GTexture>(sourceObject)	
{
	m_Texture = REngine::ResourceCache->CreateResource<RTexture>();
}


GTexture::~GTexture(void)
{
	// Remove the pointer to this from the surface
	if(m_SourceObject->GetSurface())
		m_SourceObject->GetSurface()->SetExternalEngineTexture(nullptr);

	if(!m_Texture)
	{
		LogWarn() << "Deleting GTexture without any RTexture created!";
		return;
	}

	if(m_Texture->IsInitialized())
		LogWarn() << "Deleting GTexture with still initialized RTexture!";

	REngine::ResourceCache->DeleteResource(m_Texture);
}

/** Called just after the game created locked the underlaying surface. */
void GTexture::OnSurfaceLocked(MyDirectDrawSurface7* surface)
{

}

/** Called when the game decided to cache out the surface */
void GTexture::OnSurfaceDeleted()
{
	g_TotalSize -= m_Texture->GetSizeInBytes();

	// Game cached the texture out, deallocate...
	//LogInfo() << "Deallocating texture: " << m_SourceObject->GetObjectName();
	m_Texture->Deallocate();
}

/** Called when the connected surface was unlocked, giving a pointer to the memory
	the game filled with image-data */
void GTexture::OnSurfaceUnlocked(void* imageData, unsigned int sizeInBytes, std::vector<void*> mipData)
{
	// If this is called, it is save that we have a surface
	MyDirectDrawSurface7* srf = GetSurface();
	DDSURFACEDESC2 ddsd;
	srf->GetSurfaceDesc(&ddsd);

	g_TotalSize += sizeInBytes;
	LogInfo() << "Initializing texture: " << m_SourceObject->GetObjectName() << " (" << sizeInBytes << " bytes)"
		<< " (Total: " << g_TotalSize / (1024 * 1024) << " mb)";

	// Make sure the texture is clean
	m_Texture->Deallocate();

	// Create the texture on the GPU
	LEB(m_Texture->CreateTexture(imageData, 
		sizeInBytes, 
		INT2(ddsd.dwWidth, ddsd.dwHeight), 
		ddsd.dwMipMapCount,
		srf->GetInternalTextureFormat(),
		B_SHADER_RESOURCE,
		U_DEFAULT,
		1,
		mipData));
}

/** Returns a pointer to the surface associated with this object,
	only if the texture is currently cached in */
MyDirectDrawSurface7* GTexture::GetSurface()
{
	return m_SourceObject->GetSurface();
}

/** Returns whether the underlaying texture-object is fully initialized */
bool GTexture::IsFullyInitialized()
{
	return m_Texture->IsInitialized();
}

/** Caches the texture or simply returns the cache-state if already in process or done */
zTResourceCacheState GTexture::CacheIn(bool threaded)
{
	// TODO: Maybe only check for in debug builds
	if(m_Invalid)
	{
		LogWarn() << "Operating on invalid texture: " << m_ObjectName;
		return zRES_CACHED_OUT;
	}

	return m_SourceObject->CacheIn(threaded ? DEFAULT_CACHE_IN_PRIORITY : -1);
}