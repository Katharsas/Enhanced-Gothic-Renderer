#pragma once
#include "zDefinitions.h"

#define THISPTR_OFFSET(x) (((char *)this) + (x))

#pragma warning(disable: 4731) // Change of ebp from inline-assembly

// -- call macro from GothicX (thx, Zerxes!)
#define XCALL(uAddr)                    \
        __asm { mov esp, ebp    }       \
        __asm { pop ebp                 }       \
        __asm { mov eax, uAddr  }       \
        __asm { jmp eax                 }

class zCVisual;
class zCVob;
class zCFileBIN;
class zCModelPrototype;
class zCModelMeshLib;
class zCModel;
class zCTexture;

namespace zEngine
{
	typedef void(__thiscall* GenericThiscall)(void* thisptr);
	typedef void(__thiscall* GenericThiscall1PtrArg)(void* thisptr, void* arg1);
	typedef void (__thiscall* GenericConstructor)(void* thisptr);
	typedef void (__thiscall* GenericDestructor)(void* thisptr);
	typedef void (__thiscall* oCNPC__Enable)(void*, float3&);

	// zBOOL zCTexture::CacheInNamed (const zSTRING *texFileName)  
	typedef int (__thiscall* zCTexture__CacheInNamed)(zCTexture* thisptr, const zSTRING*);

	// int __thiscall zCOption::ReadBool(class zSTRING const &, char const *, int)
	typedef int (__thiscall* zCOption__ReadBool)(zCOption* thisptr, zSTRING const & section, char const* variable, int defValue);

	// int zCWorld::LoadWorld(class zSTRING const &, enum  zCWorld::zTWorldLoadMode)
	typedef int (__thiscall* zCWorld__LoadWorld)(zCWorld* thisptr, zSTRING const & file, int mode);

	// void __thiscall zCBspTree::Render(void)
	typedef void (__thiscall* zCBspTree__Render)(zCBspTree* thisptr);

	// int __cdecl vidGetFPSRate()
	typedef int (__cdecl* vidGetFPSRate)(void);

	// int __thiscall GetTextureBuffer(int, void*&, int &);
	typedef int (__thiscall* zCTexture__GetTextureBuffer)(void*, int, void*&, int&);

	// int __thiscall zCTex_D3D::XTEX_BuildSurfaces(int)
	typedef int (__thiscall* zCTexD3D__XTEX_BuildSurfaces)(void*,int);

	typedef const zSTRING* (__thiscall* zCVisual__GetFileExtension)(void*, int);
	typedef zCVisual* (__cdecl* zCVisual__LoadVisual)(const zSTRING&);	
	typedef void (__thiscall* zCWorld__VobAddedToWorld)(void*, zCVob*);
	typedef void (__thiscall* zCWorld__VobRemovedFromWorld)(void*, zCVob*);

	typedef zBOOL(__cdecl* zCModelMeshLib__LoadMDM)(zCFileBIN&, zCModelPrototype*, zCModel*, zCModelMeshLib**);

	// static LRESULT CALLBACK AppWndProc(HWND hwnd, DWORD msg, WPARAM wParam, LPARAM lParam)
	typedef LRESULT(__stdcall* AppWndProc)(HWND, DWORD, WPARAM, LPARAM);
	typedef void(__thiscall* zCVob__SetVisual)(void*, zCVisual*);
#if GAME_VERSION == VERSION_2_6_FIX
	typedef void(__thiscall* zCVob__EndMovement)(void*, zBOOL);
#else
	typedef void(__thiscall* zCVob__EndMovement)(void*); // One parameter less for G1
#endif
};