#include "pch.h"
#include "GStaticMeshDrawable.h"


GStaticMeshDrawable::GStaticMeshDrawable(GVisual* sourceVisual) : GBaseDrawable(sourceVisual, false)
{
	m_SupportInstancing = true;
}


GStaticMeshDrawable::~GStaticMeshDrawable(void)
{
}

/** Called when a cached state got reaquired from the visual. Use this
	method to fill in custom values like constant-buffers */
void GStaticMeshDrawable::OnReaquiredState(GConstants::ERenderStage stage, RPipelineState* state)
{

}