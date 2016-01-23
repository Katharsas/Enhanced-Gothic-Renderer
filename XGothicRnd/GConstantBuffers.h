#pragma once
#include "pch.h"

namespace ConstantBuffers
{
	struct SceneParams
	{
		float S_FogStart;
		float S_FogEnd;
		float S_FogRange; // = FogEnd - FogStart
		float S_Time;
		float4 S_FogColor;

		DWORD S_LightCLUT[256]; // CLUT from gothic
	};

	struct PerFrameConstantBuffer
	{
		Matrix M_View;
		Matrix M_Proj;
		Matrix M_ViewProj;
		Matrix M_InverseView;

		SceneParams PF_SceneParams;
	};
};