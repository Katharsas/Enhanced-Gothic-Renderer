#include "pch.h"
#include "GBspTree.h"
#include "GBspNode.h"
#include "zCBspTree.h"
#include <RTools.h>
#include <RBuffer.h>
#include <RBufferCollection.h>
#include "zCCamera.h"
#include "GMaterial.h"
#include "GTexture.h"
#include "GMeshIndexed.h"
#include "GMainResources.h"
#include <RTextureAtlas.h>

// Number of subdivides the quadtree should do
const int QUAD_TREE_NUM_SUBLEVELS = 3;

// Distance to the last node to draw lightmaps in
const float LIGHTMAP_MAX_RENDER_DISTANCE = 16000.0f;

GBspTree::GBspTree(zCBspTree* sourceTree) : GzObjectExtension<zCBspTree, GBspTree>(sourceTree)
{
	m_SourceTree = sourceTree;
	
	// Pre-allocate the memory for the nodes
	m_TreeNodes.resize(sourceTree->GetNumNodes());
	m_TreeNodes.clear(); // This keeps the memory allocated

	// Generate complete tree
	m_RootNode = AddBspNode(sourceTree->GetRootNode());

	std::vector<ExTVertexStruct> vertices;
	std::vector<ExTVertexStruct> verticesFull;
	std::vector<ExTVertexStruct> verticesFullIndexed;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> indicesFull;
	std::vector<ExTVertexStruct> indicedVertices;
	std::vector<zCPolygon*> trianglePolys;

	// Load all the lightmaps
	for(int i = 0; i < m_SourceTree->GetNumPolygons(); i++)
	{
		zCPolygon* p = m_SourceTree->GetPolygons()[i];

		if(p->GetLightmap() && p->GetLightmap()->GetTexture())
			p->GetLightmap()->GetTexture()->CacheIn(-1);
	}

	// Create the lightmap atlases
	Engine::Game->GetMainResources()->ConstructLightmapAtlases();

	LogInfo() << "Building node triangle-lists";
	// Generate vertex-lists for each node
	m_RootNode->BuildTriangleList(vertices, trianglePolys);

	LogInfo() << "Indexing worldmesh...";
	// Create indexed mesh
	RTools::IndexVertices<ExTVertexStruct, ExTVertexStruct, unsigned int>(&vertices[0], vertices.size(), indicedVertices, indices);
	//RTools::IndexVertices<ExTVertexStruct, ExTVertexStruct, unsigned int>(&verticesFull[0], verticesFull.size(), indicedVertices, indices);

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

	//LogInfo() << "Generating indexded meshes for BSP-Nodes...";
	// Generate indices for each node
	//m_RootNode->GenerateIndexedMesh(indices, m_WorldMeshBuffer, m_WorldIndexBuffer);

	LogInfo() << "Generating Quad-Tree...";
	m_QuadTree = new GQuadTree<QuadTreeNode>(nullptr, m_RootNode->GetBBox());
	m_QuadTree->Subdivde(QUAD_TREE_NUM_SUBLEVELS);

	LogInfo() << "Splitting World-Mesh...";
	BuildQuadTreeVertexData(indicedVertices, indices, trianglePolys);

	LogInfo() << "Constructing collected indexbuffer...";
	m_WorldIndexBuffer->Construct(EBindFlags::B_INDEXBUFFER);
}


GBspTree::~GBspTree(void)
{
	REngine::ResourceCache->DeleteResource(m_WorldMeshBuffer);
	REngine::ResourceCache->DeleteResource(m_WorldIndexBuffer);
	delete m_QuadTree;
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

/** Generates the quad-trees vertex data */
void GBspTree::BuildQuadTreeVertexData(const std::vector<ExTVertexStruct>& vertices, const std::vector<unsigned int>& indices, const std::vector<zCPolygon*>& trianglePolys)
{
	// First split the indices into the nodes
	for(int i=0;i<indices.size();i+=3)
	{
		auto tuple = std::make_tuple(vertices[indices[i]].Position, vertices[indices[i+1]].Position, vertices[indices[i+2]].Position);

		// Add indices to the nodes on success
		std::function<void(GQuadTree<QuadTreeNode>*)> suc = [&](GQuadTree<QuadTreeNode>* n)
		{
			zCPolygon* poly = trianglePolys[i / 3];

			
			RTexture* lightmap = nullptr;
			
			// Get lightmap atlas if needed
			// TODO: This should be rather slow, here, in an inner loop. Profile and optimize!
			if(poly->GetLightmap() && poly->GetLightmap()->GetTexture() && poly->GetLightmap()->GetTexture()->GetSurface())
			{
				DDSURFACEDESC2 ddsd;
				poly->GetLightmap()->GetTexture()->GetSurface()->GetSurfaceDesc(&ddsd);

				// Get lightmap from atlas, if size is even. Otherwise just use the regular lightmap
				if(ddsd.dwWidth == ddsd.dwHeight)
					lightmap = Engine::Game->GetMainResources()->GetLightmapAtlas(INT2(ddsd.dwWidth, ddsd.dwHeight))->GetTexture();
				else
					lightmap = GTexture::GetFromSource(poly->GetLightmap()->GetTexture())->GetTexture();
			}

			WorldMeshPart& p = n->GetData().m_MeshParts[std::make_pair(lightmap, poly->GetMaterial())];

			// Add indices to mesh-part
			p.m_Indices.push_back(indices[i]);
			p.m_Indices.push_back(indices[i+1]);
			p.m_Indices.push_back(indices[i+2]);
		};

		m_QuadTree->AddTriangle(tuple, suc);
	}

	// Then, generate their pipeline-states and buffer portions
	std::function<void(GQuadTree<QuadTreeNode>* n)> gen = [&](GQuadTree<QuadTreeNode>* n)
	{
		auto& map = n->GetData().m_MeshParts;

		for(auto& pair : map)
		{
			WorldMeshPart& part = pair.second;

			// Get extension-objects...
			part.m_Material = GMaterial::GetFromSource(pair.first.second);

			if(pair.first.first)
				part.m_Lightmap = pair.first.first;

			// Add data to global index buffer
			unsigned int start = m_WorldIndexBuffer->AddData(&part.m_Indices[0], part.m_Indices.size());

			// Build buffer portion
			pair.second.m_Mesh = new GMeshIndexed(m_WorldMeshBuffer,
				m_WorldIndexBuffer->GetBuffer(),
				part.m_Indices.size(),
				0,
				start);
			
			// Create actual pipeline state
			UpdateMeshPartPipelineState(part);
		}
	};

	m_QuadTree->MapTree(gen);
}

/** Updates the pipeline-state of one meshpart */
void GBspTree::UpdateMeshPartPipelineState(WorldMeshPart& part)
{
	// Delete old state, in case this is a recreation
	REngine::ResourceCache->DeleteResource(part.m_PipelineState);

	// Get base-state for worldmeshes. This contains objects like the needed shaders.
	RPipelineState* defState = REngine::ResourceCache->GetCachedObject<RPipelineState>(GConstants::PipelineStates::BPS_WORLDMESH);
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();

	// Assign default values
	sm.SetFromPipelineState(defState);

	// Now ours...
	GMeshIndexed* msh = part.m_Mesh;
	sm.SetVertexBuffer(0, msh->GetMeshVertexBuffer());
	sm.SetIndexBuffer(msh->GetMeshIndexBuffer());

	RRasterizerState* twosidedRS = REngine::ResourceCache->GetCachedObject<RRasterizerState>("twosided");
	RRasterizerState* defaultRS = REngine::ResourceCache->GetCachedObject<RRasterizerState>("default");

	if (part.m_Material->GetDiffuse())
	{
		UINT mFlags = MPS_NONE;

		sm.SetTexture(0, part.m_Material->GetDiffuse()->GetTexture(), EShaderType::ST_PIXEL);

		// Set lightmap if we have one
		if(part.m_Lightmap)
		{
			sm.SetTexture(1, part.m_Lightmap, EShaderType::ST_PIXEL);

			mFlags |= MPS_LIGHTMAPPED;
		}

		// Force a cache-in because we need to know if the texture uses alpha-testing
		// TODO: Maybe this is possible with threading. But is it even worth it?
		part.m_Material->GetDiffuse()->CacheIn(false);

		// Apply blendstates etc.
		part.m_Material->ApplyStates();

		//if (part.m_Material->GetDiffuse()->GetSourceObject()->GetTextureFlags().HasAlpha)
		//	LogInfo() << "Alphatest on: " << part.m_Material->GetDiffuse()->GetSourceObject()->GetObjectName();

		// Get the right pixelshader for the mesh-part
		RPixelShader* ps = part.m_Material->GetMaterialPixelShader(GConstants::RS_WORLD, mFlags);
		sm.SetPixelShader(ps);
	}



	// Construct drawcall
	part.m_PipelineState = sm.MakeDrawCallIndexed(msh->GetNumIndices(), msh->GetMeshIndexStart(), 0);
}

/**
 * Renders everyting inside this BSP-Tree from the current frustum
 */
void GBspTree::Draw(RRenderQueueID queue, std::vector<GVobObject*>& visibleVobs, float objectFarplane)
{
	// Extract information about the camera so we have it all on stack. Better cache locality.
	BSPRenderInfo info;
	info.CameraDirection = zCCamera::GetActiveCamera()->GetCameraDirection();
	info.CameraPostion = zCCamera::GetActiveCameraPosition();
	memcpy(info.FrustumPlanes, zCCamera::GetActiveCamera()->GetFrustumPlanes(), sizeof(info.FrustumPlanes));
	memcpy(info.FrustumSignBits, zCCamera::GetActiveCamera()->GetFrustumSignBits(), sizeof(info.FrustumSignBits));
	info.ClipFlags = CLIP_FLAGS_FULL;
	info.ObjectFarplane = objectFarplane;
	info.WorldMaxY = m_RootNode->GetBBox().m_Max.y;

	float minNodeSizeXZ = 8000.0f;
	m_RootNode->DrawNodeRecursive(minNodeSizeXZ, queue, info, visibleVobs);

	// Draw world-mesh
	DrawQuadTreeNodes(info, queue);

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

/** Draws a world-mesh-part */
void GBspTree::DrawWorldMeshPart(WorldMeshPart& part, RRenderQueueID queue)
{
	// Make sure we have textures
	if(part.m_Material->CacheTextures())
	{
		// Check if the state is valid and the texture still the same
		if(!part.m_PipelineState 
			|| part.m_PipelineState->IDs.MainTexture != part.m_Material->GetDiffuse()->GetTexture()->GetID())
		{
			// Assign new state
			UpdateMeshPartPipelineState(part);
		}

		if(part.m_PipelineState) // Catch failed states
		{
			REngine::RenderingDevice->QueuePipelineState(part.m_PipelineState, queue);
		}
	}
}

/** Draws the pipeline-states from the visible quad-tree nodes */
void GBspTree::DrawQuadTreeNodes(BSPRenderInfo& info, RRenderQueueID queue)
{
	info.ClipFlags = CLIP_FLAGS_FULL;

	// Lambda to traverse to the leafs and draw them
	std::function<void(GQuadTree<QuadTreeNode>*)> fnDraw = [&](GQuadTree<QuadTreeNode>* n)
	{
		if(n->NodeEmpty())
			return;

		if(n->IsLeaf())
		{
			//RTools::LineRenderer.AddAABBMinMax(n->GetBBox().m_Min, n->GetBBox().m_Max, float4(0,1,0,1));

			// Decide whether to use lightmapping
			/*bool lightmapping = (float2(info.CameraPostion.x,info.CameraPostion.z)
				- float2(n->GetMidpoint().x, n->GetMidpoint().z)).LengthSquared()
				< LIGHTMAP_MAX_RENDER_DISTANCE * LIGHTMAP_MAX_RENDER_DISTANCE;*/

			bool lightmapping = true;

			// Draw all parts of this node
			for(auto& pair : n->GetData().m_MeshParts)
			{
				// Only use lightmapping when close
				if(!pair.first.first || lightmapping)
					DrawWorldMeshPart(pair.second, queue);
			}
		}
		else
		{
			// Go deeper...
			n->WalkSubs(fnDraw);
		}
	};

	// Lambda to traverse the tree and clip invisible nodes
	std::function<void(GQuadTree<QuadTreeNode>*)> fnClip = [&](GQuadTree<QuadTreeNode>* n)
	{
		if(n->NodeEmpty())
			return;

		// Check if this node is inside the bounding frustum
		//zTCam_ClipType clip = zCCamera::GetActiveCamera()->BBox3DInFrustum(n->GetBBox(), info.ClipFlags);
		zTCam_ClipType clip = zCCamera::BBox3DInFrustumCached(n->GetBBox(), info.FrustumPlanes, info.FrustumSignBits, n->GetData().m_FrustumTestCache, info.ClipFlags);

		// Gothics BBox3DInFrustum seems to mess up the farplane
		if(Toolbox::ComputePointAABBDistance(info.CameraPostion, n->GetBBox().m_Min, n->GetBBox().m_Max) > info.ObjectFarplane)
			return;

		if(clip == ZTCAM_CLIPTYPE_OUT)
		{
			//RTools::LineRenderer.AddAABBMinMax(n->GetBBox().m_Min, n->GetBBox().m_Max, float4(1,0,0,1));
			return; // Don't go futher into the tree, entire node is invisible
		}
		
		if(clip == ZTCAM_CLIPTYPE_IN || n->IsLeaf())
			n->WalkTree(fnDraw); // Draw everything in here and below, if the full node is inside or we are at the bottom
		else
			n->WalkSubs(fnClip); // If only crossing, go deeper and try to clip more nodes
	};

	std::function<void(GQuadTree<QuadTreeNode>*)> fnDebug = [&](GQuadTree<QuadTreeNode>* n)
	{
		if(n->NodeEmpty())
			return;

		RTools::LineRenderer.AddAABBMinMax(n->GetBBox().m_Min, n->GetBBox().m_Max, float4(1,0,0,1));

		n->WalkSubs(fnDebug);
	};

	//m_QuadTree->WalkTree(fnDraw);
	//m_QuadTree->WalkTree(fnDebug);
	m_QuadTree->WalkTree(fnClip);
}