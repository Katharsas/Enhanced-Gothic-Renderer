#pragma once

#define HOOK_RENDERING

#define GASSERT(x, m) {if(!(x)){LogErrorBox() << "Assertion failed: " << #x << " | " << m;}}