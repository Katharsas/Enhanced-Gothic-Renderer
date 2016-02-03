#pragma once
#include "rbaseviewport.h"

#ifdef RND_D3D11
#include "RD3D11Viewport.h"
#define RVIEWPORTBASE_API RD3D11Viewport
#endif

class RViewport :
	public RVIEWPORTBASE_API
{
public:
	RViewport(void);
	~RViewport(void);

	/**
	 * Creates this viewport resources
	 */
	bool CreateViewport(const ViewportInfo& viewport);
};

