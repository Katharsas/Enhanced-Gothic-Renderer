#pragma once
#include "zCVob.h"

class oCNpc : public zCVob
{
public:
	/** Returns true if this NPC is a player */
	zBOOL IsAPlayer()
	{
		XCALL(MemoryLocations::Gothic::oCNpc__IsAPlayer_void);
	}
};
