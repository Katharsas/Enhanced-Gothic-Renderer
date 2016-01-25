#pragma once
#include "pch.h"
#include "ihookable.h"

class zCSubMesh;
class zCProgMeshProtoHk :
	public IHookable
{
public:
	zCProgMeshProtoHk();
	~zCProgMeshProtoHk();

	/** Faster implementation of this */
	static int __fastcall CheckRayPolyIntersection(zCSubMesh *subMesh,
		int triIndex,
		const float3& rayOrigin,
		const float3& ray,
		float3& inters,
		float& alpha);
};

