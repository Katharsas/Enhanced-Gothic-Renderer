#pragma once
#include "zEngineFunctions.h"
#include "MemoryLocations.h"
#include "zCObject.h"

class zCWorld : public zCObject
{
public:

	/** Returns the BSP-Tree of this world */
	class zCBspTree* GetBspTree()
	{
		return (zCBspTree *)THISPTR_OFFSET(MemoryOffsets::Gothic::zCWorld::BSPTree);
	}

	// TODO: Broken, since the data-offest isn't right!
	/** The game creates a new world for each of the inventory cells and places vobs inside
		them for each one. This flag indicates if this world is being used for the inventory. */
	bool IsInventoryWorld()
	{
		return m_IsInventoryWorld == TRUE;
	}
private:

	byte data[40]; // TODO: This still isn't right! 

	class zCSession* m_OwnerSession;
	class zCCSPlayer* m_CsPlayer;
	zSTRING	m_StrlevelName;
	zBOOL m_Compiled;
	zBOOL m_CompiledEditorMode;
	zBOOL m_TraceRayIgnoreVobFlag;
	zBOOL m_IsInventoryWorld;
};

