#pragma once

#ifdef RND_D3D11
#include "RD3D11RasterizerState.h"
#define RRASTERIZERBASE_API RD3D11RasterizerState
#endif

class RRasterizerState : public RRASTERIZERBASE_API
{
public:
	RRasterizerState(void);
	~RRasterizerState(void);

	/**
	 * Creates the blendstate
	 */
	bool CreateState(const RRasterizerStateInfo& info);
};

