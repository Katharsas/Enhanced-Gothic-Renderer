#pragma once
#include "zSTRING.h"
#include "zCArray.h"

#define CHECK_ZCLASS_SIZE(checkclass) if(zCClassDef::GetClassDef(#checkclass)){ \
		GASSERT(zCClassDef::GetClassDef(#checkclass)->GetClassSize() == sizeof(checkclass), "Class " #checkclass " has wrong size! See log for difference. "); \
		LogInfo() << "sizeof(" #checkclass ") = " << sizeof(checkclass) << " while class def says " << zCClassDef::GetClassDef(#checkclass)->GetClassSize(); } \
		else { \
		GASSERT(false, "No class-def found for class " #checkclass); }

class zCObject;
class zCClassDef
{
public:
	// Returns the name of the class, eg. "zCObject"
	const char* GetClassName()
	{
		return m_ClassName.ToChar();
	}

	// Returns the size of the class the game knows. Useful for checking for size-mismatches between versions.
	DWORD GetClassSize()
	{
		return m_ClassSize;
	}

	// Returns the class-def for the given classname
	static zCClassDef* GetClassDef(const zSTRING& className)
	{
		XCALL(MemoryLocations::Gothic::zCClassDef__GetClassDef_zSTRING_const_r);
	}

private:

	

	zSTRING m_ClassName;
	zSTRING m_BaseClassName;
	zSTRING m_ScriptClassName;
	zCClassDef* m_BaseClassDef;
	zCObject* m_CreateNewInstance;
	zCObject* m_CreateNewInstanceBackup;
	DWORD m_ClassFlags;
	DWORD m_ClassSize;
	int m_NumLivingObjects;
	int m_NumCtorCalled;
	zCObject **m_hashTable;
	zCArray<zCObject*> m_ObjectList;
	zWORD m_ArchiveVersion;
	zWORD m_ArchiveVersionSum;
};