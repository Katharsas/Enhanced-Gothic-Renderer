#pragma once
#include "gbasedrawable.h"
class GStaticMeshDrawable :
	public GBaseDrawable
{
public:
	GStaticMeshDrawable(GVisual* sourceVisual);
	~GStaticMeshDrawable(void);

	/** Called when a cached state got reaquired from the visual. Use this
		method to fill in custom values like constant-buffers */
	virtual void OnReaquiredState(GConstants::ERenderStage stage, RPipelineState* state);
};

