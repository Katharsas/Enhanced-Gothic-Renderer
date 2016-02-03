#pragma once
#include "../Shared/Types.h"
#include "RResource.h"

/**
 * Base class for textures. Holds generic data about textures 
 */
class RBaseTexture : public RResource
{
public:

	RBaseTexture();
	virtual ~RBaseTexture();

	/**
	 *Returns the RowPitch-Bytes 
	 */
	UINT ComputeRowPitchBytes(int mip);

	/** 
	  * Returns the size of the texture in bytes 
	  */
	UINT ComputeSizeInBytes(int mip);

	/** 
	  * Returns the size of the texture in bytes 
	  */
	static UINT ComputeSizeInBytes(int mip, const INT2& resolution, ETextureFormat format);
	static UINT ComputeRowPitchBytes(int mip, const INT2& resolution, ETextureFormat format);

	/**
	 * Getters
	 */
	ETextureFormat GetTextureFormat(){return TextureFormat;}
	UINT GetSizeInBytes(){return SizeInBytes;}
	INT2 GetResolution(){return Resolution;}
	UINT GetNumMipLevels(){return NumMipLevels;}
	EBindFlags GetBindFlags(){return BindFlags;}
	EUsageFlags GetUsageFlags(){return UsageFlags;}

protected:
	// Size of the whole texture in bytes
	UINT SizeInBytes;

	// Format of the texture data
	ETextureFormat TextureFormat;

	// Wheather this texture is dynamic or not
	bool IsDynamic;

	// Size of the textures highest miplevel in pixels
	INT2 Resolution;

	// Number if mip-levels we have
	UINT NumMipLevels;

	// Bind flags
	EBindFlags BindFlags;

	// Usage
	EUsageFlags UsageFlags;

	// True if everything was created properly
	bool IsFullyInitialized;

	// Number of texture we have in a texture array
	unsigned int ArraySize;
};
