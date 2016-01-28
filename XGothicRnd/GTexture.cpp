#include "pch.h"
#include "GTexture.h"
#include <REngine.h>
#include <RResourceCache.h>
#include "zCTexture.h"
#include "zCResourceManager.h"
#include "Engine.h"
#include "GGame.h"
#include "GMainResources.h"
#include <RTextureAtlas.h>

int g_TotalSize = 0;

GTexture::GTexture(zCTexture* sourceObject) : GzObjectExtension<zCTexture, GTexture>(sourceObject)	
{
	m_Texture = REngine::ResourceCache->CreateResource<RTexture>();
	m_CurrentSurface = nullptr;
}


GTexture::~GTexture(void)
{
	// Remove the pointer to this from the surface
	if(m_CurrentSurface)
		m_CurrentSurface->SetExternalEngineTexture(nullptr);

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
	m_CurrentSurface = surface;
}

/** Called when the game decided to cache out the surface */
void GTexture::OnSurfaceDeleted()
{
	if(m_Texture->IsInitialized())
		g_TotalSize -= m_Texture->GetSizeInBytes();

	// Game cached the texture out, deallocate...
	//LogInfo() << "Deallocating texture: " << m_SourceObject->GetObjectName();
	m_Texture->Deallocate();

	m_CurrentSurface = nullptr;
}

/** Called when the connected surface was unlocked, giving a pointer to the memory
	the game filled with image-data */
void GTexture::OnSurfaceUnlocked(void* imageData, unsigned int sizeInBytes, std::vector<void*> mipData)
{
	// If this is called, it is save that we have a surface
	m_CurrentSurface = GetSurface();
	DDSURFACEDESC2 ddsd;
	m_CurrentSurface->GetSurfaceDesc(&ddsd);

	// Make sure the texture is clean
	m_Texture->Deallocate();

	g_TotalSize += sizeInBytes;
	LogInfo() << "Initializing texture: " << m_SourceObject->GetObjectName() << " (" << sizeInBytes << " bytes)"
		<< " (Total: " << g_TotalSize / (1024 * 1024) << " mb)";

	// Create the texture on the GPU
	LEB(m_Texture->CreateTexture(imageData, 
		sizeInBytes, 
		INT2(ddsd.dwWidth, ddsd.dwHeight), 
		m_CurrentSurface->ComputeBitsPerPixel() != 16 ? ddsd.dwMipMapCount : 1, // Force mipcount to 1 on 16-bit
		m_CurrentSurface->GetInternalTextureFormat(),
		B_SHADER_RESOURCE,
		U_DEFAULT,
		1,
		mipData));

	// TODO: This is a hack, find a better way to figure out if this is a lightmap! | There is "IsLightmap" on zCTexture!
	// If this is a lightmap, add to lightmap-atlas cache
	if(ddsd.dwMipMapCount == 1 && ddsd.dwWidth == ddsd.dwHeight && m_CurrentSurface->GetInternalTextureFormat() == ETextureFormat::TF_R8G8B8A8)
	{
		Engine::Game->GetMainResources()->GetLightmapAtlas(INT2(ddsd.dwWidth, ddsd.dwHeight))->StoreTexture((byte*)imageData, sizeInBytes, m_SourceObject);
	}
}

/** Returns a pointer to the surface associated with this object,
	only if the texture is currently cached in */
MyDirectDrawSurface7* GTexture::GetSurface()
{
	return m_CurrentSurface ? m_CurrentSurface : m_SourceObject->GetSurface();
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