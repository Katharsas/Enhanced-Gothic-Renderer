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
	virtual void OnReaquiredState(GConstants::ERenderStage stage, RPipelineState* state);

	/** Sets the constantbuffer containing the current bone-matrices */
	void SetBoneMatricesBuffer(RBuffer* buffer);

protected:
	// Currently used constantbuffer for the bone-matrices
	RBuffer* m_BoneMatriesBuffer;
};

