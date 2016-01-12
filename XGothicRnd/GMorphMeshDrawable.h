#pragma once
#include "GBaseDrawable.h"
class GMorphMeshDrawable :
	public GBaseDrawable
{
public:
	GMorphMeshDrawable(GVisual* sourceVisual);
	~GMorphMeshDrawable();

	/** Called when a cached state got reaquired from the visual. Use this
	method to fill in custom values like constant-buffers */
	virtual void OnReaquiredState(GConstants::ERenderStage stage, RPipelineState* state);
};

