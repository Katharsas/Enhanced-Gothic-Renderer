#pragma once

class IObserver
{
public:

	/**
	 * Called when the observer got notified
	 */
	virtual void OnNotified(size_t id, void* userData) = 0;
};