#pragma once

#ifdef RND_D3D11
#include "RD3D11DepthStencilState.h"
#define RDEPTHSTENCILBASE_API RD3D11DepthStencilState
#endif

class RDepthStencilState :
	public RDEPTHSTENCILBASE_API
{
public:
	RDepthStencilState(void);
	~RDepthStencilState(void);

	/**
	 * Initializes the state
	 */
	bool CreateState(const RDepthStencilStateInfo& info);

private:

};

