#pragma once
#include "zCBspTree.h"
#include "VertexTypes.h"
#include "GBspTree.h"

// Only generate geometry-cache for the levels from this to zero.
const unsigned int GEOMETRY_MAX_NODE_LEVEL = 5;

class GVobObject;
class GBspTree;
class GMeshIndexed;
struct zCBspBase;
class GMaterial;
class zCMaterial;
struct RPipelineState;
class RBuffer;
template <typename T>
class RBufferCollection;
class GBspNode
{
public:
	GBspNode();
	~GBspNode(void);

	/** Initialization */
	void Init(zCBspBase* sourceNode, GBspTree* sourceTree);

	/** Does frustumculling and draws this node if it is the lowest acceptable */
	void DrawNodeRecursive(float minNodeSizeXZ, RAPI::RRenderQueueID queue, GBspTree::BSPRenderInfo info, std::vector<GVobObject*>& visibleVobs);

	/** Generates the mesh-information of this and all child-nodes.
		For each triangle (That is, 3 vertices), the polygon is put into the given vector. */
	void BuildTriangleList(std::vector<ExTVertexStruct>& vertices, std::vector<zCPolygon*>& polygons);

	/** Should be called right after BuildTriangleLists to generate an indexed mesh for 
		all nodes, as well as initializing the buffers */
	void GenerateIndexedMesh(const std::vector<unsigned int>& indices, RAPI::RBuffer* vertexBuffer, RAPI::RBufferCollection<unsigned int>* indexBufferCollection);

	/** (Re)builds the pipeline-state cache for this node */
	void BuildPipelineStateCache();

	/** Returns all leafs this node has */
	const std::vector<GBspNode*>& GetLeafList() const
	{
		return m_LeafList;
	}

	/** Returns whether this is a leaf or not */
	bool IsLeaf()
	{
		return m_IsLeaf;
	}

	/** Returns number of nodes below this one */
	unsigned int GetNodeLevel(){return m_NodeLevel;}

	/** Returns the worldspace BBox of this node */
	const zTBBox3D& GetBBox(){return m_BBox;}

private:

	/** Collects all vobs from the underlaying leaf-nodes */
	void CollectVobs(std::vector<GVobObject*>& visibleVobs, const float3& cameraPosition, float objectFarplane);

	/** Draws all Nodes related to a visible Portal */
	void AddVisibleIndoorVobs(const float3& cameraPosition, std::vector<GVobObject*>& visibleVobs, unsigned int frame);

	// Original node
	zCBspBase* m_SourceNode;
	GBspNode* m_Front;
	GBspNode* m_Back;
	Plane m_SeperationPlane;
	zTBBox3D m_BBox;
	float m_NodeSizeXZ;

	//List of all Portals
	std::vector<zCPolygon*> m_PortalList;

	// Whether this node is a leaf or not
	bool m_IsLeaf;

	// Last frustum-plane this faild the test against. -1 if visible last frame
	int m_FrustumTestCache;

	// List of all further leafs
	std::vector<GBspNode*> m_LeafList;

	// Number of nodes below this one
	unsigned int m_NodeLevel;
};

