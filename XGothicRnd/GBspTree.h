#pragma once
#include "GzObjectExtension.h"
#include "GQuadTree.h"
#include "GMeshIndexed.h"
#include "VertexTypes.h"

/**
 * Additional data for the usual zCBspTree
 */
class zCBspTree;
struct zCBspBase;
class GBspNode;
class GVobObject;
class GTexture;
class GMaterial;

class zCLightmap;
class zCMaterial;
class zCPolygon;

namespace RAPI
{
	template<typename T>
	class RBufferCollection;
	struct RPipelineState;
	class RTextureAtlas;
}

class GBspTree : public GzObjectExtension<zCBspTree, GBspTree>
{
public:
	struct BSPRenderInfo
	{
		float3 CameraPostion;
		float3 CameraDirection;
		zTPlane FrustumPlanes[6];
		byte FrustumSignBits[6];
		int ClipFlags;
		float ObjectFarplane;
		float WorldMaxY;
	};

	struct WorldMeshPart
	{
		WorldMeshPart()
		{
			m_Mesh = nullptr;
			m_Material = nullptr;
			m_PipelineState = nullptr;
			m_Lightmap = nullptr;
		}

		~WorldMeshPart()
		{
			// Delete allocated memory from part
			delete m_Mesh;
			RAPI::REngine::ResourceCache->DeleteResource(m_PipelineState);		
		}

		GMeshIndexed* m_Mesh;
		GMaterial* m_Material;
		RAPI::RTexture* m_Lightmap;
		RAPI::RPipelineState* m_PipelineState;

		// Indices of the full mesh
		std::vector<unsigned int> m_Indices;
	};

	// Single node of the worldmesh-quadtree
	struct QuadTreeNode
	{
		QuadTreeNode()
		{
			m_FrustumTestCache = -1;
		}

		// Map of lightmaps, material and vertex-data
		std::map<std::pair<RAPI::RTexture*, zCMaterial*>, WorldMeshPart> m_MeshParts;
		int m_FrustumTestCache;
	};

	GBspTree(zCBspTree* sourceTree);
	~GBspTree(void);

	/**
	 * Renders everyting inside this BSP-Tree from the current frustum
	 */
	void Draw(RAPI::RRenderQueueID queue, std::vector<GVobObject*>& visibleVobs, float objectFarplane);

	/**
	 * Creates a new BSP-Node
	 */
	GBspNode* AddBspNode(zCBspBase* source);

	/**
	 * Returns the world this tree contains 
	 */
	GWorld* GetContainedWorld(){return m_ContainedWorld;}
	void SetContainedWorld(GWorld* world){m_ContainedWorld = world;}

private:
	/** Draws the pipeline-states from the visible quad-tree nodes (The world mesh) */
	void DrawQuadTreeNodes(BSPRenderInfo& info, RAPI::RRenderQueueID queue);

	/** Generates the quad-trees vertex data */
	void BuildQuadTreeVertexData(const std::vector<ExTVertexStruct>& vertices, const std::vector<unsigned int>& indices, const std::vector<zCPolygon*>& trianglePolys);
	
	/** Updates the pipeline-state of one meshpart */
	void UpdateMeshPartPipelineState(WorldMeshPart& part);

	/** Draws a world-mesh-part */
	void DrawWorldMeshPart(WorldMeshPart& part, RAPI::RRenderQueueID queue);

	// My own quad-tree for rendering the game.
	// Gothics own BSP-Tree is too much of a mess.
	GQuadTree<QuadTreeNode>* m_QuadTree;

	// Root-node
	GBspNode* m_RootNode;

	// Original tree
	zCBspTree* m_SourceTree;

	// All BSP-Nodes of this tree as single block of memory
	// to help with caching
	std::vector<GBspNode> m_TreeNodes;

	// World mesh stored in a buffer
	RAPI::RBuffer* m_WorldMeshBuffer;
	RAPI::RBufferCollection<unsigned int>* m_WorldIndexBuffer;

	// World this tree contains
	GWorld* m_ContainedWorld;
};

