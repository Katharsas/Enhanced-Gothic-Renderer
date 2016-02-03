#pragma once

#ifdef RND_D3D11
#include "RD3D11SamplerState.h"
#define RSAMPLERBASE_API RD3D11SamplerState
#endif

class RSamplerState : public RSAMPLERBASE_API
{
public:
	RSamplerState(void);
	~RSamplerState(void);

	/**
	 * Creates the blendstate
	 */
	bool CreateState(const RSamplerStateInfo& info);
};

