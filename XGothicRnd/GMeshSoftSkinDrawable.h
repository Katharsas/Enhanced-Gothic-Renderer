#pragma once
#include "GBaseDrawable.h"
class GMeshSoftSkinDrawable :
	public GBaseDrawable
{
public:
	GMeshSoftSkinDrawable(GVisual* sourceVisual);
	~GMeshSoftSkinDrawable();

	/** Called when a cached state got reaquired from the visual. Use this
	method to fill in custom values like constant-buffers */
	virtual void OnReaquiredState(GConstants::ERenderStage stage,RAPI::RPipelineState* state);

	/** Sets the constantbuffer containing the current bone-matrices */
	void SetBoneMatricesBuffer(RAPI::RBuffer* buffer);

protected:
	// Currently used constantbuffer for the bone-matrices
	RAPI::RBuffer* m_BoneMatriesBuffer;
};

