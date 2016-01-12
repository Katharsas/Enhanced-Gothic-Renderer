#pragma once
#include "ihookable.h"
#include "zEngineFunctions.h"
#include "zDefinitions.h"

class zCObject;
class zCObjectHk :
	public IHookable
{
public:

	/**
	* Called at the start of the program to apply hooks for this.
	*/
	zCObjectHk();

	static void __fastcall zCObject__Destructor(zCObject* thisptr);

private:

	zEngine::GenericDestructor m_Destructor;
};

