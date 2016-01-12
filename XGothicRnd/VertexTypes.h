#pragma once
#include "pch.h"
#include <RInputLayout.h>

typedef unsigned short VERTEX_INDEX;
typedef unsigned short VertexIndex;

#define GOTHIC_FVF_XYZ_DIF_T1 (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define GOTHIC_FVF_XYZ_DIF_T1_SIZE ((3+1+2)*4)

#define GOTHIC_FVF_XYZ_NRM_T1 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define GOTHIC_FVF_XYZ_NRM_T1_SIZE ((3 + 3 + 2) * 4)

#define GOTHIC_FVF_XYZ_NRM_DIF_T2 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2)
#define GOTHIC_FVF_XYZ_NRM_DIF_T2_SIZE ((3 + 3 + 1 + 4) * 4)

#define GOTHIC_FVF_XYZ_DIF_T2 (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2)
#define GOTHIC_FVF_XYZ_DIF_T2_SIZE ((3 + 1 + 4) * 4)

#define GOTHIC_FVF_XYZRHW_DIF_T1 (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define GOTHIC_FVF_XYZRHW_DIF_T1_SIZE ((4 + 1 + 2)*4)

#define GOTHIC_FVF_XYZRHW_DIF_SPEC_T1 (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
#define GOTHIC_FVF_XYZRHW_DIF_SPEC_T1_SIZE ((4 + 1 + 1 + 2)*4)

/** We pack most of Gothics FVF-formats into this vertex-struct */
struct ExVertexStruct
{
	static const INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[5]; 

	float3 Position;
	float3 Normal;
	float2 TexCoord;
	float2 TexCoord2;
	DWORD Color;

	// Comparison for this vertex-struct
	bool operator() (const std::pair<ExVertexStruct, int>& p1, const std::pair<ExVertexStruct, int>& p2) const
    {
		const float eps = 0.0001f;
        if (fabs(p1.first.Position.x-p2.first.Position.x) > eps) return p1.first.Position.x < p2.first.Position.x;
        if (fabs(p1.first.Position.y-p2.first.Position.y) > eps) return p1.first.Position.y < p2.first.Position.y;
        if (fabs(p1.first.Position.z-p2.first.Position.z) > eps) return p1.first.Position.z < p2.first.Position.z;

		if (fabs(p1.first.TexCoord.x-p2.first.TexCoord.x) > eps) return p1.first.TexCoord.x < p2.first.TexCoord.x;
		if (fabs(p1.first.TexCoord.y-p2.first.TexCoord.y) > eps) return p1.first.TexCoord.y < p2.first.TexCoord.y;

        return false;
    }

	static void FoundDuplicate(ExVertexStruct& toStay, const ExVertexStruct& toRemove)
	{

	}
};
__declspec(selectany) const INPUT_ELEMENT_DESC ExVertexStruct::INPUT_LAYOUT_DESC[5] =
{
	{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "DIFFUSE", 0, FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
};


/** ExVertexStruct with additional tangent information */
struct ExTVertexStruct : public ExVertexStruct
{
	static const INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[6]; 

	float3 Tangent; 

	static void FoundDuplicate(ExTVertexStruct& toStay, const ExTVertexStruct& toRemove)
	{

	}
};

__declspec(selectany) const INPUT_ELEMENT_DESC ExTVertexStruct::INPUT_LAYOUT_DESC[6] =
{
	{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "DIFFUSE", 0, FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
};

struct ExTVertexStructInstanced : public ExTVertexStruct
{
	static const INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[9]; 
};

__declspec(selectany) const INPUT_ELEMENT_DESC ExTVertexStructInstanced::INPUT_LAYOUT_DESC[9] =
{
	{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "DIFFUSE", 0, FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "INSTANCE_POSITION", 0, FORMAT_R32G32B32_FLOAT, 1, 0xFFFFFFFF, INPUT_PER_INSTANCE_DATA, 1},
	{ "INSTANCE_COLOR", 0, FORMAT_R8G8B8A8_UNORM, 1, 0xFFFFFFFF, INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCE_ROTATION", 0, FORMAT_R32G32B32A32_FLOAT, 1, 0xFFFFFFFF, INPUT_PER_INSTANCE_DATA, 1 },
};

struct ExTSkelVertexStruct : public ExTVertexStruct
{
	static const INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[15];

	float3 LocalPositions[4];
	unsigned char BoneIndices[4];
	float Weights[4];
};

__declspec(selectany) const INPUT_ELEMENT_DESC ExTSkelVertexStruct::INPUT_LAYOUT_DESC[15] =
{
	{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "DIFFUSE", 0, FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "LOCALPOSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "LOCALPOSITION", 1, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "LOCALPOSITION", 2, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "LOCALPOSITION", 3, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "BONEIDS", 0, FORMAT_R8G8B8A8_UINT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHTS", 0, FORMAT_R32G32B32A32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "INSTANCE_POSITION", 0, FORMAT_R32G32B32_FLOAT, 1, 0xFFFFFFFF, INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCE_COLOR", 0, FORMAT_R8G8B8A8_UNORM, 1, 0xFFFFFFFF, INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCE_ROTATION", 0, FORMAT_R32G32B32A32_FLOAT, 1, 0xFFFFFFFF, INPUT_PER_INSTANCE_DATA, 1 },
};

struct SimpleObjectVertexStruct
{
	float3 Position;
	float2 TexCoord;
};

struct ObjVertexStruct
{
	float3 Position;
	float3 Normal;
	float2 TexCoord;
};

struct BasicVertexStruct
{
	float3 Position;
};

struct ExSkelVertexStruct
{
	float3 Position[4];
	float3 Normal;
	float2 TexCoord;
	DWORD Color;
	unsigned char boneIndices[4];
	float weights[4];
};

struct Gothic_XYZ_DIF_T1_Vertex
{
	float3 xyz;
	DWORD color;
	float2 texCoord;
};

struct Gothic_XYZRHW_DIF_T1_Vertex
{
	float3 xyz;
	float rhw;
	DWORD color;
	float2 texCoord;
};


struct Gothic_XYZRHW_DIF_SPEC_T1_Vertex
{
	float3 xyz;
	float rhw;
	DWORD color;
	DWORD spec;
	float2 texCoord;
};

struct Gothic_XYZ_NRM_T1_Vertex
{
	float3 xyz;
	float3 nrm;
	float2 texCoord;
};
