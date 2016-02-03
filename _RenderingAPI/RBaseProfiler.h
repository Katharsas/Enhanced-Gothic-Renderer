#pragma once
#include "pch.h"

class RBaseProfiler
{
public:
	RBaseProfiler();
	~RBaseProfiler();

	// Constants
	static const UINT64 QueryLatency = 5;

protected:
	UINT64 currFrame;
};

