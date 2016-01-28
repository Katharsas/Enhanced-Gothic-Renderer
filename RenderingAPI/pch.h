#pragma once

#define RND_D3D11

#ifdef RND_D3D11
// Disable warning about makro-redifinitions
// There are a lot of these about D3D11 and DXGI-Errorcodes which
// have been redefined in other files than those in the DX-SDK
#pragma warning( disable : 4005 )
#include <D3D11.h>
#include <DXGI.h>
#include <D3DX11.h>
#include <D3DX11tex.h>
#pragma warning( default : 4005 )
#endif

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include <vector>
#include <unordered_map>
#include <map>
#include <mutex>
#include <set>
#include <thread>
#include <future>
#include <numeric>
#include "../Shared/Types.h"
#include "AntTweakBar.h"

#define R_PI 3.14159265358979f

#define SafeRelease(x) do{if(x){x->Release(); x = NULL;}}while(0)