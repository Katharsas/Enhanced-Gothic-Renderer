#include "pch.h"
#include "RViewport.h"


RViewport::RViewport(void)
{
}


RViewport::~RViewport(void)
{
}

/**
 * Creates this viewport resources
 */
bool RViewport::CreateViewport(const ViewportInfo& viewport)
{
	Viewport = viewport;

	return CreateViewportAPI();
}