#include "pch.h"
#include "GBspTree.h"
#include "GBspNode.h"
#include "zCBspTree.h"
#include <RTools.h>
#include <RBuffer.h>
#include <RBufferCollection.h>
#include "zCCamera.h"

GBspTree::GBspTree(zCBspTree* sourceTree) : GzObjectExtension<zCBspTree, GBspTree>(sourceTree)
{
	m_SourceTree = sourceTree;

	// Pre-allocate the memory for the nodes
	m_TreeNodes.resize(sourceTree->GetNumNodes());
	m_TreeNodes.clear(); // This keeps the memory allocated

	// Generate complete tree
	m_RootNode = AddBspNode(sourceTree->GetRootNode());

	std::vector<ExTVertexStruct> vertices;
	std::vector<unsigned int> indices;
	std::vector<ExTVertexStruct> indicedVertices;


	LogInfo() << "Building node triangle-lists";
	// Generate vertex-lists for each node
	m_RootNode->BuildTriangleList(vertices);

	LogInfo() << "Indexing worldmesh...";
	// Create indexed mesh
	RTools::IndexVertices<ExTVertexStruct, ExTVertexStruct, unsigned int>(&vertices[0], vertices.size(), indicedVertices, indices);

	// Create buffers
	m_WorldMeshBuffer = REngine::ResourceCache->CreateResource<RBuffer>();
	m_WorldIndexBuffer = REngine::ResourceCache->CreateResource<RBufferCollection<unsigned int>>();

	LogInfo() << "Moving meshdata to the GPU...";
	// Copy vertices to the GPU
	m_WorldMeshBuffer->Init(&indicedVertices[0], 
		indicedVertices.size() * sizeof(ExTVertexStruct), 
		sizeof(ExTVertexStruct),
		EBindFlags::B_VERTEXBUFFER,
		EUsageFlags::U_IMMUTABLE,
		ECPUAccessFlags::CA_NONE,
		"WorldMesh");

	LogInfo() << "Generating indexded meshes for BSP-Nodes...";
	// Generate indices for each node
	m_RootNode->GenerateIndexedMesh(indices, m_WorldMeshBuffer, m_WorldIndexBuffer);

	LogInfo() << "Constructing collected indexbuffer...";
	m_WorldIndexBuffer->Construct(EBindFlags::B_INDEXBUFFER);
}


GBspTree::~GBspTree(void)
{
	REngine::ResourceCache->DeleteResource(m_WorldMeshBuffer);
	REngine::ResourceCache->DeleteResource(m_WorldIndexBuffer);
}

/**
* Creates a new BSP-Node
*/
GBspNode* GBspTree::AddBspNode(zCBspBase* source)
{
	m_TreeNodes.resize(m_TreeNodes.size()+1);
	GBspNode* node = &m_TreeNodes.back();

	node->Init(source, this);
	
	return node;
}

/**
 * Renders everyting inside this BSP-Tree from the current frustum
 */
void GBspTree::Draw(RRenderQueueID queue, std::vector<GVobObject*>& visibleVobs, float objectFarplane)
{
	// Extract information about the camera so we have it all on stack. Better cache locality.
	GBspNode::BSPRenderInfo info;
	info.CameraDirection = zCCamera::GetActiveCamera()->GetCameraDirection();
	info.CameraPostion = zCCamera::GetActiveCameraPosition();
	memcpy(info.FrustumPlanes, zCCamera::GetActiveCamera()->GetFrustumPlanes(), sizeof(info.FrustumPlanes));
	memcpy(info.FrustumSignBits, zCCamera::GetActiveCamera()->GetFrustumSignBits(), sizeof(info.FrustumSignBits));
	info.ClipFlags = CLIP_FLAGS_FULL;
	info.ObjectFarplane = objectFarplane;

	m_RootNode->DrawNodeRecursive(std::max(0U, GEOMETRY_MAX_NODE_LEVEL - 4), queue, info, visibleVobs);

	// DEBUG
	zCBspSector* startSector = m_SourceTree->GetCurrentCameraSector();

	// Draw everything connected to the sector we are currently inside of
	if(startSector)
	{
		startSector->AddSectorVobsRec(info.CameraPostion, visibleVobs, REngine::RenderingDevice->GetFrameCounter(), nullptr);
		Engine::Game->AddFrameDebugLine("SECTOR: " + std::string(startSector->m_SectorName.ToChar()) + " #" + std::to_string(startSector->m_SectorIndex));
	}

	Engine::Game->AddFrameDebugLine(std::to_string(visibleVobs.size()) + " Vobs drawn");

}