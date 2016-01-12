//--------------------------------------------------------------------------------------
// Vertexshader for the main-pass of the World-Mesh
//--------------------------------------------------------------------------------------

#include <PerFrameConstantBuffer.h>

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	uint Idx : SV_VertexId;
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

struct ParticleInfo
{
	float3 m_WorldPosition;
	float3 m_Velocity;
	float2 m_Scale;
	float4 m_Color;
};

StructuredBuffer<ParticleInfo> Particles : register(s0);

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VSMain( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	uint n = input.Idx / 6;
	
	float4 pl;
	float2 tx;
	
	/*
	float4(-1.0f, 1.0f, 1.0f, 1.0f);

	if(VtxID == 1)
		output = float4(3.0f, 1.0f, 1.0f, 1.0f);
	else if(VtxID == 2)
		output = float4(-1.0f, -3.0f, 1.0f, 1.0f);
	*/
	
	// Generate a quad
	// 0  1  2    3  4  5
	// UL/UR/BL - UR/BR/BL
	switch(input.Idx % 6)
	{
	case 0:
		pl = float4(-1.0f, 1.0f, 0.0f, 1.0f); // UL
		tx = float2(0.0f,0.0f);
		break;
		
	case 3:
	case 1:
		pl = float4(1.0f, 1.0f, 0.0f, 1.0f); // UR
		tx = float2(1.0f,0.0f);
		break;
		
	case 5:
	case 2:
		pl = float4(-1.0f, -1.0f, 0.0f, 1.0f); // BL
		tx = float2(0.0f,1.0f);
		break;
		
	case 4:
		pl = float4(1.0f, -1.0f, 0.0f, 1.0f); // BR
		tx = float2(1.0f,1.0f);
		break;
	}
	
	// Scale the particle
	pl.xy *= Particles[n].m_Scale;
	
	// Undo rotations
	pl.xyz = mul(pl.xyz, (float3x3)M_InverseView);
	
	// Offset by world position
	pl.xyz += Particles[n].m_WorldPosition;
	
	output.vPosition = mul( pl, M_ViewProj);
	output.vTexcoord2 = 0;
	output.vTexcoord = tx;
	output.vDiffuse  = Particles[n].m_Color;
	output.vNormalVS = 0;
	output.vViewPosition = 0;
	
	return output;
}


















