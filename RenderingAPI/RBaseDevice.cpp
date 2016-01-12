#include "pch.h"
#include "RBaseDevice.h"


RBaseDevice::RBaseDevice()
{
	FrameCounter = 0;
	QueuedDrawCallCounter = 0;
	QueueCounter = 0;

	SetMainClearValues(float4(0, 0, 0, 0), 1.0f);
}


RBaseDevice::~RBaseDevice()
{
}

/**
* Returns the resolution needed for the given window
*/
INT2 RBaseDevice::GetWindowResolution(HWND hWnd)
{
	RECT r; 
	GetClientRect(OutputWindow, &r);

	return INT2(r.right - r.left, r.bottom - r.top);
}
