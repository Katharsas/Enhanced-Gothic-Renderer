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
class GBspTree : public GzObjectExtension<zCBspTree, GBspTree>
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

	/**
	 * Returns the world this tree contains 
	 */
	GWorld* GetContainedWorld(){return m_ContainedWorld;}
	void SetContainedWorld(GWorld* world){m_ContainedWorld = world;}

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

	// World this tree contains
	GWorld* m_ContainedWorld;
};

