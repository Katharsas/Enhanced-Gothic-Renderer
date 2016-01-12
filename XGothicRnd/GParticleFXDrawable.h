#pragma once
#include "GBaseDrawable.h"
class GParticleFXDrawable :
	public GBaseDrawable
{
public:
	GParticleFXDrawable(GVisual* sourceVisual);
	~GParticleFXDrawable();

	/** Called when a cached state got reaquired from the visual. Use this
	method to fill in custom values like constant-buffers */
	virtual void OnReaquiredState(GConstants::ERenderStage stage, RPipelineState* state);
};

