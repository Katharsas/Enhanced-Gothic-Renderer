/** Fixed function emulator */
#include "FFConstants.h"
#include "FFConstantBuffer.h"

Texture2D	texture0 : register( t0 );
Texture2D	texture1 : register( t1 );

struct XYZ_DIF_T1_Vertex
{
	float3 position : POSITION;
	float4 color : DIFFUSE;
	float2 texCoord : TEXCOORD;
};

struct XYZRHW_DIF_T1_Vertex
{
	float4 xyzrhw : POSITION;
	float4 color : DIFFUSE;
	float2 texCoord : TEXCOORD;
};


struct XYZRHW_DIF_SPEC_T1_Vertex
{
	float4 xyzrhw : POSITION;
	float4 color : DIFFUSE;
	float4 spec : SPECULAR;
	float2 texCoord : TEXCOORD;
};

struct XYZ_NRM_T1_Vertex
{
	float3 position : POSITION;
	float3 nrm : NORMAL;
	float2 texCoord : TEXCOORD;
};

/** Transforms a usual model-space vector into worldspace */
float3 TransformModelToWorld(float3 v)
{
	return mul(float4(v,1), FF_WorldMatrix).xyz;
}

/** Transforms a world-space vector into viewspace */
float3 TransformWorldToView(float3 v)
{
	return mul(float4(v,1), FF_ViewMatrix).xyz;
}

/** Transforms a view-space vector into projectionspace */
float4 TransformViewToProj(float3 v)
{
	return mul(float4(v,1), FF_ProjMatrix);
}

/** Transforms a usual model-space vector into projectionspace */
float4 TransformModelToProj(float3 v)
{
	//return mul(float4(v,1), mul(FF_ProjMatrix, mul(FF_ViewMatrix, FF_WorldMatrix)));
	return mul(float4(v,1), FF_WorldViewProj);
}

/** Rotates a model-space direction-vector to viewspace*/
float3 RotateModelToView(float3 v)
{
	return mul(v, (float3x3)mul(FF_ViewMatrix, FF_WorldMatrix)).xyz;
}

/** Transforms a pre-transformed xyzrhw-coordinate into d3d11-space */
float4 TransformXYZRHW(float4 xyzrhw)
{
	// Remove viewport-transformation
	xyzrhw.xy -= FF_ViewportPos;
	xyzrhw.xy = xyzrhw.xy * 2.0f - FF_ViewportSize;
	
	// We don't want this in pixels
	xyzrhw.xy /= FF_ViewportSize; 
	
	// D3D11 will turn this upside down later, so counter that here!
	xyzrhw.y = -xyzrhw.y;
	
	// Remove the stupid half-pixel offset from pre D3D10
	xyzrhw.xy -= 0.5f / FF_ViewportSize;
	
	// Remove w-component, as it can mess things up when not 1. (Why? Not sure, sorry)
	return float4(xyzrhw.xyz, 1);
}

/** Selects the input from the given texture arg */
float4 SelectArg(int arg, float4 current, float4 diffuse, Texture2D tex, float2 uv, SamplerState samplerState)
{
	switch(arg)
	{
	case D3DTA_DIFFUSE:
		return diffuse;
		break;
		
	case D3DTA_CURRENT:
		return current;
		break;
		
	case D3DTA_TEXTURE:
		return tex.Sample(samplerState, uv);
		break;
		
	case D3DTA_TFACTOR:
		return FF_TextureFactor;
		break;
	}
	
	// Invalid arg, return green to indicate the mistake
	return float4(0,1,0,0);
}

/** Internal function for the state emulator. Should not be used directly. */
float4 __runStage(int colorop, int colorarg1, int colorarg2, float4 current, float4 diffuse, Texture2D tex, float2 uv, SamplerState samplerState)
{
	switch(colorop)
	{
	case 0:
		return float4(1,0,1,1); // Invalid value
		break;
	
	case D3DTOP_DISABLE: // 1
		return 0;
		break;
	
	case D3DTOP_SELECTARG1: // 2
		return SelectArg(colorarg1, current, diffuse, tex, uv, samplerState);
		break;
		
	case D3DTOP_SELECTARG2: // 3
		return SelectArg(colorarg2, current, diffuse, tex, uv, samplerState);
		break;
	
	case D3DTOP_MODULATE: // 4
		return SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) * 
			   SelectArg(colorarg2, current, diffuse, tex, uv, samplerState);
		break;
		
	case D3DTOP_MODULATE2X: // 5
		return SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) * 
			   SelectArg(colorarg2, current, diffuse, tex, uv, samplerState) * 2;
		break;
		
	case D3DTOP_MODULATE4X: // 6
		return SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) * 
			   SelectArg(colorarg2, current, diffuse, tex, uv, samplerState) * 4;
		break;
		
	case D3DTOP_ADD: // 7
		return SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) +
			   SelectArg(colorarg2, current, diffuse, tex, uv, samplerState);
		break;
		
	case D3DTOP_ADDSIGNED: // 8
	case D3DTOP_ADDSIGNED2X: // 9
		return float4(1,1,0,1);
		break;
		
	case D3DTOP_SUBTRACT: // 10
		return SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) -
			   SelectArg(colorarg2, current, diffuse, tex, uv, samplerState);
		break;
		
	case D3DTOP_ADDSMOOTH: // 11
		return SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) +
			   SelectArg(colorarg2, current, diffuse, tex, uv, samplerState) -
			   SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) *
			   SelectArg(colorarg2, current, diffuse, tex, uv, samplerState);
		break;
		
	//Arg1*(Alpha) + Arg2*(1-Alpha)
	case D3DTOP_BLENDDIFFUSEALPHA: // 12
		return 	SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) * diffuse.a + 
				SelectArg(colorarg2, current, diffuse, tex, uv, samplerState) * (1 - diffuse.a);
		break;
	case D3DTOP_BLENDTEXTUREALPHA: // 13
		return 	SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) * tex.Sample(samplerState, uv).a + 
				SelectArg(colorarg2, current, diffuse, tex, uv, samplerState) * (1 - tex.Sample(samplerState, uv).a);
		break;
		
	case D3DTOP_BLENDFACTORALPHA: // 14
		return 	SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) * FF_TextureFactor.a + 
				SelectArg(colorarg2, current, diffuse, tex, uv, samplerState) * (1 - FF_TextureFactor.a);
		break;
		
	case D3DTOP_BLENDTEXTUREALPHAPM: // 15
		return 	SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) + 
				SelectArg(colorarg2, current, diffuse, tex, uv, samplerState) * (1 - tex.Sample(samplerState, uv).a);
		break;
	
	case D3DTOP_BLENDCURRENTALPHA: // 16
		return 	SelectArg(colorarg1, current, diffuse, tex, uv, samplerState) * current.a + 
				SelectArg(colorarg2, current, diffuse, tex, uv, samplerState) * (1 - current.a);
		break;
		
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
		return float4(0,1,0,0); // Unsupported
		break;
	}

	return float4(1,0,0,0); // Invalid value
	
}

float4 GetColorFromStates(float4 diffuse, float2 uv, float2 uv2, SamplerState samplerState)
{		
	float4 color = __runStage(FF_Stages[0].colorop, FF_Stages[0].colorarg1, FF_Stages[0].colorarg2, diffuse, diffuse, texture0, uv, samplerState);
	
	if(FF_Stages[1].colorop != D3DTOP_DISABLE)
	{
		//color = __runStage(FF_Stages[1].colorop, FF_Stages[1].colorarg1, FF_Stages[1].colorarg2, color, diffuse, texture1, uv2, samplerState);
	}
	
	float4 alpha = color;//__runStage(FF_Stages[0].alphaop, FF_Stages[0].alphaarg1, FF_Stages[0].alphaarg2, diffuse, diffuse, texture0, uv, samplerState);

	if(FF_AlphaTestEnabled())
		DoAlphaTest(alpha.a);
	
	//return textureFactor;
	return float4(color.rgb, alpha.a);
}
