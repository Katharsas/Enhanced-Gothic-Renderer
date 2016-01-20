#pragma once

#define HOOK_RENDERING

#ifndef PUBLIC_RELEASE
#define GASSERT(x, m) {if(!(x)){__debugbreak(); LogErrorBox() << "Assertion failed: " << #x << " | " << m;}}
#else
#define GASSERT(x, m) {if(!(x)){LogError() << "Assertion failed: " << #x << " | " << m;}}
#endif