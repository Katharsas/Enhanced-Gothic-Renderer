#include "pch.h"
#include "GParticleFXDrawable.h"


GParticleFXDrawable::GParticleFXDrawable(GVisual* sourceVisual) : GBaseDrawable(sourceVisual, false)
{
	m_InformVisual = true;
}


GParticleFXDrawable::~GParticleFXDrawable()
{
}

/** Called when a cached state got reaquired from the visual. Use this
method to fill in custom values like constant-buffers */
void GParticleFXDrawable::OnReaquiredState(GConstants::ERenderStage stage, RPipelineState* state)
{

}