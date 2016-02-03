#pragma once
#include <set>
#include "IObserver.h"

class RObservable
{
public:

	/**
	 * Registers an observer
	 */
	void RegisterObserver(IObserver* observer)
	{
		Observers.insert(observer);
	}

	/**
	 * Unregisters an observer
	 */
	void RemoveObserver(IObserver* observer)
	{
		Observers.erase(observer);
	}

	/**
	 * Notifies all registered observers, broadcasting the given data
	 */
	void NotifyAll(size_t id, void* userdata)
	{
		for(IObserver* obs : Observers)
		{
			obs->OnNotified(id, userdata);
		}
	}

private:
	std::set<IObserver*> Observers;
};