#pragma once
#include "zDefinitions.h"
#include "GzObjectExtension.h"

/**
 * World-Wrapper for the currently loaded ZEN of the game
 */

class GBspTree;
class zCWorld;
class GVobObject;
class zCVob;
class GWorld : public GzObjectExtension<zCWorld, GWorld>
{
public:
	GWorld(zCWorld* sourceObject);
	~GWorld(void);

	/**
	 * Draws the world from the current camerapperspective
	 */
	void Render();

	/** Registers a zCVob into the world */
	void AddVob(zCVob* vob);

	/** Removes a zCVob from the world */
	bool RemoveVob(zCVob* vob);

	/** Returns the number of vobs registered in here */
	unsigned int GetNumRegisteredVobs();

	/** 
	 * Called when the game finished loading the world
	 */
	void OnWorldLoaded(zTWorldLoadMode mode);

	/**
	 * Draws the games original sky 
	 */
	void DrawSkyPre();
	void DrawSkyPost();

private:
	/** Draws the vobs in this scene */
	void DrawVobs();

	// Our version of the games bsp-tree
	GBspTree* m_BspTree;

	// All vobs in this world
	std::set<GVobObject*> m_VobSet;

	// Currently rendered vobs
	std::vector<GVobObject*> m_VobRenderList;
};

