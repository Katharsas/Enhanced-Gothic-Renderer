#pragma once
#include "IHookable.h"
#include "zEngineFunctions.h"
#include "zDefinitions.h"
#include "zSTRING.h"

class zCModel;
class zCModelMeshLib;
class zCModelMeshLibHk : public IHookable
{
public:
	/**
	* Called at the start of the program to apply hooks for this.
	*/
	zCModelMeshLibHk();

	/**
	* Called whenever the game finshed loading the model of an npc
	*/
	static void __fastcall zCModelMeshLib__BuildFromModel(zCModelMeshLib* thisptr, void* edx, zCModel* model);
	static void __fastcall zCModelMeshLib__BuildFromModelProto(zCModelMeshLib* thisptr, void* edx, zCModelPrototype* model);
	static zBOOL zCModelMeshLib__LoadMDM(zCFileBIN& file, zCModelPrototype* proto, zCModel* model, zCModelMeshLib** lib);

private:
	zEngine::GenericThiscall1PtrArg m_BuildFromModel;
	zEngine::GenericThiscall1PtrArg m_BuildFromModelProto;
	zEngine::zCModelMeshLib__LoadMDM m_LoadMDM;
};