#include "pch.h"
#include <unordered_map>
#include "Engine.h"
#include <mutex>
#include "../Shared/Logger.h"

std::unordered_map<DWORD, size_t> AllocatedMemory;
bool inCheck = false;
std::mutex m;

void AddMemRange(void* start, size_t size)
{
	m.lock();
	DWORD dw = (DWORD)start;
	AllocatedMemory[dw] = size;

	m.unlock();
}

void RemoveMemRange(void* start)
{
	m.lock();
	DWORD dw = (DWORD)start;
	auto it = AllocatedMemory.find(dw);

	if(it == AllocatedMemory.end())
	{
		LogError() << "HEAP CORRUPTION DETECTED AT 0x" << start;
		return;
	}

	AllocatedMemory.erase(it);

	size_t size = (*it).second;

	for(DWORD i = dw+1; i < dw + size; i++)
	{
		size_t s = AllocatedMemory.size();
		AllocatedMemory.erase(i);

		if(AllocatedMemory.size() != s)
		{
			LogError() << "HEAP CORRUPTION DETECTED AT 0x" << std::hex << dw+i << " WHILE DELETING 0x" << start;
		}
	}
	m.unlock();
}

void* operator new (size_t size)
{
	void* ptr = malloc(size);

	memset(ptr, 0xFF, size);

	/*if(!inCheck && Engine::Game)
	{
		inCheck = true;
		AddMemRange(ptr, size);
		inCheck = false;
	}*/
	return (void*)ptr;
}

void operator delete(void *ptr)
{
	if(!ptr)
		return;

	/*if(!inCheck && Engine::Game)
	{
		inCheck = true;
		RemoveMemRange(ptr);
		inCheck = false;
	}*/

	memset(ptr, 0xCD, _msize(ptr));

	free(ptr);
}

void* operator new[] (size_t size)
{
	void* ptr = malloc(size);

	memset(ptr, 0xFF, size);

	/*if(!inCheck && Engine::Game)
	{
		inCheck = true;
		AddMemRange(ptr, size);
		inCheck = false;
	}*/

	return (void*)ptr;
}

void operator delete[] (void *ptr)
{
	if(!ptr)
		return;

	/*if(!inCheck && Engine::Game)
	{
		inCheck = true;
		RemoveMemRange(ptr);
		inCheck = false;
	}*/

	memset(ptr, 0xCD, _msize(ptr));

	free(ptr);
}