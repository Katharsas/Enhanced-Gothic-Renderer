#pragma once

#include <string>
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

/** Defines types used for the project */

/** Errorcodes */
enum XRESULT
{
	XR_SUCCESS,
	XR_FAILED,
	XR_INVALID_ARG,
};

struct INT2
{
	INT2(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	INT2(const Vector2& v)
	{
		this->x = (int)(v.x + 0.5f);
		this->y = (int)(v.y + 0.5f);
	}

	INT2(){}

	std::string toString()
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}

	bool operator== (const INT2& v)
	{
		return x == v.x && y == v.y;
	}

	bool operator!= (const INT2& v)
	{
		return x != v.x || y != v.y;
	}

	int x;
	int y;
};


struct INT4
{
	INT4(int x, int y, int z, int w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	INT4(){}

	int x;
	int y;
	int z;
	int w;
};

struct ViewportInfo
{
	ViewportInfo(){}
	ViewportInfo(	unsigned int topleftX, 
		unsigned int topleftY, 
		unsigned int width, 
		unsigned int height, 
		float minZ = 0.0f, 
		float maxZ = 1.0f)
	{
		TopLeftX = (float)topleftX;
		TopLeftY = (float)topleftY;
		Width = (float)width;
		Height = (float)height;
		MinZ = minZ;
		MaxZ = maxZ;
	}

	float TopLeftX;
	float TopLeftY;
	float Width;
	float Height;
	float MinZ;	
	float MaxZ;
};

struct DisplayModeInfo
{
	unsigned int Height;
	unsigned int Width;
	unsigned int Bpp;
};

/** Layed out for D3D11*/
enum ECPUAccessFlags
{
	CA_NONE = 0,
	CA_WRITE = 0x10000L,
	CA_READ = 0x20000L,
};

/** Layed out for D3D11*/
enum EUsageFlags
{
	U_DEFAULT = 0,
	U_STAGING = 3,
	U_DYNAMIC = 2,
	U_IMMUTABLE = 1
};

/** Layed out for D3D11*/
enum EMapFlags
{
	M_READ = 1,
	M_WRITE = 2,
	M_READ_WRITE = 3,
	M_WRITE_DISCARD = 4,
};

/** Layed out for D3D11*/
enum EBindFlags
{
	B_VERTEXBUFFER = 0x1,
	B_INDEXBUFFER = 0x2,
	B_STREAM_OUT = 0x10,
	B_SHADER_RESOURCE = 0x8,
	B_RENDERTARGET = 0x20,
	B_CONSTANTBUFFER = 0x4,
	B_DEPTHSTENCIL = 0x40,
};

/** Layed out for DXGI */
enum ETextureFormat
{
	TF_R8G8B8A8 = 28,
	TF_DXT1 = 71,
	TF_DXT3 = 74,
	TF_DXT5 = 77,
	TF_FORMAT_R32_TYPELESS = 39,
    TF_FORMAT_D32_FLOAT = 40,
    TF_FORMAT_R32_FLOAT = 41,
};

// Same as DXGI_FORMAT
enum EFormat
{
	FORMAT_UNKNOWN	                    = 0,
	FORMAT_R32G32B32A32_TYPELESS       = 1,
	FORMAT_R32G32B32A32_FLOAT          = 2,
	FORMAT_R32G32B32A32_UINT           = 3,
	FORMAT_R32G32B32A32_SINT           = 4,
	FORMAT_R32G32B32_TYPELESS          = 5,
	FORMAT_R32G32B32_FLOAT             = 6,
	FORMAT_R32G32B32_UINT              = 7,
	FORMAT_R32G32B32_SINT              = 8,
	FORMAT_R16G16B16A16_TYPELESS       = 9,
	FORMAT_R16G16B16A16_FLOAT          = 10,
	FORMAT_R16G16B16A16_UNORM          = 11,
	FORMAT_R16G16B16A16_UINT           = 12,
	FORMAT_R16G16B16A16_SNORM          = 13,
	FORMAT_R16G16B16A16_SINT           = 14,
	FORMAT_R32G32_TYPELESS             = 15,
	FORMAT_R32G32_FLOAT                = 16,
	FORMAT_R32G32_UINT                 = 17,
	FORMAT_R32G32_SINT                 = 18,
	FORMAT_R32G8X24_TYPELESS           = 19,
	FORMAT_D32_FLOAT_S8X24_UINT        = 20,
	FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
	FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
	FORMAT_R10G10B10A2_TYPELESS        = 23,
	FORMAT_R10G10B10A2_UNORM           = 24,
	FORMAT_R10G10B10A2_UINT            = 25,
	FORMAT_R11G11B10_FLOAT             = 26,
	FORMAT_R8G8B8A8_TYPELESS           = 27,
	FORMAT_R8G8B8A8_UNORM              = 28,
	FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
	FORMAT_R8G8B8A8_UINT               = 30,
	FORMAT_R8G8B8A8_SNORM              = 31,
	FORMAT_R8G8B8A8_SINT               = 32,
	FORMAT_R16G16_TYPELESS             = 33,
	FORMAT_R16G16_FLOAT                = 34,
	FORMAT_R16G16_UNORM                = 35,
	FORMAT_R16G16_UINT                 = 36,
	FORMAT_R16G16_SNORM                = 37,
	FORMAT_R16G16_SINT                 = 38,
	FORMAT_R32_TYPELESS                = 39,
	FORMAT_D32_FLOAT                   = 40,
	FORMAT_R32_FLOAT                   = 41,
	FORMAT_R32_UINT                    = 42,
	FORMAT_R32_SINT                    = 43,
	FORMAT_R24G8_TYPELESS              = 44,
	FORMAT_D24_UNORM_S8_UINT           = 45,
	FORMAT_R24_UNORM_X8_TYPELESS       = 46,
	FORMAT_X24_TYPELESS_G8_UINT        = 47,
	FORMAT_R8G8_TYPELESS               = 48,
	FORMAT_R8G8_UNORM                  = 49,
	FORMAT_R8G8_UINT                   = 50,
	FORMAT_R8G8_SNORM                  = 51,
	FORMAT_R8G8_SINT                   = 52,
	FORMAT_R16_TYPELESS                = 53,
	FORMAT_R16_FLOAT                   = 54,
	FORMAT_D16_UNORM                   = 55,
	FORMAT_R16_UNORM                   = 56,
	FORMAT_R16_UINT                    = 57,
	FORMAT_R16_SNORM                   = 58,
	FORMAT_R16_SINT                    = 59,
	FORMAT_R8_TYPELESS                 = 60,
	FORMAT_R8_UNORM                    = 61,
	FORMAT_R8_UINT                     = 62,
	FORMAT_R8_SNORM                    = 63,
	FORMAT_R8_SINT                     = 64,
	FORMAT_A8_UNORM                    = 65,
	FORMAT_R1_UNORM                    = 66,
	FORMAT_R9G9B9E5_SHAREDEXP          = 67,
	FORMAT_R8G8_B8G8_UNORM             = 68,
	FORMAT_G8R8_G8B8_UNORM             = 69,
	FORMAT_BC1_TYPELESS                = 70,
	FORMAT_BC1_UNORM                   = 71,
	FORMAT_BC1_UNORM_SRGB              = 72,
	FORMAT_BC2_TYPELESS                = 73,
	FORMAT_BC2_UNORM                   = 74,
	FORMAT_BC2_UNORM_SRGB              = 75,
	FORMAT_BC3_TYPELESS                = 76,
	FORMAT_BC3_UNORM                   = 77,
	FORMAT_BC3_UNORM_SRGB              = 78,
	FORMAT_BC4_TYPELESS                = 79,
	FORMAT_BC4_UNORM                   = 80,
	FORMAT_BC4_SNORM                   = 81,
	FORMAT_BC5_TYPELESS                = 82,
	FORMAT_BC5_UNORM                   = 83,
	FORMAT_BC5_SNORM                   = 84,
	FORMAT_B5G6R5_UNORM                = 85,
	FORMAT_B5G5R5A1_UNORM              = 86,
	FORMAT_B8G8R8A8_UNORM              = 87,
	FORMAT_B8G8R8X8_UNORM              = 88,
	FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
	FORMAT_B8G8R8A8_TYPELESS           = 90,
	FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
	FORMAT_B8G8R8X8_TYPELESS           = 92,
	FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
	FORMAT_BC6H_TYPELESS               = 94,
	FORMAT_BC6H_UF16                   = 95,
	FORMAT_BC6H_SF16                   = 96,
	FORMAT_BC7_TYPELESS                = 97,
	FORMAT_BC7_UNORM                   = 98,
	FORMAT_BC7_UNORM_SRGB              = 99,
	FORMAT_AYUV                        = 100,
	FORMAT_Y410                        = 101,
	FORMAT_Y416                        = 102,
	FORMAT_NV12                        = 103,
	FORMAT_P010                        = 104,
	FORMAT_P016                        = 105,
	FORMAT_420_OPAQUE                  = 106,
	FORMAT_YUY2                        = 107,
	FORMAT_Y210                        = 108,
	FORMAT_Y216                        = 109,
	FORMAT_NV11                        = 110,
	FORMAT_AI44                        = 111,
	FORMAT_IA44                        = 112,
	FORMAT_P8                          = 113,
	FORMAT_A8P8                        = 114,
	FORMAT_B4G4R4A4_UNORM              = 115,
	FORMAT_FORCE_UINT                  = 0xffffffff
};

enum EInputClassification
{
	INPUT_PER_VERTEX_DATA	= 0,
	INPUT_PER_INSTANCE_DATA	= 1
};


enum EFilter
{	
	FILTER_MIN_MAG_MIP_POINT	= 0,
	FILTER_MIN_MAG_POINT_MIP_LINEAR	= 0x1,
	FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x4,
	FILTER_MIN_POINT_MAG_MIP_LINEAR	= 0x5,
	FILTER_MIN_LINEAR_MAG_MIP_POINT	= 0x10,
	FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x11,
	FILTER_MIN_MAG_LINEAR_MIP_POINT	= 0x14,
	FILTER_MIN_MAG_MIP_LINEAR	= 0x15,
	FILTER_ANISOTROPIC	= 0x55,
	FILTER_COMPARISON_MIN_MAG_MIP_POINT	= 0x80,
	FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR	= 0x81,
	FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x84,
	FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR	= 0x85,
	FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT	= 0x90,
	FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x91,
	FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT	= 0x94,
	FILTER_COMPARISON_MIN_MAG_MIP_LINEAR	= 0x95,
	FILTER_COMPARISON_ANISOTROPIC	= 0xd5
};

/** Layed out for D3D11 */
enum ETextureAddress
{
	TA_WRAP	= 1,
	TA_MIRROR = 2,
	TA_CLAMP = 3,
	TA_BORDER = 4,
	TA_MIRROR_ONCE = 5
};

enum EShaderType
{
	ST_VERTEX,
	ST_PIXEL,
	ST_GEOMETRY,
	ST_HULL,
	ST_DOMAIN,
	ST_NUM_SHADER_TYPES
};

enum EDrawCallType
{
	DCT_DrawTriangleList = 0,
	DCT_DrawIndexed = 1,
	DCT_DrawInstanced = 2,
	DCT_DrawIndexedInstanced = 3
};