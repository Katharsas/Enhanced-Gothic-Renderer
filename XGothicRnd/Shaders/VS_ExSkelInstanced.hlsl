//--------------------------------------------------------------------------------------
// Vertexshader for the main-pass of a skeletal mesh
//--------------------------------------------------------------------------------------

#include <Const.h>
#include <Tools.h>

#include <PerFrameConstantBuffer.h>

cbuffer ModelConstantBuffer : register( b1 )
{
	float M_Fatness;
	float M_Scale;
	float2 M_Pad1;
	Matrix M_NodeTransforms[NUM_MAX_NODES];
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
	float3 vLocalPosition[4]	: LOCALPOSITION;
	uint4  iBoneIndices : BONEIDS;
	float4 vWeights 	: WEIGHTS;
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
	
	// Transform vertices to the skeletons current state (local space)
	float3 position = float3(0,0,0);
	for(int i=0;i<4;i++)
	{
		position += Input.vWeights[i] * mul(float4(Input.vLocalPosition[i], 1), M_NodeTransforms[Input.iBoneIndices[i]]).xyz;
	}
	
	// Transform the local vertex to world-coords
	//position = Input.vPosition;
	float3 positionWorld = RotateVectorByQuat(Input.qInstanceRotation, position) + Input.vInstancePosition;
	
	// TODO: Fatness
	// TODO: Model scale
	// TODO: Normal is messed up and must be transformed using the bone-transforms as well
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

