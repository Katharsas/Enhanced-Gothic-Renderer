#pragma once
#include "GVisual.h"
class GParticleFXVisual :
	public GVisual
{
public:
	GParticleFXVisual(zCVisual* sourceVisual);
	~GParticleFXVisual();

	/** Updates the statecache of the given drawable. If the drawable wasn't registered in
	the statecache before, will create a new cache entry and return a new pointer to the
	drawables state-cache. Otherwise the old pointer will be returned. */
	virtual StateCache* UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage);

	/** Called when a drawable got drawn. Only if the InformVisual-Flag is set on it, though! */
	virtual void OnDrawableDrawn(GBaseDrawable* drawable);

	/** Creates a drawable for this visual */
	virtual void CreateDrawables(std::vector<GBaseDrawable*>& v);

private:
	/** Updates the underlaying particle effect to get it ready for rendering */
	void UpdateEffect();

	// Last frame this was updated
	unsigned int m_LastFrameUpdated;

	// Position and other attributes of the particles
	RBuffer* m_ParticleInfoBuffer;

	// Pipelinestate of this visual
	RPipelineState* m_PFXPipelineState;

};

