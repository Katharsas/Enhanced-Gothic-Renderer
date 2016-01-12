#pragma once
#include "zCVisual.h"
#include "zCArray.h"
#include "VertexTypes.h"
#include "zDefinitions.h"
#include "zCMaterial.h"

struct zTPMWedge 
{
	float3 m_Normal;
	float2 m_Texcoord;
	uint16_t m_VertexIndex;
};

struct zTPMTriangle 
{
	uint16_t m_Wedges[3];
};													

struct zTPMTriangleEdges 
{
	uint16_t m_Edges[3];
};													

struct zTPMEdge 
{
	uint16_t m_Wedges[2];
};													

struct zTPMVertexUpdate 
{
	uint16_t m_NumNewTri;
	uint16_t m_NumNewWedge;
};

class zCMaterial;
class zCSubMesh 
{
public:
	zCSubMesh();
	~zCSubMesh();

	zCMaterial* m_Material;
	zCArrayAdapt<zTPMTriangle> m_TriangleList;
	zCArrayAdapt<zTPMWedge> m_WedgeList;
	zCArrayAdapt<float> m_ColorList;
	zCArrayAdapt<VERTEX_INDEX> m_TrianglePlaneIndexList;
	zCArrayAdapt<zTPlane> m_TrianglePlaneList;
	zCArrayAdapt<zTPMTriangleEdges>	m_TriEdgeList;
	zCArrayAdapt<zTPMEdge> m_EdgeList;
	zCArrayAdapt<float> m_EdgeScoreList;	
	zCArrayAdapt<VERTEX_INDEX> m_WedgeMap;
	zCArrayAdapt<zTPMVertexUpdate> m_VertexUpdates;
	int m_VbStartIndex;
};

class zCProgMeshProto : public zCVisual
{
public:

	/** Returns the submesh with the given index */
	zCSubMesh& GetSubmesh(unsigned int idx)
	{
		return m_SubMeshList[idx];
	}

	/** Returns how many submeshes this visual got */
	unsigned int GetNumSubmeshes()
	{
		return m_NumSubMeshes;
	}

	/** 
	 * Packs the vertices into our format
	 * T: Vertexformat. Should be a ExVertexStruct-Type. 
	 * I: Indexformat.
	 * vxs: vector of vertices to push to
	 * ixs: vector of indices to push to
	 * indexStart: Value to add to every index
	 * submeshIndexStarts: Index value when each submesh starts
	 */
	template<typename T, typename I>
	void PackVertices(std::vector<T>& vxs, std::vector<I>& ixs, I indexStart = (I)0, std::vector<I>& submeshIndexStarts = std::vector<I>())
	{
		for(int s=0;s<m_NumSubMeshes;s++)
		{

			zCSubMesh& sm = m_SubMeshList[s];

			unsigned int meshVxStart = vxs.size();

			// Get data
			for(int i=0;i<sm.m_WedgeList.m_NumInArray;i++)
			{
				zTPMWedge& wedge = sm.m_WedgeList.m_Array[i];

				T v;
				v.Position = m_PositionList.m_Array[wedge.m_VertexIndex];
				v.Normal = wedge.m_Normal;
				v.TexCoord = wedge.m_Texcoord;
				v.TexCoord2 = float2(0,0);
				v.Color = 0xFFFFFFFF; // TODO: Apply color from material!
				vxs.push_back(v);
			}

			// Mark when the submesh starts
			submeshIndexStarts.push_back(ixs.size());

			// And get the indices
			for(int i=0;i<sm.m_TriangleList.m_NumInArray;i++)
			{
				for(int j=2;j>=0;j--)
				{
					ixs.push_back(((I)sm.m_TriangleList.m_Array[i].m_Wedges[j]) // Take wedge-index of submesh
						+ indexStart // Add our custom offset
						+ meshVxStart); // And add the starting location of the vertices for this submesh
				}
			}		
		}
	}

protected:

	// Full list of all positions and normals for all submeshes.
	// Must be indiced through the Wedges.
	zCArrayAdapt<float3> m_PositionList;
	zCArrayAdapt<float3> m_NormalList;

	zTBBox3D m_BBox3D;
	zCOBBox3D m_obbox3D;

	class zCVertexBuffer* m_StaticVertexBuffer;

	zCSubMesh* m_SubMeshList;
	int	m_NumSubMeshes;

	byte* m_DataPool;
	DWORD m_DataSize;
	float m_AvgTriArea;
	zBOOL m_IsUsingAlphaTesting;

	struct zTLODParams {
		float m_Strength;		
		float m_ZDisplace2;		
		float m_MorphPerc;		
		int m_MinVerts;			
	};

	zTLODParams	m_LodParams;
	int	m_LightingMode;
	DWORD m_PMeshID;
	DWORD m_RenderSortKey;
};