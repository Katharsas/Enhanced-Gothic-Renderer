#pragma once
#include "pch.h"

class RTweakBarBase
{
public:
	RTweakBarBase();
	virtual ~RTweakBarBase();

protected:

	// Helpers for allowing std::function as button-callbacks
	struct cbStruct
	{
		cbStruct(){}
		cbStruct(void* ud, std::function<void(void*)> fn)
		{
			userdata = ud;
			onButtonPressed = fn;
		}

		void* userdata;
		std::function<void(void*)> onButtonPressed;
	};

	// Holds a pair of "BARNAME|BUTTONNAME" and the corresponding struct
	std::map<std::string, cbStruct> ButtonCallbackMap;

	// Map of names and tables
	std::map<std::string, TwBar*> BarMap;

	// Whether to show the bars
	bool IsActive;
};

