#pragma once
#include "pch.h"
#include "zEngineFunctions.h"

class zSTRING
{
public:

	

	/**
	 * Constructs a new string from a c-str
	 */
	zSTRING(const char* str)
	{
		XCALL(MemoryLocations::Gothic::zSTRING__zSTRING_char_const_p);
	}

	/**
	 * Converts this zSTRING to a usual char-array
	 */
	const char* ToChar() const
	{
		XCALL(MemoryLocations::Gothic::zSTRING__ToChar_void);
	}

private:
	char data1[8];
	char* m_String;
	char data2[8];
};

static_assert(sizeof(zSTRING) == 20, "zSTRING has to be 20bytes!");