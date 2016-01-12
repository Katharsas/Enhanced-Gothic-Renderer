#include "pch.h"
#include "REngine.h"
#include "RDevice.h"
#include "RResourceCache.h"
#include "RDynamicBufferCache.h"
#include "RThreadPool.h"
#include <thread>

/**
 * Initializes these objects 
 */
bool REngine::InitializeEngine()
{
	REngine::RenderingDevice = new RDevice();
	REngine::ResourceCache = new RResourceCache();
	REngine::DynamicBufferCache = new RDynamicBufferCache();
	REngine::ThreadPool = new RThreadPool(std::thread::hardware_concurrency());
	return true;
}

/**
 * Destroys these objects
 */
void REngine::UninitializeEngine()
{
	delete REngine::ResourceCache;
	delete REngine::RenderingDevice;
	delete REngine::DynamicBufferCache;
}