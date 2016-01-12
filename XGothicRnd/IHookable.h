#pragma once
#include "Hooks.h"

// Constructs one of these objects at startup. The IHookable-Class then registers 
// the object in the hook-manager.
#define REGISTER_HOOK(hkClass)\
			namespace _hkClasses{\
			volatile hkClass _##hkClass##_reg;}

// Returns the registered object of the given class
#define GET_HOOK(hkClass) _hkClasses::_##hkClass##_reg

class IHookable
{
public:

};