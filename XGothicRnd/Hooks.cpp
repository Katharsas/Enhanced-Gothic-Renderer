#include "pch.h"
#include "Hooks.h"
#include "detours.h"

namespace Hooks
{
	/**
	 * Direct hooking-function. Works by passing only memory adresses.
	 */
	void* HookDirect(const void* address, const void* hook)
	{
		return DetourFunction((PBYTE)address, (PBYTE)hook);
	}
};