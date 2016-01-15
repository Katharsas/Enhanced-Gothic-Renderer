//--------------------------------------------------------------------------------------
// World/VOB-Pixelshader for G2D3D11 by Degenerated
//--------------------------------------------------------------------------------------

#include <PerFrameConstantBuffer.h>

#define LAST_TEXTURE_SLOT t1

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
SamplerState SS_Linear : register( s0 );
Texture2D	TX_Texture0 : register( t0 );

#ifdef LIGHTMAPPING
Texture2D	TX_Lightmap : register( LAST_TEXTURE_SLOT );
#endif

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float2 vTexcoord		: TEXCOORD0;
	float2 vTexcoord2		: TEXCOORD1;
	float4 vDiffuse			: TEXCOORD2;
	float3 vNormalVS		: TEXCOORD4;
	float3 vViewPosition	: TEXCOORD5;
	float4 vPosition		: SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSMain( PS_INPUT Input ) : SV_TARGET
{
	float4 color = TX_Texture0.Sample(SS_Linear, Input.vTexcoord);
	
	// Get sky-color
	float4 lighting = GetLightCLUT(Input.vDiffuse.r);
	
#ifdef LIGHTMAPPING
	
	
	// Sample lightmap
	float4 lightmap = TX_Lightmap.Sample(SS_Linear, Input.vTexcoord2);
	lighting *= lightmap;
	
#endif

	// Apply lighting
	color *= lighting;
	
#ifdef DO_ALPHATEST
	// Gothic always uses a value around 0.6
	// TODO: Get this from FF-State or via other constantbuffers
	clip(color.a - 0.6);
#endif
	
	return ApplyFog(length(Input.vViewPosition), color);
}

