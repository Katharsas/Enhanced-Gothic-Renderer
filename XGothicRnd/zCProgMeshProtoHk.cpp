#include "pch.h"
#include "zCProgMeshProtoHk.h"

// Init global instance of this object
REGISTER_HOOK(zCProgMeshProtoHk);


zCProgMeshProtoHk::zCProgMeshProtoHk()
{
	// TODO: Implement this!
	//Hooks::HookDirect(MemoryLocations::Gothic::zCProgMeshProto__CheckRayPolyIntersection_zCProgMeshProto__zCSubMesh_p_int_zVEC3_const_r_zVEC3_const_r_zVEC3_r_float_r, 
	//	CheckRayPolyIntersection);
}


zCProgMeshProtoHk::~zCProgMeshProtoHk()
{
}

/** Faster implementation of this */
int __fastcall zCProgMeshProtoHk::CheckRayPolyIntersection(zCSubMesh *subMesh,
	int triIndex,
	const float3& rayOrigin,
	const float3& ray,
	float3& inters,
	float& alpha)
{

}