//--------------------------------------------------------------------------------------
// Fixed-Function Emulator for pretransformed vertices
//--------------------------------------------------------------------------------------

#include <FixedFunctionPipeline.h>

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------

struct VS_OUTPUT
{
	float2 vTexcoord		: TEXCOORD0;
	float2 vTexcoord2		: TEXCOORD1;
	float4 vDiffuse			: TEXCOORD2;
	float3 vViewPosition 	: TEXCOORD3;
	float3 vNormalVS		: TEXCOORD4;
	float4 vPosition		: SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VSMain( XYZRHW_DIF_SPEC_T1_Vertex Input )
{
	VS_OUTPUT Output;
			
	Output.vPosition = TransformXYZRHW(Input.xyzrhw);
	
	Output.vTexcoord2 = 0;
	Output.vTexcoord = Input.texCoord;
	Output.vDiffuse  = Input.color;
	Output.vNormalVS = float3(0,0,0);
	Output.vViewPosition = float3(0,0,0);
	
	return Output;
}

