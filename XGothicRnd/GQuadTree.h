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
			std::set<int> sectors = { GetSector(std::get<0>(t)), GetSector(std::get<1>(t)), GetSector(std::get<2>(t)) };

			// Put it in all sectors it wants. This will cause some overdraw, but it shouldn't be too bad.
			// TODO: Profile!
			for(int s : sectors)
			{
				m_SubNodes[s]->AddTriangle(t, onSuccess);

				m_BBox3D.m_Min.y = std::min(m_BBox3D.m_Min.y, m_SubNodes[s]->m_BBox3D.m_Min.y);
				m_BBox3D.m_Max.y = std::max(m_BBox3D.m_Max.y, m_SubNodes[s]->m_BBox3D.m_Max.y);
			}
		}
		else
		{
			// Down at the bottom, notify the caller
			onSuccess(this);

			// Adjust boundingbox.y
			m_BBox3D.m_Min.y = std::min(m_BBox3D.m_Min.y, std::min(std::min(std::get<0>(t).y, std::get<1>(t).y), std::get<2>(t).y));
			m_BBox3D.m_Max.y = std::max(m_BBox3D.m_Max.y, std::max(std::max(std::get<0>(t).y, std::get<1>(t).y), std::get<2>(t).y));
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
		float3 a = p - (m_BBox3D.m_Max + m_BBox3D.m_Min) * 0.5f;

		//   x
		// z 0 1
		//   2 3

		if(a.x < 0) // Left
		{
			
			if(a.z < 0) // Left/Top
			{
				return 0;
			}
			else // Left/Bottom
			{
				return 2;
			}
		}
		else // Right
		{
			if(a.z < 0) // Right/Top
			{
				return 1;
			}
			else // Right/Bottom
			{
				return 3;
			}
		}
	}

	/** Creates sub-nodes down to the specified level */
	void Subdivde(unsigned int numLevels)
	{
		m_BBoxMid = (m_BBox3D.m_Max + m_BBox3D.m_Min) * 0.5f;
		m_NodeLevel = numLevels;

		if(!numLevels)
			return;

		// 0 1
		// 2 3

		zTBBox3D subBB[4] = {	{{m_BBox3D.m_Min},{m_BBoxMid}},
								{{float3(m_BBoxMid.x,0,m_BBox3D.m_Min.z)},{float3(m_BBox3D.m_Max.x,0,m_BBoxMid.z)}},
								{{float3(m_BBox3D.m_Min.x,0,m_BBoxMid.z)},{float3(m_BBoxMid.x,0,m_BBox3D.m_Max.z)}},
								{{m_BBoxMid},{m_BBox3D.m_Max}} };

		

		for(int i = 0; i < 4; i++)
		{
			subBB[i].m_Min.y = FLT_MAX;
			subBB[i].m_Max.y = FLT_MIN;

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

	/** Executes a function at the root (=this object) of the tree. However,
		you can call WalkSubs() on this node to Walk further into the subtree. */
	void WalkTree(std::function<void(GQuadTree<T>*)> fn)
	{
		fn(this);
	}

	/** Executes the given function on all subnodes. See WalkTree() for more information */
	void WalkSubs(std::function<void(GQuadTree<T>*)> fn)
	{
		if(!IsLeaf())
			for(int i = 0; i < 4; i++)
				m_SubNodes[i]->WalkTree(fn);
	}

	/** Returns the userdata */
	T& GetData(){return m_Data;}

	/** Returns the boundingbox */
	const zTBBox3D& GetBBox(){return m_BBox3D;}

	/** Returns if this BBox was untouched, that is, nothing was added to this node */
	bool NodeEmpty()
	{
		return m_BBox3D.m_Min.y == FLT_MIN && m_BBox3D.m_Max.y == FLT_MIN;
	}

	/** Returns the midpoint of the bbox */
	const float3& GetMidpoint()
	{
		return m_BBoxMid;
	}

private:
	// Sub-nodes this node has. If one of them is zero (checking first is enough!),
	// then this is a leaf.
	// 0 1
	// 2 3
	GQuadTree<T>* m_SubNodes[4];
	zTBBox3D m_BBox3D;
	float3 m_BBoxMid;
	unsigned int m_NodeLevel;

	// Parent node. nullptr on root.
	GQuadTree<T>* m_Parent;

	// Userdata
	T m_Data;
};