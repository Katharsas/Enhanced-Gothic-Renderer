#pragma once
#include "zSTRING.h"

class zCViewProgressBar
{
	/** Sets the current percentage of this progressbar */
	void SetPercent(int percent, class zSTRING text = zSTRING(""))
	{
		XCALL(MemoryLocations::Gothic::zCViewProgressBar__SetPercent_int_zSTRING);
	}
};