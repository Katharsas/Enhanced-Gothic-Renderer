#include "pch.h"
#include "RResourceCache.h"
#include "../Shared/Toolbox.h"

RResourceCache::RResourceCache(void)
{
}


RResourceCache::~RResourceCache(void)
{
	for(auto it = RegisteredCaches.begin(); it != RegisteredCaches.end(); it++)
	{
		std::vector<RResource*>& v = (*it)->Objects;
		Toolbox::DeleteElements(v);
	}
}
