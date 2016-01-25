#pragma once
#include "pch.h"
#include "zEngineFunctions.h"
#include "zCObject.h"
#include "zCTexture.h"

enum zTMat_Group 
{
	zMAT_GROUP_UNDEF = 0,
	zMAT_GROUP_METAL,
	zMAT_GROUP_STONE,
	zMAT_GROUP_WOOD,
	zMAT_GROUP_EARTH,
	zMAT_GROUP_WATER,
	zMAT_GROUP_SNOW,
	zMAT_NUM_MAT_GROUP
};

class zCTexAniCtrl 
{
private:
	int m_AniChannel;
	float m_ActFrame;
	float m_AniFPS;
	DWORD m_FrameCtr;
	zBOOL m_BOneShotAni;

public:
	void AdvanceAni(zCTexture* texture)
	{
		XCALL(MemoryLocations::Gothic::zCTexAniCtrl__AdvanceAni_zCTexture_p);
	}
};


struct zCBspSector;
class zCTexture;
class zCMaterial : public zCObject
{
public:

	/** Returns the texture associated with this material */
	zCTexture* GetTexture()
	{
		if (m_Texture) 
		{
			if (m_Texture->GetTextureFlags().IsAnimated) 
			{ 
				m_TexAniCtrl.AdvanceAni(m_Texture); 
				return m_Texture->GetAniTexture();
			}
			else
			{
				return m_Texture;
			}
		}
		else
		{
			return 0;
		}

		return nullptr;
	}

	/** Returns the blendfunc of this material */
	zTRnd_AlphaBlendFunc GetBlendFunc()
	{
		return m_Flags.m_RndAlphaBlendFunc;
	}

	/** Sets the blend-func of this material */
	void SetBlendFunc(zTRnd_AlphaBlendFunc func)
	{
		m_Flags.m_RndAlphaBlendFunc = func;
	}

	zCBspSector* GetSectorFront()
	{
		return m_BspSectorFront;
	}

	zCBspSector* GetSectorBack()
	{
		return m_BspSectorBack;
	}

private:
	
	zCArray<class zCPolygon*> m_PolyList;
	DWORD m_PolyListTimeStamp;

	zCTexture* m_Texture;
	DWORD m_Color;		
	float m_SmoothAngle;
	zTMat_Group m_MatGroup;
	zCBspSector* m_BspSectorFront;
	zCBspSector* m_BspSectorBack;
	zCTexAniCtrl m_TexAniCtrl;
	zSTRING* m_DetailObjectVisualName;

	float m_KAmbient;
	float m_KDiffuse;
	float m_EnvironmentalMappingStrength;

	struct zMatFlags
	{
		byte m_Smooth : 1;
		byte m_DontUseLightmaps : 1;
		byte m_TexAniMap : 1;
		byte m_LodDontCollapse : 1;
		byte m_NoCollDet : 1;
		byte m_ForceOccluder : 1;
		byte m_EnvironmentalMapping : 1;
		byte m_PolyListNeedsSort : 1;
		byte m_MatUsage : 8;
		byte m_LibFlag : 8;
		zTRnd_AlphaBlendFunc m_RndAlphaBlendFunc : 8;
		byte m_IgnoreSun : 1;
	}m_Flags;

	/*zTWaveAniMode			m_enuWaveMode;
	zTFFT					m_enuWaveSpeed;
	float					m_fWaveMaxAmplitude;
	float					m_fWaveGridSize;*/
};