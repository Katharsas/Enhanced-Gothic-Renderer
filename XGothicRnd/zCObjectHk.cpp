#include "pch.h"
#include "zCObjectHk.h"
#include "GzObjectExtension.h"

// Init global instance of this object
REGISTER_HOOK(zCObjectHk);

/**
* Called at the start of the program to apply hooks for this.
*/
zCObjectHk::zCObjectHk()
{
	m_Destructor = Hooks::HookFunction<zEngine::GenericDestructor>
		(MemoryLocations::Gothic::zCObject__dzCObject_void, 
			zCObject__Destructor);
}


void __fastcall zCObjectHk::zCObject__Destructor(zCObject* thisptr)
{
	// Template-magic is needed to call that function for zCObject
	// TODO: Improve
	auto obj = GObject::QueryFromSource(thisptr);

	if(obj)
	{
		obj->DeleteSafe();
	}

	GET_HOOK(zCObjectHk).m_Destructor(thisptr);
}