#include <Tools.h>

struct SceneParams
{
	float S_FogStart;
	float S_FogEnd;
	float S_FogRange; // = FogEnd - FogStart
	float S_Time;
	float4 S_FogColor;
	
	// CLUT from gothic. Packed in uint4 because otherwiese every uint in the array would get 16-byte aligned
	uint4 S_LightCLUT[256 / 4]; 
};

cbuffer CB_PerFrame : register( b0 )
{
	matrix M_View;
	matrix M_Proj;
	matrix M_ViewProj;	
	matrix M_InverseView;
	
	SceneParams PF_SceneParams;
	
};

float4 GetLightCLUT(float lighting)
{
	// This is the way PB computes the index for the CLUT
	const int MAX_CLUT = 255;
	uint v = clamp((uint)(lighting.x * MAX_CLUT), 0, MAX_CLUT);
	
	// Unpack clut
	uint clut = ((const uint[4])(PF_SceneParams.S_LightCLUT[v/4]))[v%4];
	
	return DWORDToFloat4(clut);
};

/** Computes the fog-term */
float3 ApplyFogLinear(float distance, float3 color)
{
	float l = saturate((distance-PF_SceneParams.S_FogStart)/(PF_SceneParams.S_FogRange));
	
	return lerp(color.rgb, PF_SceneParams.S_FogColor.rgb, l);
}


/** Computes the fog-term */
float3 ApplyFog(float distance, float3 color)
{
	return ApplyFogLinear(distance, color);
}

/** Computes the fog-term */
float4 ApplyFog(float distance, float4 color)
{
	return float4(ApplyFogLinear(distance, color.rgb), color.a);
}














