#pragma once
#include "pch.h"

namespace ConstantBuffers
{
	struct SceneParams
	{
		float S_FogStart;
		float S_FogEnd;
		float S_FogRange; // = FogEnd - FogStart
		float S_pad0;
		float4 S_FogColor;
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