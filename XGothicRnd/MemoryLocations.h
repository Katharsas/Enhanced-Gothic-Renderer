#pragma once

#define VERSION_1_8K_MOD 181
#define VERSION_1_12F 112
#define VERSION_2_6_FIX 261

#ifdef BUILD_1_8K_MOD
#define GAME_VERSION VERSION_1_8K_MOD
#define DATASET_VERSION VERSION_1_8K_MOD
#elif BUILD_1_12F
#define GAME_VERSION VERSION_1_12F
#define DATASET_VERSION VERSION_1_8K_MOD
#else
#define GAME_VERSION VERSION_2_6_FIX
#define DATASET_VERSION VERSION_2_6_FIX
#endif

#if GAME_VERSION == VERSION_2_6_FIX
#include "MemoryLocations2_6fix.h"
#include "MemoryOffsets_2_6fix.h"
#elif GAME_VERSION == VERSION_1_8K_MOD
#include "MemoryLocations1_8kmod.h"
#include "MemoryOffsets_1_8mod.h"
#elif GAME_VERSION == VERSION_1_12F
#include "MemoryLocations1_12f.h"
#include "MemoryOffsets1_12f.h"
#endif
