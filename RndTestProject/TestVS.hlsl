//--------------------------------------------------------------------------------------
// Simple vertex shader
//--------------------------------------------------------------------------------------

cbuffer Matrices_PerFrame : register( b0 )
{
	matrix M_View;
	matrix M_Proj;
	matrix M_ViewProj;	
};

cbuffer Matrices_PerInstances : register( b1 )
{
	matrix M_World;
};


//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 vPosition	: POSITION;
	float2 vTexcoord	: TEXCOORD0;
	float4 vDiffuse		: DIFFUSE;
};

struct VS_OUTPUT
{
	float2 vTexcoord		: TEXCOORD0;
	float4 vDiffuse			: TEXCOORD1;
	float4 vPosition		: SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VSMain( VS_INPUT Input )
{
	VS_OUTPUT Output;
	
	//float3 positionWorld = mul(float4(Input.vPosition,1), M_World).xyz;
	
	//Output.vPosition = mul( float4(Input.vPosition,1), M_World * M_View * M_Proj);
	Output.vPosition = mul( float4(Input.vPosition,1), M_Proj * M_View * M_World);
	Output.vTexcoord = Input.vTexcoord;
	Output.vDiffuse  = Input.vDiffuse;

	return Output;
}