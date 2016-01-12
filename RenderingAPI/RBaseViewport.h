#pragma once
#include "pch.h"
#include "RResource.h"

class RBaseViewport : public RResource
{
public:
	RBaseViewport(void);
	virtual ~RBaseViewport(void);

	/**
	 * Getters
	 */
	const ViewportInfo& GetViewportInfo(){return Viewport;}

protected:
	// Viewport-data
	ViewportInfo Viewport;
};

