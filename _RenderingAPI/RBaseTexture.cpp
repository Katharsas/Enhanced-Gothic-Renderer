#include "pch.h"
#include "RBaseTexture.h"
#include "../Shared/Toolbox.h"

// This shouldn't be needed, there are no Windows-h-includes without NOMINMAX AFAIK.
#undef min
#undef max

RBaseTexture::RBaseTexture()
{
	SizeInBytes = 0;
	TextureFormat = ETextureFormat::TF_R8G8B8A8;
	NumMipLevels = 0;
	BindFlags = EBindFlags::B_SHADER_RESOURCE;
	UsageFlags = EUsageFlags::U_DEFAULT;
	IsFullyInitialized = false;
}


RBaseTexture::~RBaseTexture()
{
}


UINT RBaseTexture::ComputeSizeInBytes(int mip, const INT2& resolution, ETextureFormat format)
{
	int px = (int)std::max(1.0f, floor(resolution.x / pow(2.0f, mip)));
	int py = (int)std::max(1.0f, floor(resolution.y / pow(2.0f, mip)));
	//int px = TextureSize.x;
	//int py = TextureSize.y;

	if (format == DXGI_FORMAT_BC1_UNORM || format == DXGI_FORMAT_BC2_UNORM ||
		format == DXGI_FORMAT_BC3_UNORM)
	{
		return Toolbox::GetDDSStorageRequirements(px, py, format == DXGI_FORMAT_BC1_UNORM);
	}
	else // Use R8G8B8A8
	{
		return px * py * 4;
	}
}

UINT RBaseTexture::ComputeRowPitchBytes(int mip, const INT2& resolution, ETextureFormat format)
{
	int px = (int)std::max(1.0f, floor(resolution.x / pow(2.0f, mip)));
	int py = (int)std::max(1.0f, floor(resolution.y / pow(2.0f, mip)));
	//int px = TextureSize.x;
	//int py = TextureSize.y;

	if (format == FORMAT_BC1_UNORM || format == FORMAT_BC2_UNORM ||
		format == FORMAT_BC3_UNORM)
	{
		return Toolbox::GetDDSRowPitchSize(px, format == DXGI_FORMAT_BC1_UNORM);
	}
	else // Use R8G8B8A8
	{
		return px * 4;
	}
}

/** Returns the RowPitch-Bytes */
UINT RBaseTexture::ComputeRowPitchBytes(int mip)
{
	return RBaseTexture::ComputeRowPitchBytes(mip, Resolution, TextureFormat);
}

/** Returns the size of the texture in bytes */
UINT RBaseTexture::ComputeSizeInBytes(int mip)
{
	return RBaseTexture::ComputeSizeInBytes(mip, Resolution, TextureFormat);
}
