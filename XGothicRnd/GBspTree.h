#pragma once
#include "GzObjectExtension.h"

/**
 * Additional data for the usual zCBspTree
 */
class zCBspTree;
struct zCBspBase;
class GBspNode;
class GVobObject;

template<typename T>
class RBufferCollection;
class GBspTree
{
public:
	GBspTree(zCBspTree* sourceTree);
	~GBspTree(void);

	/**
	 * Renders everyting inside this BSP-Tree from the current frustum
	 */
	void Draw(RRenderQueueID queue, std::vector<GVobObject*>& visibleVobs, float objectFarplane);

	/**
	 * Creates a new BSP-Node
	 */
	GBspNode* AddBspNode(zCBspBase* source);
private:

	// Root-node
	GBspNode* m_RootNode;

	// Original tree
	zCBspTree* m_SourceTree;

	// All BSP-Nodes of this tree as single block of memory
	// to help with caching
	std::vector<GBspNode> m_TreeNodes;

	// World mesh stored in a buffer
	RBuffer* m_WorldMeshBuffer;
	RBufferCollection<unsigned int>* m_WorldIndexBuffer;
};

