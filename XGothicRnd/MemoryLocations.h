#pragma once

#define VERSION_1_8K_MOD 181
#define VERSION_2_6_FIX 261

#define GAME_VERSION VERSION_2_6_FIX

#if GAME_VERSION == VERSION_2_6_FIX
#include "MemoryLocations2_6fix.h"
#include "MemoryOffsets_2_6fix.h"
#elif GAME_VERSION == VERSION_1_8K_MOD
#include "MemoryLocations1_8kmod.h
#endif
