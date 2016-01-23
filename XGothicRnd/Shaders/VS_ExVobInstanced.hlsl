//--------------------------------------------------------------------------------------
// Vertexshader for the main-pass of the World-Mesh
//--------------------------------------------------------------------------------------

#include <Tools.h>

#include <PerFrameConstantBuffer.h>

struct VobInstanceInfo
{
	// Vobs don't need scale, so only pass position and rotation as quaternion
	float3 vPosition;
	uint cInstanceColor;

	float4 qRotation;
};


//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTex1		: TEXCOORD0;
	float2 vTex2		: TEXCOORD1;
	float4 vDiffuse		: DIFFUSE;
	float3 vInstancePosition : INSTANCE_POSITION;
	float4 cInstanceColor : INSTANCE_COLOR;
	float4 qInstanceRotation : INSTANCE_ROTATION;

};

struct VS_OUTPUT
{
	float2 vTexcoord		: TEXCOORD0;
	float2 vTexcoord2		: TEXCOORD1;
	float4 vDiffuse			: TEXCOORD2;
	float3 vNormalVS		: TEXCOORD4;
	float3 vViewPosition	: TEXCOORD5;
	float4 vPosition		: SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VSMain( VS_INPUT Input )
{
	VS_OUTPUT Output;
		
	// Transform the local vertex to world-coords
	float3 positionWorld = RotateVectorByQuat(Input.qInstanceRotation, Input.vPosition) + Input.vInstancePosition;

	// TODO: Use instancecolor
	
	// Now transform to view and projection-space and output to next shaderstage
	Output.vPosition = mul( float4(positionWorld,1), M_ViewProj);
	Output.vTexcoord2 = Input.vTex2;
	Output.vTexcoord = Input.vTex1;
	Output.vDiffuse  = Input.vDiffuse * Input.cInstanceColor;
	Output.vNormalVS = mul(Input.vNormal, (float3x3)M_View);
	Output.vViewPosition = mul(float4(positionWorld,1), M_View);
	
	return Output;
}

