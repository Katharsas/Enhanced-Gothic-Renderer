#pragma once
#include "pch.h"

#ifdef RND_D3D11
#include "RD3D11CommandList.h"
#define RCOMMANDLIST_API RD3D11CommandList
#endif

class RCommandList :
	public RCOMMANDLIST_API
{
public:
	RCommandList();
	~RCommandList();

	/** Initializes a commandlist for the given Thread ID */
	bool Init();

	/** Creates the commandlist and makes it ready to be played back. 
		This must be called from an other thread than the main-thread! */
	bool FinalizeCommandList();

	/** Plays the generated commandlist back on the main-thread */
	bool ExecuteCommandList();
};

