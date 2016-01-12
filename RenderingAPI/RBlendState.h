#pragma once

#ifdef RND_D3D11
#include "RD3D11BlendState.h"
#define RBLENDBASE_API RD3D11BlendState
#endif

class RBlendState : public RBLENDBASE_API
{
public:
	RBlendState(void);
	~RBlendState(void);

	/**
	 * Creates the blendstate
	 */
	bool CreateState(const RBlendStateInfo& info);
};

