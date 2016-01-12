#pragma once
#include "zDefinitions.h"
#include "zSTRING.h"

class zCObject
{
public:
	// Reimplementation if the release-method
	void Release()
	{
		m_RefCounter--;
		int temp = m_RefCounter;
		if(temp <= 0)
		{
			// Call destructor
			delete this;
		}
	}

	// Recreate V-Table
	virtual	zCClassDef*	GetClassDef() = 0;
	virtual void Archive() = 0;
	virtual void Unarchive() = 0;
	virtual ~zCObject(){}

	/**
	 * Returns the name of this object
	 */
	const char* GetObjectName()
	{
		return __GetObjectName().ToChar();
	}

protected:
	const zSTRING& __GetObjectName()
	{
		XCALL(MemoryLocations::Gothic::zCObject__GetObjectName_void);
	}

	int m_RefCounter;
	zWORD m_HashIndex;
	zCObject* m_HashNext;
	zSTRING m_ObjectName;	
};

