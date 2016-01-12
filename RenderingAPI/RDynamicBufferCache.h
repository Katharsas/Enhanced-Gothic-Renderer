#pragma once
#include "pch.h"

// Numbers of frames should have buffers to prepare for
const unsigned int NUM_BUFFERCACHE_FRAME_STORAGES = 1;

class RBuffer;
class RDynamicBufferCache
{
public:
	RDynamicBufferCache(void);
	~RDynamicBufferCache(void);

	/** Request a dynamic buffer from the stash. This buffer will
		get invalid after the frame ended.
		Returns the current frame-number and the buffer. */
	std::pair<unsigned int, RBuffer*> GetDataBuffer(EBindFlags bindFlags, unsigned int size, unsigned int stride);

	/** Signals the cache that we're done with a buffer */
	void DoneWith(RBuffer* buffer, unsigned int bufferFrame, EBindFlags bindFlags);
	void DoneWith(std::pair<unsigned int, RBuffer*>& buffer);

	/** Called by the Device when the frame ended */
	void OnFrameEnded();

	/** Clears unused buffers from the cache */
	void ClearCache();
private:
	/** Inserts a new buffer into the current frames map */
	RBuffer* MakeBufferFor(std::unordered_multimap<unsigned int, RBuffer*>& map, EBindFlags bindFlags, unsigned int size, unsigned int stride);

	/** Marks all buffers in the given frame as free */
	void MarkFrameAsFree(unsigned int frame);

	struct FrameBufferStorage
	{
		// Map of buffers and their size. 
		std::unordered_map<EBindFlags, std::unordered_multimap<unsigned int, RBuffer*>> AllBuffers;
		std::unordered_map<EBindFlags, std::unordered_multimap<unsigned int, RBuffer*>> FreeBuffers;
		std::unordered_map<EBindFlags, std::unordered_multimap<unsigned int, RBuffer*>> DoneBuffers;
	};

	// Array of storages for a full frame so we can use new
	// buffers instead of relaying on the driver to rename them
	// or even waiting for the GPU to finish its frame before
	// putting new data into it.
	FrameBufferStorage FrameBuffers[NUM_BUFFERCACHE_FRAME_STORAGES];
	unsigned int Frame;

	std::set<RBuffer*> AllocatedBuffers;
};

