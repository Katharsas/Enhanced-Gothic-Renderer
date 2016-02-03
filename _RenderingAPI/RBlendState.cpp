#include "pch.h"
#include "RBlendState.h"


RBlendState::RBlendState(void)
{
}


RBlendState::~RBlendState(void)
{
}


/**
 * Creates the blendstate
 */
bool RBlendState::CreateState(const RBlendStateInfo& info)
{
	StateInfo = info;
	return CreateStateAPI();
}