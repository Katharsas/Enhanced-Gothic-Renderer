#pragma once
#include "../Shared/SimpleMath.h"
using namespace DirectX::SimpleMath;

/** Struct handling all the graphical states set by the game. Can be used as Constantbuffer */
const int GSWITCH_FOG = 1;
const int GSWITCH_ALPHAREF = 2;
const int GSWITCH_LIGHING = 4;
const int GSWITCH_REFLECTIONS = 8;
const int GSWITCH_LINEAR_DEPTH = 16;

/** A single fixed function stage */
struct FixedFunctionStage
{
	enum EColorOp
	{
		CO_DISABLE = 1,
		CO_SELECTARG1 = 2,
		CO_SELECTART2 = 3,

		CO_MODULATE = 4,
		CO_MODULATE2X = 5,
		CO_MODULATE4X = 6,

		CO_ADD = 7,
		CO_SUBTRACT = 10
	};

	enum ETextureArg
	{
		TA_DIFFUSE = 0,
		TA_CURRENT = 1,
		TA_TEXTURE = 2,
		TA_TFACTOR = 3
	};

	/** Sets the default values for this struct */
	void SetDefault()
	{
		
	}


	EColorOp ColorOp;
	ETextureArg ColorArg1;
	ETextureArg ColorArg2;

	EColorOp AlphaOp;
	ETextureArg AlphaArg1;
	ETextureArg AlphaArg2;

	int FFS_Pad1;
	int FFS_Pad2;
};



struct FixedFunctionGraphicsState
{
	/** Sets the default values for this struct */
	void SetDefault()
	{
		FF_FogWeight = 0.0f;
		FF_FogColor = Vector3(1.0f,1.0f,1.0f);
		FF_FogNear = 1.0f;
		FF_FogFar = 10000.0f;

		FF_AmbientLighting = Vector3(1.0f,1.0f,1.0f);
		FF_TextureFactor = Vector4(1.0f,1.0f,1.0f,1.0f);

		FF_AlphaRef = 0.5f;

		FF_GSwitches = 0;
	}

	/** Sets one of the GraphicsFlags */
	void SetGraphicsSwitch(int flag, bool enabled)
	{
		if(enabled)
			FF_GSwitches |= flag;
		else
			FF_GSwitches &= ~flag;
	}

	/** Fog section */
	float FF_FogWeight;
	Vector3 FF_FogColor;

	float FF_FogNear;
	float FF_FogFar;
	float FF_zNear;
	float FF_zFar;

	/** Lighting section */
	Vector3 FF_AmbientLighting;
	float FF_Time;

	/** Texture factor section */
	Vector4 FF_TextureFactor;

	/** Alpha ref section */
	float FF_AlphaRef;

	/** Graphical Switches (Takes GSWITCH_*) */
	unsigned int FF_GSwitches;
	Vector2 ggs_Pad3;

	/** Matrices */
	Matrix ProjMatrix;
	Matrix ViewMatrix;
	Matrix WorldMatrix;
	Matrix WorldViewProj;

	/** Viewport */
	float FF_TopLeftX;
	float FF_TopLeftY;
	float FF_Width;
	float FF_Height;

	FixedFunctionStage FF_Stages[2];
};
