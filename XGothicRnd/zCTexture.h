#pragma once
#include "zDefinitions.h"
#include "zCResource.h"
#include "D3D7\MyDirectDrawSurface7.h"

// zCTexture needs a second vtable, so just leave this class as empty as possible
// and extend from it
class zCTextureExchange
{
public:
	virtual ~zCTextureExchange() = 0;
};

class zCTexture : public zCResource, public zCTextureExchange
{
public:

	/** Creates a new texture object and registers the name. Does not cache the texture in! */
	static zCTexture* Load(const zSTRING& texFile, int texLoadFlags = 0)
	{
		XCALL(MemoryLocations::Gothic::zCTexture__Load_zSTRING_r_int);
	}

	struct zTextureFlags
	{
		byte HasAlpha			: 1; // Only valid after the texture was cached in
		byte IsAnimated			: 1;				
		byte ChangingRealtime	: 1;				
		byte IsTextureTile		: 1;
	};

	/** Returns the flags of this texture */
	zTextureFlags& GetTextureFlags(){return m_TextureFlags;}

	/** Returns the D3D7-Surface allocated in the zCTex_D3D-Part of this object.
		It's okay to simply access it here, because all zCTextures are zCTex_D3D anyways */
	MyDirectDrawSurface7* GetSurface()
	{
		return *(MyDirectDrawSurface7**)THISPTR_OFFSET(MemoryOffsets::Gothic::zCTexture::D3D7Surface);
	}

	/** Returns the current animation-texture */
	zCTexture* GetAniTexture()
	{
		if (!m_TextureFlags.IsAnimated)
			return this;


		zCTexture* tex = this;
		for (int i=0; i<zTEX_MAX_ANIMATIONS; i++) 
		{
			if (m_AnimationNumFrames[i]==0) 
				continue;

			for (int j=0; j<m_AnimationActiveFrame[i]; j++) 
			{
				if (!tex->m_NextAniTexture[i]) 
					break;

				tex = tex->m_NextAniTexture[i];
			}
		}
		return tex;
	}

private:
	enum
	{
		zTEX_MAX_ANIMATIONS = 3,
	};

	// These textures can have multiple animations as it seems
	zCTexture* m_NextAniTexture[zTEX_MAX_ANIMATIONS];
	zCTexture* m_PrevAniTexture[zTEX_MAX_ANIMATIONS];
	int	m_AnimationActiveFrame[zTEX_MAX_ANIMATIONS];
	int	m_AnimationNumFrames[zTEX_MAX_ANIMATIONS];

	zTextureFlags m_TextureFlags;
};


class zCLightmap : public zCObject
{	
public:

	/** Returns matching UV-Coords for the input world-position */
	float2 GetLightmapTexCoords (const float3& posWorldSpace) const 
	{
		float3 q = posWorldSpace - m_LightmapOrigin;
		return float2(q.Dot(m_LightmapUVRight), q.Dot(m_LightmapUVUp));
	}

	/** Accessors */
	void GetLightmapUVs(float3& origin, float3& up, float3& right)
	{
		origin = m_LightmapOrigin;
		up = m_LightmapUVUp;
		right = m_LightmapUVRight;
	}

	zCTexture* GetTexture()
	{
		return m_Texture;
	}

private:
	float3 m_LightmapOrigin;
	float3 m_LightmapUVUp;
	float3 m_LightmapUVRight;

	zCTexture* m_Texture;
};