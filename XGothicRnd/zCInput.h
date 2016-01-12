#pragma once
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>
#include "pch.h"

class zCInput
{
public:

	/**
	 * Sets the inputstate of the games DInput-Device
	 */
	static void SetInputEnabled(bool enabled)
	{
		if (!GetInput())
			return;

		IDirectInputDevice7A* dInputMouse = *(IDirectInputDevice7A **)MemoryStaticObjects::Gothic::DInput::DInput7DeviceMouse;
		IDirectInputDevice7A* dInputKeyboard = *(IDirectInputDevice7A **)MemoryStaticObjects::Gothic::DInput::DInput7DeviceKeyboard;

		// Keep the game from changing these values
		GetInput()->SetDeviceEnabled(2, enabled ? 1 : 0);
		GetInput()->SetDeviceEnabled(1, enabled ? 1 : 0);

		if (dInputMouse)
		{
			if (!enabled)
				dInputMouse->Unacquire();
			else
				dInputMouse->Acquire();
		}

		if (dInputKeyboard)
		{
			if (!enabled)
				dInputKeyboard->Unacquire();
			else
				dInputKeyboard->Acquire();
		}
	}

	/**
	 * Enables/Disables a given input-device in the game.
	 * You still need to call (Un)Acquire if you want this to have an effect!
	 */
	void SetDeviceEnabled(int dev, zBOOL value)
	{
		XCALL(MemoryLocations::Gothic::zCInput_Win32__SetDeviceEnabled_zTInputDevice_int);
	}

	/**
	 * Returnst the global instance of zCInput
	 */
	static zCInput* GetInput()
	{
		return *(zCInput**)MemoryLocations::Gothic::zCInput_p_zinput;
	}
};