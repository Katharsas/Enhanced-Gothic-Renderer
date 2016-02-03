#include "pch.h"
#include "RDepthStencilState.h"


RDepthStencilState::RDepthStencilState(void)
{
}


RDepthStencilState::~RDepthStencilState(void)
{
}

/**
 * Initializes the state
 */
bool RDepthStencilState::CreateState(const RDepthStencilStateInfo& info)
{
	StateInfo = info;
	return CreateStateAPI();
}