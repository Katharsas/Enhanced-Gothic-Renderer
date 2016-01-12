struct SceneParams
{
	float S_FogStart;
	float S_FogEnd;
	float S_FogRange; // = FogEnd - FogStart
	float S_pad0;
	float4 S_FogColor;
};

cbuffer CB_PerFrame : register( b0 )
{
	matrix M_View;
	matrix M_Proj;
	matrix M_ViewProj;	
	matrix M_InverseView;
	
	SceneParams PF_SceneParams;
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














