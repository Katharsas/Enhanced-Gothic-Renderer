#pragma once
#include "pch.h"

#ifdef RND_D3D11
#include "RD3D11Device.h"
#define RDEVICEBASE_API RD3D11Device
#endif

// Whether to profile the rendering queues. This will store the queues timing in a map per name.
// If no name is present, the queue won't be profiled. If enabled, multithreading will not be used, if enabled.
//#define R_PROFILE_QUEUES

// Whether to use multithreading to draw the queues
#define NO_MULTITHREADED_RENDERING

class RDevice :
	public RDEVICEBASE_API
{
public:
	RDevice();
	~RDevice();

	/**
	 * Creates the renderingdevice for the set API 
	 */
	bool CreateDevice();

	/**
	 * Sets the output window for this device
	 */
	bool SetWindow(HWND hWnd);

	/**
	 * Registers a thread in the renderer. Creates a resources like a deferred context.
	 */
	bool RegisterThread(UINT threadID);

	/**
	* Creates the commandlist for the given threadID
	*/
	bool CreateCommandListForThread(UINT threadID);

	/**
	* Called on frame-start 
	*/
	bool OnFrameStart();

	/**
	 * Called on frame-end 
	 */
	bool OnFrameEnd();

	/**
	 * Presents the backbuffer on screen
	 */
	bool Present();

	/**
	 * Renders the given pipeline-state
	 */
	bool DrawPipelineState(const struct RPipelineState& state);
	bool DrawPipelineState(const struct RPipelineState& state, const RStateMachine::ChangesStruct& changes, RStateMachine& stateMachine);

	/**
	 * Renders an array of pipeline-states
	 */
	bool DrawPipelineStates(struct RPipelineState*const* stateArray, unsigned int numStates);

	/**
	 * Puts the given pipeline-state into the renderingqueue, which is flushed at the end of the frame
	 */
	bool QueuePipelineState(const struct RPipelineState* state, RRenderQueueID queue);

	/**
	 * Renders everything in the renderqueue
	 */
	bool FlushRenderQueue(RRenderQueueID queue);

	/** 
	 * Returns a list of available display modes 
	 */
	bool GetDisplayModeList(std::vector<DisplayModeInfo>& modeList, bool includeSuperSampling=false);

	/**
	 * Registers a renderingqueue in the device. Registration will be cleared every frame, so you have to
	 * get one every frame you want to use it.  */
	RRenderQueueID AcquireRenderQueue(bool sortable = false, const std::string& name = "");

	/** 
	 * Returns the Counter of how many frames since the start of the program have been rendered 
	 */
	unsigned int GetFrameCounter();

	/** 
	 * Fills the "changes"-vector of the given queue with values
	 */
	void ProcessRenderQueue(RRenderQueueID queue);

	/**
	 * Returns the number of queues in use
	 */
	unsigned int GetNumQueuesInUse();

	/**
	 * Returns the total number of pipeline-states registered in the queues 
	 */
	unsigned int GetNumRegisteredDrawCalls();

	/**
	 * Flushes actions from the given thread-ID
	 */

	/**
	 * Returns the current main output window
	 */
	HWND GetOutputWindow();

	/**
	 * Returns a list of all result the profiler has gathered 
	 */
	std::vector<std::pair<std::string, RProfiler::RProfileResult>> GetProfilerResults();

#ifndef PUBLIC_RELEASE
	/**
	 * DEBUG ONLY, NOT FOR PRODUCTION USE
	 */
	RRenderQueue* __GetRenderQueueByID(RRenderQueueID id){return RenderQueue[id]; }
#endif
protected:

	/** Initializes the tweak-bar with values */
	void AddTweakBarValues();

	/**
	 * Prepares the renderqueues for rendering. Does the actual drawcalls.
	 */
	bool PrepareCommandlists(RRenderQueueID queue);

	/**
	 * Draws the whole given queue on the main thread
	 */
	bool FlushQueueImmediate(RRenderQueueID queue);

	/**
	 * Uses the generated commandlists of the given queue and draws them
	 */
	bool FlushQueueCmdLists(RRenderQueueID queue);
};

