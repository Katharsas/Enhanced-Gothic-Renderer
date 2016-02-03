#include "pch.h"
#include "RD3D11TweakBar.h"
#include "../Shared/Logger.h"
#include "REngine.h"
#include "RDevice.h"

RD3D11TweakBar::RD3D11TweakBar()
{
}


RD3D11TweakBar::~RD3D11TweakBar()
{
}

/** Initializes the tweakbar */
bool RD3D11TweakBar::InitAPI()
{
	LogInfo() << "Initializing AntTweakBar";
	if(!TwInit(TW_DIRECT3D11, REngine::RenderingDevice->GetDevice()))
		return XR_FAILED;


	TwWindowSize(REngine::RenderingDevice->GetOutputResolution().x, REngine::RenderingDevice->GetOutputResolution().y);
	return true;
}