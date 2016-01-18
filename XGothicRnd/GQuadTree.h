#pragma once
#include "pch.h"
#include "zDefinitions.h"

// Simple Quad-Tree structure
template<typename T>
class GQuadTree
{
public:
	GQuadTree(GQuadTree<T>* parent, const zTBBox3D& bbox)
	{
		m_BBox3D = bbox;
		memset(m_SubNodes, 0, sizeof(m_SubNodes));
		m_Parent = parent;
	}

	~GQuadTree()
	{
		for(int i = 0; i < 4; i++)
			delete m_SubNodes[i];
	}

	/** Adds a triangle to the tree. onSuccess() is called at the end with the containing node.*/
	void AddTriangle(const std::tuple<float3,float3,float3>& t, std::function<void(GQuadTree<T>*)> onSuccess)
	{
		if(!IsLeaf())
		{
			// Get sector where the most points are in
			// TODO: Very large polygons could be completely outside. That would be bad.
			std::set<int> sectors = { GetSector(std::get(0, t)), GetSector(std::get(1, t)), GetSector(std::get(2, t)) };

			// Put it in all sectors it wants. This will cause some overdraw, but it shouldn't be too bad.
			// TODO: Profile!
			for(int s : sectors)
			{
				m_SubNodes[s]->AddTriangle(t, onSuccess);
			}
		}
		else
		{
			// Down at the bottom, notify the caller
			onSuccess(this);
		}
	}

	/** Returns whether this is a leaf or not */
	bool IsLeaf()
	{
		return !m_SubNodes[0];
	}

	/** Returns the index of the sub-node the given position is in */
	int GetSector(const float3& p)
	{
		float3 a = p - (m_BBox3D.m_Max - m_BBox3D.m_Min) * 0.5f;

		
		if(a.x < 0) // Left
		{
			
			if(a.z < 0) // Left/Bottom
			{
				return 2;
			}
			else // Left/Top
			{
				return 0;
			}
		}
		else // Right
		{
			if(a.z < 0) // Right/Bottom
			{
				return 3;
			}
			else // Right/Top
			{
				return 1;
			}
		}
	}

	/** Creates sub-nodes down to the specified level */
	void Subdivde(unsigned int numLevels)
	{
		float2 midPoint = (m_BBox3D.m_Max - m_BBox3D.m_Min) * 0.5f;
		m_NodeLevel = numLevels;

		if(!numLevels)
			return;

		zTBBox3D subBB[4] = {	{{m_BBox3D.m_Min},{midPoint}},
								{{float3(midPoint.x,0,m_BBox3D.m_Min.z)},{float3(m_BBox3D.m_Max.x,0,midPoint.z)}},
								{{float3(m_BBox3D.m_Min.x,0,midPoint.z)},{float3(midPoint.x,0,m_BBox3D.m_Max.z)}},
								{{midPoint},{m_BBox3D.m_Max}} };

		for(int i = 0; i < 4; i++)
		{
			m_SubNodes[i] = new GQuadTree<T>(this, subBB[i]);
			m_SubNodes[i]->Subdivde(numLevels-1);
		}
	}

	/** Executes the given function on every sub-node */
	void MapTree(std::function<void(GQuadTree<T>*)> fn)
	{
		fn(this);

		if(!IsLeaf())
			for(int i = 0; i < 4; i++)
				m_SubNodes[i]->MapTree(fn);
	}

	/** Returns the userdata */
	T& GetData(){return m_Data;}

private:
	// Sub-nodes this node has. If one of them is zero (checking first is enough!),
	// then this is a leaf.
	// 0 1
	// 2 3
	GQuadTree<T>* m_SubNodes[4];
	zTBBox3D m_BBox3D;
	unsigned int m_NodeLevel;

	// Parent node. nullptr on root.
	GQuadTree<T>* m_Parent;

	// Userdata
	T m_Data;
};