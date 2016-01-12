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