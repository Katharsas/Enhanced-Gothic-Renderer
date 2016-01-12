#pragma once
#include "pch.h"
#include "zCProgMeshProto.h"
#include "zCArray.h"

#pragma pack (push, 1)
struct zTWeightEntry 
{
	// Weight and position of the vertex.
	// This vertexposition is in the local space of the joint-matrix!
	float m_Weight;
	float3 m_LocalVertexPosition;

	// Nodeindex this belongs to
	unsigned char m_NodeIndex;
};
#pragma pack (pop)

class zCOBBox3D;
class zCMeshSoftSkin : public zCProgMeshProto
{
public:

	// The normal-vector to a triangle
	struct zTNodeWedgeNormal 
	{
		float3 m_Normal;

		// Nodeindex this belongs to
		int	m_NodeIndex;
	};

	/** Returns a block of data which is made up like the following:
	 *	For each vertex:
	 *	 4 bytes (int) -> Num of zTWeightEntrys in the stream
	 *	 sizeof(zTWeightEntry) * n -> The actual zTWeightEntries. 
	 */
	byte* GetVertWeightStream()
	{
		return m_VertexWeightStream;
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
	void PackVerticesAndWeights(std::vector<T>& vxs, std::vector<I>& ixs, I indexStart = (I)0, std::vector<I>& submeshIndexStarts = std::vector<I>())
	{
		std::vector<T> weightedVxList;

		// Collect weights and local vertexpositions
		// We need to put this into it's own vector, since
		// these information need to be shared among different
		// vertices using the same position
		PackWeights(weightedVxList, (unsigned int)indexStart);

		// TODO: This is almost exactly the same code as in
		// 'PackVertices'. Refractor!
		for (int s = 0; s<m_NumSubMeshes; s++)
		{

			zCSubMesh& sm = m_SubMeshList[s];

			unsigned int meshVxStart = vxs.size();

			// Get data
			for (int i = 0; i<sm.m_WedgeList.m_NumInArray; i++)
			{
				zTPMWedge& wedge = sm.m_WedgeList.m_Array[i];

				T v = weightedVxList[wedge.m_VertexIndex];
				v.Position = m_PositionList.m_Array[wedge.m_VertexIndex];
				v.Normal = wedge.m_Normal;
				v.TexCoord = wedge.m_Texcoord;
				v.TexCoord2 = float2(0, 0);
				v.Color = 0xFFFFFFFF; // TODO: Apply color from material!
				vxs.push_back(v);
			}

			// Mark when the submesh starts
			submeshIndexStarts.push_back(ixs.size());

			// And get the indices
			for (int i = 0; i<sm.m_TriangleList.m_NumInArray; i++)
			{
				for (int j = 2; j >= 0; j--)
				{
					ixs.push_back(((I)sm.m_TriangleList.m_Array[i].m_Wedges[j]) // Take wedge-index of submesh
						+ indexStart // Add our custom offset
						+ meshVxStart); // And add the starting location of the vertices for this submesh
				}
			}
		}
	}

private:
	/** Packs the weights and local vertexpositions into the given vector of vertices.
	T should be a ExTSkelVertexStruct-type. */
	template <typename T>
	void PackWeights(std::vector<T>& vxs, unsigned int start)
	{
		byte* stream = m_VertexWeightStream;

		vxs.resize(m_PositionList.m_NumInArray);

		// Get weights for each vertex
		for (int i = 0; i < m_PositionList.m_NumInArray; i++)
		{
			// Read amount of weights we have for this vertex
			unsigned int numWeights = *(unsigned int*)stream;
			stream += 4;

			for (unsigned int j = 0; j < numWeights; j++)
			{
				zTWeightEntry* e = (zTWeightEntry*)stream;
				stream += sizeof(zTWeightEntry);

				// Move data to our structure
				vxs[start + i].BoneIndices[j] = e->m_NodeIndex;
				vxs[start + i].LocalPositions[j] = e->m_LocalVertexPosition;
				vxs[start + i].Weights[j] = e->m_Weight;
			}
		}
	}


	zCArray<int> m_NodeIndexList;
	zCArray<zCOBBox3D*> m_NodeOBBList;
	zCArray<zTNodeWedgeNormal> m_NodeWedgeNormalList;

	// Pointer to a block of data containing the vertex weights
	byte* m_VertexWeightStream;
	byte data1[4];
};