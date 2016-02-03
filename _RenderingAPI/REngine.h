#pragma once
#include "RDevice.h"

/**
* Global namespace of REngine-Objects
*/
class RDynamicBufferCache;
class RResourceCache;
class RDevice;
class RThreadPool;
namespace REngine
{
	/**
	 * Initializes these objects 
	 */
	bool InitializeEngine();

	/**
	 * Destroys these objects
	 */
	void UninitializeEngine();

	__declspec(selectany) RDevice* RenderingDevice;
	__declspec(selectany) RResourceCache* ResourceCache;
	__declspec(selectany) RDynamicBufferCache* DynamicBufferCache;
	__declspec(selectany) RThreadPool* ThreadPool;
}