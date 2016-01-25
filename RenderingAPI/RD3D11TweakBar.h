#pragma once
#include "RTweakBarBase.h"

class RD3D11TweakBar : public RTweakBarBase
{
public:
	RD3D11TweakBar();
	~RD3D11TweakBar();

	/** Initializes the tweakbar */
	bool InitAPI();
};

