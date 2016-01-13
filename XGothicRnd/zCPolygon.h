#pragma once
#include "pch.h"
#include "zDefinitions.h"
#include "VertexTypes.h"
#include "zEngineFunctions.h"

class zCTexture;
class zCMaterial;
class zCLightmap;
class zCPolygon
{
public:
	#pragma pack (push, 1)	
#if GAME_VERSION == VERSION_2_6_FIX
struct PolyFlags {
	unsigned char PortalPoly			: 2;
	unsigned char Occluder				: 1;
	unsigned char SectorPoly			: 1;
	unsigned char MustRelight			: 1;
	unsigned char PortalIndoorOutdoor	: 1;	
	unsigned char GhostOccluder			: 1;
	unsigned char NoDynLightNear		: 1;	
	zWORD SectorIndex					: 16;
};

#elif GAME_VERSION == VERSION_1_8K_MOD
struct PolyFlags 
{
		unsigned char PortalPoly : 2;	
		unsigned char Occluder : 1;		
		unsigned char SectorPoly : 1;		
		unsigned char LodFlag : 1;		
		unsigned char PortalIndoorOutdoor : 1;	
		unsigned char GhostOccluder : 1;		
		unsigned char NormalMainAxis : 2;	
		zWORD SectorIndex : 16;
};
#endif
#pragma pack (pop)

	/** Array of vertices this polygon references */
	zCVertex** GetVertices() const
	{
		return m_VertexArray;
	}

	/** Array of features for the vertices with the same index. Contains values like texturecoords */
	zCVertFeature** GetFeatures() const
	{
		return m_FeatureArray;
	}
	
	/** Returns the vertex at the given index packed into an ExTVertexStruct */
	void PackVertex(unsigned char idx, ExVertexStruct& vx)
	{
		vx.Position = GetVertices()[idx]->m_Position;
		vx.Normal = GetFeatures()[idx]->m_Normal;
		vx.Color = GetFeatures()[idx]->m_LightStatic;
		vx.TexCoord = GetFeatures()[idx]->m_TexCoord;
		vx.TexCoord2 = float2(0,0); // TODO: zCLightMap::GetLightmapTexCoords
	}

	/** Size of the vertices-array */
	unsigned char GetNumPolyVertices() const
	{
		return m_PolyNumVert;
	}

	/** Flags about this polygon */
	PolyFlags& GetPolyFlags()
	{
		return m_PolyFlags;
	}

	/** Returns the material this uses */
	zCMaterial* GetMaterial()
	{
		return m_Material;
	}

	/** Set the material of the polygon */
	void SetMaterial(zCMaterial* material)
	{
		m_Material = material;
	}

	/** Returns the lightmap of this polygon */
	zCLightmap* GetLightmap() const
	{
		return m_Lightmap;
	}

	/** Sets the lightmap of this polygon */
	void SetLightmap(zCLightmap* lightmap)
	{
		m_Lightmap = lightmap;
	}

	/** Ray intersection */
	zBOOL __fastcall CheckRayPolyIntersection(const float3& rayOrigin, const float3& ray, float3& inters, float& alpha) const
	{
		XCALL(MemoryLocations::Gothic::zCPolygon__CheckRayPolyIntersection_zVEC3_const_r_zVEC3_const_r_zVEC3_r_float_r);
	}
	
	/** Returns the static lighting under the given position */
	float3 GetLightStatAtPos(const float3& pos)
	{
		// Can't sample from lightmap, so just NULL it
		// TODO: Don't forget this, if I ever implement lightmapping!
		SetLightmap(nullptr);
		return __GetLightStatAtPos(pos);
	}

	bool IsBackfacing(const float3& camPosition) const
	{
		return (camPosition.Dot(m_PolyPlane.m_Normal) <= m_PolyPlane.m_Distance);
	}

private:
	float3 __GetLightStatAtPos(const float3& pos)
	{
		XCALL(MemoryLocations::Gothic::zCPolygon__GetLightStatAtPos_zVEC3_const_r);
	}

	zCVertex** m_VertexArray;
	unsigned int m_LastTimeDrawn;
	zTPlane	m_PolyPlane;
	zCMaterial* m_Material;
	zCLightmap* m_Lightmap;

	// Don't know what this is...
	char m_pad1[12];

	zCVertFeature** m_FeatureArray;
	unsigned char m_PolyNumVert;

	PolyFlags m_PolyFlags;
};