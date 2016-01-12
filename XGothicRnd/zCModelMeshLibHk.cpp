#include "pch.h"
#include "zCModelMeshLibHk.h"
#include "zCModelMeshLib.h"
#include "GVisual.h"

// Init global instance of this object
REGISTER_HOOK(zCModelMeshLibHk);

void InitMeshLib(zCModelMeshLib* lib)
{
	for (unsigned int i = 0; i < lib->GetMeshSoftSkinList()->GetSize(); i++)
	{
		zCMeshSoftSkin* mesh = lib->GetMeshSoftSkinList()->Array[i];
		GVisual::CreateExtensionVisual(mesh);
	}
}

/**
* Called whenever the game finshed changing the model of an npc
*/
void zCModelMeshLibHk::zCModelMeshLib__BuildFromModel(zCModelMeshLib* thisptr, void* edx, zCModel* model)
{
	// This sets the meshlibs and finishes up model initialization
	GET_HOOK(zCModelMeshLibHk).m_BuildFromModel(thisptr, model);

	InitMeshLib(thisptr);
}

void zCModelMeshLibHk::zCModelMeshLib__BuildFromModelProto(zCModelMeshLib* thisptr, void* edx, zCModelPrototype* model)
{
	// This sets the meshlibs and finishes up model initialization
	GET_HOOK(zCModelMeshLibHk).m_BuildFromModelProto(thisptr, model);

	InitMeshLib(thisptr);
}

zBOOL zCModelMeshLibHk::zCModelMeshLib__LoadMDM(zCFileBIN& file, zCModelPrototype* proto, zCModel* model, zCModelMeshLib** lib)
{
	zBOOL r = GET_HOOK(zCModelMeshLibHk).m_LoadMDM(file, proto, model, lib);

	if(lib)
		InitMeshLib(*lib);

	return r;
}

/**
* Called at the start of the program to apply hooks for this.
*/
zCModelMeshLibHk::zCModelMeshLibHk()
{
	m_BuildFromModel = Hooks::HookFunction<zEngine::GenericThiscall1PtrArg>
		(MemoryLocations::Gothic::zCModelMeshLib__BuildFromModel_zCModel_p,
			zCModelMeshLib__BuildFromModel);

	m_BuildFromModelProto = Hooks::HookFunction<zEngine::GenericThiscall1PtrArg>
		(MemoryLocations::Gothic::zCModelMeshLib__BuildFromModel_zCModelPrototype_p,
			zCModelMeshLib__BuildFromModelProto);

	m_LoadMDM = Hooks::HookFunction<zEngine::zCModelMeshLib__LoadMDM>
		(MemoryLocations::Gothic::zCModelMeshLib__LoadMDM_zCFileBIN_r_zCModelPrototype_p_zCModel_p_zCModelMeshLib_p_p,
			zCModelMeshLib__LoadMDM);
}