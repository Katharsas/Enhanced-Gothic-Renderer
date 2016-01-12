#include "pch.h"
#include "SmartHeap.h"
#include "detours.h"

//#define UNPATCH_SMARTHEAP


void ReplaceCodeBytes(const char* bytes, int numBytes, unsigned int addr)
{
	// Unprotect the given code-area
	DWORD dwProtect;
	VirtualProtect((void *)addr, numBytes, PAGE_EXECUTE_READWRITE, &dwProtect);

	// Overwrite the code
	memcpy((void*)addr, bytes, numBytes);
}

void* _new(unsigned int size)
{
	return malloc(size);
}

void _delete(void* ptr)
{
	free(ptr);
}

void* _realloc(void* ptr, unsigned int size)
{
	return realloc(ptr, size);
}

void* _malloc(unsigned int size)
{
	return malloc(size);
}

void* _calloc(unsigned int count, unsigned int size)
{
	return calloc(count, size);
}

void _free(void* ptr)
{
	free(ptr);
}

unsigned int __msize(void* ptr)
{
	return _msize(ptr);
}

/** Patches all mallocs and news to use the normal CRT */
void UnpatchSmartHeap()
{
#ifdef UNPATCH_SMARTHEAP
	HMODULE sh = GetModuleHandle("SHW32.dll");
	DetourFunction((PBYTE)GetProcAddress(sh, "shi_new"), (PBYTE)_new);
	DetourFunction((PBYTE)GetProcAddress(sh, "shi_delete"), (PBYTE)_delete);
	DetourFunction((PBYTE)GetProcAddress(sh, "shi_malloc"), (PBYTE)_malloc);
	DetourFunction((PBYTE)GetProcAddress(sh, "shi_realloc"), (PBYTE)_realloc);
	DetourFunction((PBYTE)GetProcAddress(sh, "shi_calloc"), (PBYTE)_calloc);
	DetourFunction((PBYTE)GetProcAddress(sh, "shi_free"), (PBYTE)_free);
	DetourFunction((PBYTE)GetProcAddress(sh, "shi_msize"), (PBYTE)__msize);
#endif
}