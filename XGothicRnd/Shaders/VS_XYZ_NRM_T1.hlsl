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
VS_OUTPUT VSMain( XYZ_NRM_T1_Vertex Input )
{
	VS_OUTPUT Output;
			
	Output.vPosition = TransformModelToProj(Input.position);
	
	Output.vTexcoord2 = 0;
	Output.vTexcoord = Input.texCoord;
	Output.vDiffuse  = 1;
	Output.vNormalVS = RotateModelToView(Input.nrm);
	Output.vViewPosition = TransformWorldToView(TransformModelToWorld(Input.position));
	
	return Output;
}


