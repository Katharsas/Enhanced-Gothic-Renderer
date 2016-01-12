#pragma once
#include <Windows.h>

#include <vector>
#include <unordered_map>
#include <map>
#include "../Shared/SimpleMath.h"
#include "../Shared/Logger.h"
#include "MemoryLocations.h"
#include <RDevice.h>

#pragma comment(lib, "RenderingAPI.lib")

#define _USE_MATH_DEFINES
#include <math.h>

#pragma warning(disable: 4100) // Unreferenced formal parameter
#pragma warning(disable: 4201) // Nameless structs
#pragma warning(disable: 4505) // Removed unreferenced function

void* operator new (size_t size);
void operator delete(void *ptr);
void* operator new[] (size_t size);
void operator delete[] (void *ptr);