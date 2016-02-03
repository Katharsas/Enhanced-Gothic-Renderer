#pragma once
#include "pch.h"
#include "RBaseInputLayout.h"

/** Structure to push line-vertices to the LineRenderer */
struct LineVertex
{
	static const INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[2]; 

	LineVertex(){}
	LineVertex(const float3& position, DWORD color = 0xFFFFFFFF)
	{
		Position = float4(position.x, position.y, position.z, 1.0f);
		Color = float4::FromColor(color);
	}

	LineVertex(const float3& position, const float4& color, float zScale = 1.0f)
	{
		Position = float4(position.x, position.y, position.z, 1.0f);
		Position.w = zScale;
		Color = color;
	}


	float4 Position;
	float4 Color;
};

__declspec(selectany) const INPUT_ELEMENT_DESC LineVertex::INPUT_LAYOUT_DESC[2] =
{
	{ "POSITION", 0, FORMAT_R32G32B32A32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	{ "DIFFUSE", 0, FORMAT_R32G32B32A32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
};


class RBuffer;
struct RPipelineState;
class RLineRenderer
{
public:
	RLineRenderer(void);
	virtual ~RLineRenderer(void);

	/** Adds a line to the list */
	bool AddLine(const LineVertex& v1, const LineVertex& v2);

	/** Flushes the cached lines. Should be called only once per frame! */
	bool Flush(const Matrix& viewProj);

	/** Clears the line cache */
	bool ClearCache();

	/** Adds a point locator to the renderlist */
	void AddPointLocator(const float3& location, float size=1, const float4& color = float4(1,1,1,1));

	/** Adds a plane to the renderlist */
	void AddPlane(const float4& plane, const float3& origin, float size=1, const float4& color = float4(1,1,1,1));

	/** Adds a ring to the renderlist */
	void AddRingZ(const float3& location, float size=1.0f, const float4& color = float4(1,1,1,1), int res=32);

	/** Adds an AABB-Box to the renderlist */
	void AddAABB(const float3& location, float halfSize, const float4& color = float4(1,1,1,1));
	void AddAABB(const float3& location, const float3& halfSize, const float4& color = float4(1,1,1,1));
	void AddAABBMinMax(const float3& min, const float3& max, const float4& color = float4(1,1,1,1));

	/** Adds a triangle to the renderlist */
	void AddTriangle(const float3& t0, const float3& t1, const float3& t2, const float4& color = float4(1,1,1,1));

	/** Plots a vector of floats */
	void PlotNumbers(const std::vector<float>& values, const float3& location, const float3& direction, float distance, float heightScale, const float4& color = float4(1,1,1,1));

protected:

	struct LineConstantBuffer
	{
		Matrix M_ViewProj;
	};

	/** Initializes the buffers and states */
	bool InitResources();

	// Cache since last flush
	std::vector<LineVertex> LineCache;

	// State to draw the line with
	RPipelineState* LinePipelineState;

	// Dynamic buffer for storing the line information
	RBuffer* LineBuffer;

	// Constantbuffer for the matrices
	RBuffer* LineCB;

	// The frame when the last call to flush() happened
	unsigned int LastFrameFlushed;
};

