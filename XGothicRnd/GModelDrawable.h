#pragma once
#include "GBaseDrawable.h"

// This is a little dummy-state, since zCModels are only containers for
// zCMeshSoftSkin and node-attachment visuals
// This drawable will only inform the zCModel that the underlaying vob has been
// queued for rendering, so it can update the model
class GModelDrawable :
	public GBaseDrawable
{
public:
	GModelDrawable(GVisual* sourceVisual);
	~GModelDrawable();

	/** Called when a cached state got reaquired from the visual. Use this
	method to fill in custom values like constant-buffers */
	virtual void OnReaquiredState(GConstants::ERenderStage stage, RPipelineState* state) {}
};

