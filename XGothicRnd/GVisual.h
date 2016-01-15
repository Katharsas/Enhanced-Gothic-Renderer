#pragma once
#include "GzObjectExtension.h"
#include "GConstants.h"

// Number of LOD-Levels a vob should have
const int NUM_VISUAL_LOD_LEVELS = 4;

/**
 * In our approach visuals only hold the data needed to render. The drawables contain the renderlogic
 * so we don't have to use virtual functions and pointers in our inner rendering-loop
 */

class GBaseDrawable;
class zCVisual;
class GVobObject;
class GVisual : public GzObjectExtension<zCVisual, GVisual>
{
public:
	struct StateCache
	{
		std::vector<RPipelineState*> PipelineStates;
	};

	/** Creates an extension-object of the right visual type if none was created before */
	static GVisual* CreateExtensionVisual(zCVisual* visual);

	GVisual(zCVisual* sourceObject);
	virtual ~GVisual(void);

	/** Tells all drawables they should re-get their cached pipeline states */
	void InvalidateStateCaches();

	/** Registers a drawable */
	void RegisterDrawable(GBaseDrawable* drawable);

	/** Unregisters a drawable */
	void UnregisterDrawable(GBaseDrawable* drawable);

	/** Updates the statecache of the given drawable. If the drawable wasn't registered in
		the statecache before, will create a new cache entry and return a new pointer to the
		drawables state-cache. Otherwise the old pointer will be returned. */
	virtual StateCache* UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage){return nullptr;}

	/** Creates a drawable for this visual */
	virtual void CreateDrawables(std::vector<GBaseDrawable*>& v, int lodLevel) {};

	/** Called when a drawable got drawn. Only if the InformVisual-Flag is set on it, though! */
	virtual void OnDrawableDrawn(GBaseDrawable* drawable) {};

	/** Returns the name of the file this was created from */
	const std::string& GetFileName() { return m_FileName; }

	/** Adds a vob using this visual (directly and indirectly).
		Returns true if not already in the list. */
	virtual bool AddVob(GVobObject* vob);

	/** Removes a vob using this visual (directly and indirectly).
		Returns true, if found. */
	virtual bool RemoveVob(GVobObject* vob);

protected:
	/** Destroys the pipelinestates of the given drawable */
	void DestroyDrawableStates(GBaseDrawable* drawable);

	// Set of drawables using this visual
	std::set<GBaseDrawable*> m_RegisteredDrawables;

	// All pipeline-states created by this visual, ordered by the drawable which wanted them
	// Memory will be freed when the drawable gets unregistered
	std::unordered_map<GBaseDrawable*, StateCache> m_CreatedPipelineStates;

	// File name of this visual, so I can see this in debugger
	std::string m_FileName;

	// Set of vobs using this visual
	std::set<GVobObject*> m_Vobs;

	// Size of the bounding-box of this visual
	float m_VisualSize;
};

