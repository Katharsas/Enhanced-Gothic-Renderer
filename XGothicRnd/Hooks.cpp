#include "pch.h"
#include "Hooks.h"
#include "detours.h"

namespace Hooks
{
	struct _hkHelper
	{
		~_hkHelper()
		{
			// Remove all hooks when the program ends or we got unloaded
			for(auto& p : m_AppliedHooks)
			{
				DetourRemove(p.first, p.second);
			}
		}

		std::vector<std::pair<PBYTE, PBYTE>> m_AppliedHooks;
	}hkHelper;

	/**
	 * Direct hooking-function. Works by passing only memory adresses.
	 */
	void* HookDirect(const void* address, const void* hook)
	{
		PBYTE trampoline = DetourFunction((PBYTE)address, (PBYTE)hook);

		// Store the pointers, so we can remove the hook later
		if(trampoline)
			hkHelper.m_AppliedHooks.push_back(std::make_pair(trampoline, (PBYTE)hook));

		return trampoline;
	}
};