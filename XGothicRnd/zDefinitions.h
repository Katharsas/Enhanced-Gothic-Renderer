#pragma once
#include "zCArray.h"

class zCWorld;
class zCOption;
class zSTRING;
class zCBspTree;
class zCClassDef;

typedef int zBOOL;
typedef short zWORD;

/** Load-mode */
enum zTWorldLoadMode
{	
	ZTW_LOAD_GAME_STARTUP,													
	ZTW_LOAD_GAME_SAVED_DYN,												
	ZTW_LOAD_GAME_SAVED_STAT,												
	ZTW_LOAD_EDITOR_COMPILED,																									
	ZTW_LOAD_EDITOR_UNCOMPILED,											
	ZTW_LOAD_MERGE 
};

enum zTBspTreeMode		
{	
	zTB_MODE_INDOOR,						
	zTB_MODE_OUTDOOR 
};

enum zTWld_RenderMode 
{ 
	zTW_RENDER_MODE_VERT_LIGHT,		
	zTW_RENDER_MODE_LIGHTMAPS  
};	

struct zTBBox3D 
{
	float3 m_Min;	
	float3 m_Max;

	enum zTPlaneClass 
	{ 
		zPLANE_INFRONT, 
		zPLANE_BEHIND, 
		zPLANE_ONPLANE, 
		zPLANE_SPANNING 
	};

	DirectX::BoundingBox ToBoundingBox()
	{
		return DirectX::BoundingBox(0.5f * (m_Min + m_Max), m_Max - m_Min);
	}

	int ClassifyToPlane (float planeDist, int axis) const 
	{
		if (planeDist>=((float*)&m_Max)[axis])	
			return zPLANE_BEHIND;	
		else if (planeDist<=((float*)&m_Max)[axis])	
			return zPLANE_INFRONT;	
		else return zPLANE_SPANNING;
	}
};

struct zTPlane
{
	// Can't use the DX-Math library for this, since PB decided they wanted to store dxyz instead of xyzd, like everyone else does.
	float m_Distance;
	float3 m_Normal;
};

enum zTBspNodeType 
{ 
	zBSP_LEAF=1, 
	zBSP_NODE=0 
};

struct zCVertex
{
	float3 m_Position;

	int m_TransformedIndex;
	int m_MyIndex;
};

struct zCVertFeature
{
	float3 m_Normal;
	DWORD m_LightStatic;
	DWORD m_LightDynamic;
	float2 m_TexCoord;
};

struct zTBSphere3D 
{			
	float3 m_Center;
	float m_Radius;
};

class zCOBBox3D 
{
public:
	float3 m_Center;
	float3 m_Axis[3];
	float3 m_Extend;

	zCList<zCOBBox3D> m_Children;
};

enum zTClipFlags
{ 
	CLIP_FLAGS_FULL	= 63, 
	CLIP_FLAGS_FULL_WO_FAR = 15 
};

enum zTResourceCacheState
{
	zRES_FAILURE =-1,
	zRES_CACHED_OUT	= 0,
	zRES_QUEUED	= 1,
	zRES_LOADING = 2,
	zRES_CACHED_IN = 3
};

enum zTVisualCamAlign
{	
	zVISUAL_CAMALIGN_NONE = 0,
	zVISUAL_CAMALIGN_YAW = 1,
	zVISUAL_CAMALIGN_FULL = 2,
	zVISUAL_CAMALIGN_COUNT = 3
};

enum zTRnd_AlphaBlendFunc	{	
	zRND_ALPHA_FUNC_MAT_DEFAULT,			
	zRND_ALPHA_FUNC_NONE,					
	zRND_ALPHA_FUNC_BLEND,					
	zRND_ALPHA_FUNC_ADD,					
	zRND_ALPHA_FUNC_SUB,					
	zRND_ALPHA_FUNC_MUL,					
	zRND_ALPHA_FUNC_MUL2,					
	zRND_ALPHA_FUNC_TEST,					
	zRND_ALPHA_FUNC_BLEND_TEST				
};	 	

enum zTCam_ClipType
{
	ZTCAM_CLIPTYPE_IN,
	ZTCAM_CLIPTYPE_OUT,
	ZTCAM_CLIPTYPE_CROSSING
};