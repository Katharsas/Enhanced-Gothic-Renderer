#pragma once
#include "zSTRING.h"

class zCClassDef
{
public:
	// Returns the name of the class, eg. "zCObject"
	const char* GetClassName()
	{
		return m_ClassName.ToChar();
	}

private:
	zSTRING	m_ClassName;
	zSTRING	m_BaseClassName;
	zSTRING	m_ScriptClassName;
	zCClassDef* m_BaseClassDef;

	// There is more data here
};