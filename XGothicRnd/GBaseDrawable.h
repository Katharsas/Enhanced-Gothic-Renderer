#pragma once
#include "GConstants.h"
#include "GVisual.h"

/**
 * We exactly know what kinds of visuals Gothic has, so we don't need to be so flexible
 * and implement them using pointers in vobs and virtual functions.
 * Idea: Make each GVobObject inherit from one drawable-type, representing a visual.
 * These can be fed from the real visuals they represent. If a vob changes its visual,
 * destroy the GVobObject and create a new one using the fitting drawable
 */

struct VobInstanceInfo
{
	// Vobs don't need scale, so only pass position and rotation as quaternion
	float3 m_Position;
	DWORD m_InstanceColor;

	Quaternion m_Rotation;
};

class GBaseDrawable;
struct VobInstanceInfo;
struct RenderInstance
{
	RenderInstance() {}
	RenderInstance(uint32_t key, const VobInstanceInfo* instance, GBaseDrawable* drawable, GVobObject* sourceVob)
		: m_SortKey(key), m_Instance(instance), m_Drawable(drawable), m_SourceVob(sourceVob)
	{}

	uint32_t m_SortKey;
	const VobInstanceInfo* m_Instance;
	GBaseDrawable* m_Drawable;
	GVobObject* m_SourceVob;
};

struct RPipelineState;
class GVisual;
class GBaseDrawable
{
public:

	GBaseDrawable(GVisual* sourceVisual, bool informVisual=false, bool drawAlways=false);
	virtual ~GBaseDrawable(void);

	/** Queries the visual for new pipeline states */
	void ReaquireStateCache(GConstants::ERenderStage stage = GConstants::RS_UNDEFINED);

	/** Called when a cached state got reaquired from the visual. Use this
		method to fill in custom values like constant-buffers */
	virtual void OnReaquiredState(GConstants::ERenderStage stage, RPipelineState* state) = 0;

	/** Pushes the cached pipeline-states to the renderer */
	void PushRenderStateCache(GConstants::ERenderStage stage, RRenderQueueID queue, unsigned int instanceIndex, unsigned int numInstances);

	/** Returns the visual this was created from */
	GVisual* GetVisual()const{return m_SourceVisual;}

	/** Sets a new instance info for this drawable */
	void SetInstanceInfo(const VobInstanceInfo& instance);

	/** Returns the current instance info for this drawable */
	const VobInstanceInfo& GetInstanceInfo() { return m_InstanceInfo; };

	/** Wheather this drawable supports instancing or not */
	bool SupportsInstancing() { return m_SupportInstancing; }

	/** Checks if this vob even has states for the current pass */
	bool HasStatesForStage(GConstants::ERenderStage stage) { return m_DrawAlways || m_StateCaches[stage] != nullptr; }

	/** Called when drawn */
	inline void OnDrawn()
	{
		if(m_InformVisual)
			m_SourceVisual->OnDrawableDrawn(this);
	}

	/** If true, this drawable should inform it's visial that it has been drawn */
	bool ShouldInformVisual() { return m_InformVisual; }

	/** Sets the vob we are currently attached to */
	void SetHomeVob(GVobObject* vob){m_HomeVob = vob;}
protected:

	// Visual we got the data from
	GVisual* m_SourceVisual;

	// Vob we are attached to
	GVobObject* m_HomeVob;

	// If true, we will inform the visual about drawcalls
	bool m_InformVisual;

	// If true, this will always be drawn, even if there are no states in the cache for the current stage
	bool m_DrawAlways;

	// Current instance-info for this drawable
	VobInstanceInfo m_InstanceInfo;

	// If true, this object can render multiple instances in one batch
	bool m_SupportInstancing;

	// State caches ordered by their renderstage
	GVisual::StateCache* m_StateCaches[GConstants::ERenderStage::RS_NUM_STAGES];
};

