#include "pch.h"
#include "zCTextureHk.h"
#include "MemoryLocations.h"
#include "GTexture.h"
#include "zCTexture.h"

// Init global instance of this object
REGISTER_HOOK(zCTextureHk);

/**
* Called at the start of the program to apply hooks for this.
*/
zCTextureHk::zCTextureHk()
{
		m_GetTextureBuffer = Hooks::HookFunction<zEngine::zCTexture__GetTextureBuffer>
		(MemoryLocations::Gothic::zCTex_D3D__GetTextureBuffer_int_void_p_r_int_r, 
		zCTexture__GetTextureBuffer);

		m_Constructor = Hooks::HookFunction<zEngine::GenericConstructor>
			(MemoryLocations::Gothic::zCTexture__zCTexture_void, zCTexture__Constructor);

		m_Destructor = Hooks::HookFunction<zEngine::GenericDestructor>
			(MemoryLocations::Gothic::zCTexture__dzCTexture_void, zCTexture__Destructor);

		m_XTEX_BuildSurfaces = Hooks::HookFunction<zEngine::zCTexD3D__XTEX_BuildSurfaces>
			(MemoryLocations::Gothic::zCTex_D3D__XTEX_BuildSurfaces_int, zCTexD3D__XTEX_BuildSurfaces);
}

/**
 * Object constructor
 */
void __fastcall zCTextureHk::zCTexture__Constructor(zCTexture* thisptr, void* edx)
{
	GET_HOOK(zCTextureHk).m_Constructor(thisptr);

	// Construct object for this
	// GTexture::GetFromSource(thisptr);
}

/**
* Object destructor
*/
void __fastcall zCTextureHk::zCTexture__Destructor(zCTexture* thisptr, void* edx)
{
	GTexture* tex = GTexture::GetFromSource(thisptr);

	// Delete our version of this as well
	delete tex;

	GET_HOOK(zCTextureHk).m_Destructor(thisptr);
}

/**
* Called whenever the game loads a world from file
*/
int __fastcall zCTextureHk::zCTexture__GetTextureBuffer(zCTexture* thisptr, void* edx, int mip, void*& buffer, int& pitch)
{
	int r = GET_HOOK(zCTextureHk).m_GetTextureBuffer(thisptr, mip, buffer, pitch);

	
	return r;
}

/**
* Called when the original renderer creates a surface for a zCTexture
*/
zBOOL __fastcall zCTextureHk::zCTexD3D__XTEX_BuildSurfaces(zCTexture* thisptr, void* edx, zBOOL decompress)
{
	zBOOL r = GET_HOOK(zCTextureHk).m_XTEX_BuildSurfaces(thisptr, decompress);

	GTexture* extRes = GTexture::GetFromSource(thisptr);

	// There should be a surface after this functioncall
	if(thisptr->GetSurface())
	{
		// Surface should be freshly created. Tell it about the engine-object
		thisptr->GetSurface()->SetExternalEngineTexture(extRes);
		extRes->OnSurfaceLocked(thisptr->GetSurface());
	}else
	{
		LogWarn() << "No surface after zCTexD3D__XTEX_BuildSurfaces!";
	}

	return r;
}