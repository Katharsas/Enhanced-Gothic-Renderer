#pragma once
#include "rbaseviewport.h"
class RD3D11Viewport :
	public RBaseViewport
{
public:
	RD3D11Viewport(void);
	~RD3D11Viewport(void);

	/**
	 * Creates this viewport resources
	 */
	bool CreateViewportAPI();
};

