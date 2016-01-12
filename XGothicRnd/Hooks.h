#pragma once

namespace Hooks
{
	/**
	 * Hooks a function of the given type. Does the same as HookDirect, but
	 * should give cleaner code.
	 */
	template<typename T> T HookFunction(const void* address, const void* hook)
	{
		return (T)HookDirect(address, hook);
	}

	/**
	 * Direct hooking-function. Works by passing only memory adresses.
	 */
	void* HookDirect(const void* address, const void* hook);
};