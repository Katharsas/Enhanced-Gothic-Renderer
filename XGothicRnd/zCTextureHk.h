#pragma once
#include "ihookable.h"
#include "zEngineFunctions.h"

class zCTexture;
class zCTextureHk :
	public IHookable
{
public:
	/**
	 * Called at the start of the program to apply hooks for this.
	 */
	zCTextureHk();

	/**
	 * Object constructor
	 */
	static void __fastcall zCTexture__Constructor(zCTexture* thisptr, void* edx);

	/**
	 * Object destructor
	 */
	static void __fastcall zCTexture__Destructor(zCTexture* thisptr, void* edx);

	/**
	 * Called when the game locks a surface. After this method, we should have a valid surface-pointer.
	 * Might be called on other thread!
	 */
	static int __fastcall zCTexture__GetTextureBuffer(zCTexture* thisptr, void* edx, int mip, void*& buffer, int& pitch);

	/**
	 * Called when the original renderer creates a surface for a zCTexture
	 */
	static zBOOL __fastcall zCTexD3D__XTEX_BuildSurfaces(zCTexture* thisptr, void* edx, zBOOL decompress);

	/** Called when a texture is being loaded */
	static zBOOL __fastcall zCTexture__CacheInNamed(zCTexture* thisptr, void* edx, const zSTRING *texFileName);
private:

	zEngine::GenericConstructor m_Constructor;
	zEngine::GenericDestructor m_Destructor;
	zEngine::zCTexture__GetTextureBuffer m_GetTextureBuffer;
	zEngine::zCTexD3D__XTEX_BuildSurfaces m_XTEX_BuildSurfaces;
	zEngine::zCTexture__CacheInNamed m_CacheInNamed;
};

