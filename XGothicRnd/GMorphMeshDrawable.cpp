#include "pch.h"
#include "GMorphMeshDrawable.h"


GMorphMeshDrawable::GMorphMeshDrawable(GVisual* sourceVisual) : GBaseDrawable(sourceVisual, false)
{
	m_InformVisual = true;
}


GMorphMeshDrawable::~GMorphMeshDrawable()
{
}

/** Called when a cached state got reaquired from the visual. Use this
method to fill in custom values like constant-buffers */
void GMorphMeshDrawable::OnReaquiredState(GConstants::ERenderStage stage,RAPI::RPipelineState* state)
{

}