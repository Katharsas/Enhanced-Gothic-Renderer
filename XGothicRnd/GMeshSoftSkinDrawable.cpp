#include "pch.h"
#include "GMeshSoftSkinDrawable.h"


GMeshSoftSkinDrawable::GMeshSoftSkinDrawable(GVisual* sourceVisual) : GBaseDrawable(sourceVisual, false)
{
	// Need different bone-buffers per instance
	m_SupportInstancing = false;
}


GMeshSoftSkinDrawable::~GMeshSoftSkinDrawable()
{

}

/** Called when a cached state got reaquired from the visual. Use this
method to fill in custom values like constant-buffers */
void GMeshSoftSkinDrawable::OnReaquiredState(GConstants::ERenderStage stage, RPipelineState* state)
{
	// Apply the bone-matrices buffer to all states
	if(state->ConstantBuffers[EShaderType::ST_VERTEX].size() < 2)
		state->ConstantBuffers[EShaderType::ST_VERTEX].resize(2);

	state->ConstantBuffers[EShaderType::ST_VERTEX][1] = m_BoneMatriesBuffer;
}

/** Sets the constantbuffer containing the current bone-matrices */
void GMeshSoftSkinDrawable::SetBoneMatricesBuffer(RBuffer* buffer)
{
	m_BoneMatriesBuffer = buffer;

	// Update states
	ReaquireStateCache();
}