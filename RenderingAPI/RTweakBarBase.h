#pragma once
#include "pch.h"

class RTweakBarBase
{
public:
	RTweakBarBase();
	virtual ~RTweakBarBase();

protected:

	// Map of names and tables
	std::map<std::string, TwBar*> BarMap;

	// Whether to show the bars
	bool IsActive;
};

