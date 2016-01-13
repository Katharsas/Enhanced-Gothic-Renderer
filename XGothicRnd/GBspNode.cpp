#include "pch.h"
#include "GBspNode.h"
#include "zCBspTree.h"
#include <RTools.h>
#include "GMeshIndexed.h"
#include "GMaterial.h"
#include <RPipelineState.h>
#include "GConstants.h"
#include <REngine.h>
#include <RDevice.h>
#include "GTexture.h"
#include <RTexture.h>
#include "zCVob.h"
#include "zCTexture.h"
#include "GBspTree.h"
#include "zCCamera.h"
#include "GVobObject.h"
#include "zCMaterial.h"

GBspNode::GBspNode()
{
	m_SourceNode = nullptr;
	m_FrustumTestCache = -1;
}

/** Initialization */
void GBspNode::Init(zCBspBase* sourceNode, GBspTree* sourceTree)
{
	m_SourceNode = sourceNode;
	m_Front = nullptr;
	m_Back = nullptr;
	m_IsLeaf = sourceNode->m_NodeType == zTBspNodeType::zBSP_LEAF;
	m_NodeLevel = 0;
	m_BBox = sourceNode->m_BBox3D;

	if (sourceNode->m_NodeType == zTBspNodeType::zBSP_NODE)
		m_SeperationPlane = ((zCBspNode*)sourceNode)->m_Plane;

	// Construct further tree
	if(m_SourceNode->m_NodeType == zTBspNodeType::zBSP_NODE)
	{
		zCBspNode* node = (zCBspNode*)m_SourceNode;
		
		// Get front and back
		if(node->m_Front)
		{
			m_Front = sourceTree->AddBspNode(node->m_Front);

			// Collect leafs
			if(node->m_Front->m_NodeType == zTBspNodeType::zBSP_LEAF)
				m_LeafList.push_back(m_Front);
			else
				m_LeafList.insert(m_LeafList.end(), m_Front->GetLeafList().begin(), m_Front->GetLeafList().end());

			// Node level is at least this value. Don't need to do a max here.
			m_NodeLevel = m_Front->GetNodeLevel() + 1;
		}

		if(node->m_Back)
		{
			m_Back = sourceTree->AddBspNode(node->m_Back);

			// Collect leafs
			if(node->m_Back->m_NodeType == zTBspNodeType::zBSP_LEAF)
				m_LeafList.push_back(m_Back);
			else
				m_LeafList.insert(m_LeafList.end(), m_Back->GetLeafList().begin(), m_Back->GetLeafList().end());

			// Need to max this, as back-node could be higher and thus have less levels
			m_NodeLevel = std::max(m_NodeLevel, m_Back->GetNodeLevel() + 1);
		}
	}
}


GBspNode::~GBspNode(void)
{
	// Delete allocated memory from parts
	for(auto& pair : m_MeshParts)
	{
		delete pair.second.m_Mesh;
		REngine::ResourceCache->DeleteResource(pair.second.m_PipelineState);
	}
}


/** Generates the mesh-information of this and all child-nodes
	and stores it as triangle-list in the vertices-vector.*/
void GBspNode::BuildTriangleList(std::vector<ExTVertexStruct>& vertices)
{
	// If this is a leaf, generate the vertexdata right away
	if(m_SourceNode->m_NodeType == zTBspNodeType::zBSP_LEAF)
	{
		// Store them by material for now
		std::unordered_map<zCMaterial*, std::vector<ExTVertexStruct>> vertexMap;

		// Don't store the same triangles twice!
		std::set<size_t> triangleHashes;

		// Walk all polygons and convert them into more useful data
		for(unsigned int i=0;i<m_SourceNode->m_NumPolys;i++)
		{
			zCPolygon* poly = m_SourceNode->m_PolyList[i];

			if (poly->GetPolyFlags().PortalIndoorOutdoor)
				m_PortalList.push_back(poly);

			// Pack all vertices into a better datastructure
			std::vector<ExTVertexStruct> packed;

			if(poly->GetNumPolyVertices() == 3)
			{
				for(int j = poly->GetNumPolyVertices() - 1; j >= 0; j--) // Flip order, gothic has it different
				{
					ExTVertexStruct vx;
					poly->PackVertex(j, vx);
					packed.push_back(vx);
				}
			}
			else
			{
				for(int j = 0; j < poly->GetNumPolyVertices(); j++) // Conversion from fan to list already changing order
				{
					ExTVertexStruct vx;
					poly->PackVertex(j, vx);
					packed.push_back(vx);
				}

				// Copy to temp-vector. These don't happen often.
				std::vector<ExTVertexStruct> temp = packed;
				packed.clear();
				RTools::TriangleFanToList(&temp[0], temp.size(), packed);
			}

			// Now 'packed' contains a list of vertices, where 3 of them form a triangle.
			// Compute Tangent-vectors
			std::vector<ExTVertexStruct> validVertices;
			for(unsigned int i=0;i<packed.size();i+=3)
			{
				float3 tangent = RTools::CalculateTangent(packed[i].Position, packed[i].TexCoord,
														  packed[i+1].Position, packed[i+1].TexCoord,
														  packed[i+2].Position, packed[i+2].TexCoord);

				// Store in the vertices
				for(int j=0;j<3;j++)
					packed[i+j].Tangent = tangent;

				// Check if we already got this triangle. Do that by adding the hashes together, to ensure
				// We don't take equal triangles in different ordering
				size_t triHash = Toolbox::HashObject(packed[i].Position)
					+ Toolbox::HashObject(packed[i+1].Position)
					+ Toolbox::HashObject(packed[i+2].Position);

				if(triangleHashes.find(triHash) == triangleHashes.end())
				{
					triangleHashes.insert(triHash);
					// This doesn't exist, add to final list
					for(int j=0;j<3;j++)
						validVertices.push_back(packed[i+j]);
				}
			}

			


			zCMaterial* mat = poly->GetMaterial();
			WorldMeshPart& p = m_MeshParts[mat];

			// Store indices to the real vertex indices so in case they change,
			// we still can read out the real index-values for the buffer
			for(unsigned int i=0;i<validVertices.size();i++)
				p.m_IndexIndices.push_back(vertices.size() + i);

			// Put at the back of the vertices-vector
			vertices.insert(vertices.end(), validVertices.begin(), validVertices.end());

		}

	}else
	{
		// Build the triangle-lists for all subnodes, eventually reaching the leafs
		if(m_Front)
			m_Front->BuildTriangleList(vertices);

		if(m_Back)
			m_Back->BuildTriangleList(vertices);

		if(m_NodeLevel <= GEOMETRY_MAX_NODE_LEVEL)
		{
			// Get all indices out of the leafs now and put them together
			for each (GBspNode* leaf in m_LeafList)
			{
				auto& map = leaf->GetMeshesByMaterial();
				for each (auto& pair in map)
				{
					// Fetch the material and append the indices of the node
					auto& indices = m_MeshParts[pair.first].m_IndexIndices;
					indices.insert(indices.end(), pair.second.m_IndexIndices.begin(), pair.second.m_IndexIndices.end());
				}
			}
		}
	}
}

/** Should be called right after BuildTriangleLists to generate an indexed mesh for 
		all nodes, as well as initializing the buffers */
void GBspNode::GenerateIndexedMesh(const std::vector<unsigned int>& indices, 
								   RBuffer* vertexBuffer, 
								   RBufferCollection<unsigned int>* indexBufferCollection)
{
	if(m_NodeLevel <= GEOMETRY_MAX_NODE_LEVEL || m_IsLeaf)
	{
		// Get real indices to ours
		for(auto& pair : m_MeshParts)
		{
			// Get real indices
			std::vector<unsigned int> actualIndices;
			actualIndices.reserve(pair.second.m_IndexIndices.size());
			for each (unsigned int idx in pair.second.m_IndexIndices)
			{
				actualIndices.push_back(indices[idx]);
			}

			// Add data to buffer-collection and get offset
			unsigned int start = indexBufferCollection->AddData(&actualIndices[0], actualIndices.size());

			// Make a mesh from this
			pair.second.m_Mesh = new GMeshIndexed(vertexBuffer,
				indexBufferCollection->GetBuffer(),
				pair.second.m_IndexIndices.size(),
				0,
				start);

			// Get GMaterial-instance
			pair.second.m_Material = GMaterial::GetFromSource(pair.first);
		}

		// We can now create our statecache
		BuildPipelineStateCache();
	}

	// Do the same for child-nodes
	if (m_Front)m_Front->GenerateIndexedMesh(indices, vertexBuffer, indexBufferCollection);
	if (m_Back)m_Back->GenerateIndexedMesh(indices, vertexBuffer, indexBufferCollection);
}

/** (Re)builds the pipeline-state cache for this node */
void GBspNode::BuildPipelineStateCache()
{
	for(auto& pair : m_MeshParts)
	{
		UpdateMeshPartPipelineState(pair.second);
	}
}

/** Updates the pipeline-state of one meshpart */
void  GBspNode::UpdateMeshPartPipelineState(WorldMeshPart& part)
{
	// Delete old state, in case this is a recreation
	REngine::ResourceCache->DeleteResource(part.m_PipelineState);

	// Get base-state for worldmeshes. This contains objects like the needed shaders.
	RPipelineState* defState = REngine::ResourceCache->GetCachedObject<RPipelineState>(GConstants::PipelineStates::BPS_WORLDMESH);
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();

	// Assign default values
	sm.SetFromPipelineState(*defState);

	// Now ours...
	GMeshIndexed* msh = part.m_Mesh;
	sm.SetVertexBuffer(0, msh->GetMeshVertexBuffer());
	sm.SetIndexBuffer(msh->GetMeshIndexBuffer());

	if (part.m_Material->GetDiffuse())
	{
		sm.SetTexture(0, part.m_Material->GetDiffuse()->GetTexture(), EShaderType::ST_PIXEL);

		// Force a cache-in because we need to know if the texture uses alpha-testing
		part.m_Material->GetDiffuse()->CacheIn(false);

		//if (part.m_Material->GetDiffuse()->GetSourceObject()->GetTextureFlags().HasAlpha)
		//	LogInfo() << "Alphatest on: " << part.m_Material->GetDiffuse()->GetSourceObject()->GetObjectName();

		// Get the right pixelshader for the mesh-part
		RPixelShader* ps = part.m_Material->GetMaterialPixelShader(GConstants::RS_WORLD);
		sm.SetPixelShader(ps);
	}
	


	// Construct drawcall
	part.m_PipelineState = sm.MakeDrawCallIndexed(msh->GetNumIndices(), msh->GetMeshIndexStart(), 0);
}

/** Does frustumculling and draws this node if it is the lowest acceptable */
void GBspNode::DrawNodeRecursive(unsigned int lowestLevel, RRenderQueueID queue, BSPRenderInfo info, std::vector<GVobObject*>& visibleVobs)
{
	// If the BBox is inside the frustum, we can just draw the contents of this node here
	zTCam_ClipType clip = zCCamera::BBox3DInFrustumCached(m_BBox, info.FrustumPlanes, info.FrustumSignBits, m_FrustumTestCache, info.ClipFlags);
	//zTCam_ClipType clip = zCCamera::GetActiveCamera()->BBox3DInFrustum(m_BBox, info.ClipFlags);

	// Trivial out?
	if (clip == ZTCAM_CLIPTYPE_OUT)
		return;
	
	// Check if we can just draw this. Trivial in?
	if((m_NodeLevel <= GEOMETRY_MAX_NODE_LEVEL && (lowestLevel == m_NodeLevel || clip == ZTCAM_CLIPTYPE_IN)) // All subnodes visible?
		|| m_IsLeaf)
	{
		// Collect vobs while submitting the drawcall
		// TODO: Profile: Is this really faster?
		//#pragma omp parallel sections num_threads(2)
		{
		//	#pragma omp section
			DrawNodeExplicit(queue);

		//	#pragma omp section
			CollectVobs(visibleVobs, info.CameraPostion, info.ObjectFarplane);
		}
		return;
	}

	// Node is just crossing the frustum. Break it further down if possible.

	if (m_Front)m_Front->DrawNodeRecursive(lowestLevel, queue, info, visibleVobs);
	if (m_Back)m_Back->DrawNodeRecursive(lowestLevel, queue, info, visibleVobs);
	/*

	// Check which side of the node the camera is on
	// dot( (a,b,c,d), (x,y,z,1) ) > 0 where positive dot product is in front of the plane and negative is behind
	GBspNode* inside, *other;
	if (m_SeperationPlane.DotCoordinate(info.CameraPostion) > 0)
	{
		// Camera in front
		inside = m_Front;
		other = m_Back;
	}
	else 
	{
		// Camera in back
		inside = m_Back;
		other = m_Front;
	}

	// Continue with the node the camera is in
	if(inside)
		inside->DrawNodeRecursive(lowestLevel, queue, info);

	// For the other node, check if it faces away from the seperation-plane
	if (other && m_SeperationPlane.DotNormal(info.CameraDirection) > 0)
	{
		// Camera is facing the plane, draw the other node as well
		other->DrawNodeRecursive(lowestLevel, queue, info);
	}
	*/

	// TODO: Do better than this!
	/*if(m_Front)
		m_Front->DrawNodeRecursive(lowestLevel, queue);

	if(m_Back)
		m_Back->DrawNodeRecursive(lowestLevel, queue);*/
}

/** Renderlogic for this specific node */
void GBspNode::DrawNodeExplicit(RRenderQueueID queue)
{
	// Push all cached pipeline states if we have everything loaded
	for(auto& pair : m_MeshParts)
	{
		WorldMeshPart& part = pair.second;
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
}

/** Collects all vobs from the underlaying leaf-nodes */
void GBspNode::CollectVobs(std::vector<GVobObject*>& visibleVobs, const float3& cameraPosition, float objectFarplane)
{
	// TODO: This method is pretty bad at cache locality. Keep an eye on optimizing this!

	// Run until we find a leaf and put its vobs into the target vector
	if (IsLeaf())
	{
		zCBspLeaf* leaf = (zCBspLeaf*)m_SourceNode;

		unsigned int frame = REngine::RenderingDevice->GetFrameCounter();

		// Copy all vobs currently in this leaf into the vector
		for (unsigned int i = 0; i < leaf->LeafVobList.NumInArray; i++)
		{
			GVobObject* vob = leaf->LeafVobList.Array[i]->GetVobObject();
			bool isIndoor = (leaf->LeafVobList.Array[i]->GetGroundPolygon() && leaf->LeafVobList.Array[i]->GetGroundPolygon()->GetMaterial()->GetSectorFront());
			if (vob && !isIndoor && vob->UpdateObjectCollectionState(frame))
			{
				visibleVobs.push_back(vob);
			}
		}

		AddVisibleIndoorVobs(cameraPosition, visibleVobs, frame);

	}
	else
	{
		// Go through subnodes
		if(m_Front)
		{
			if(Toolbox::ComputePointAABBDistance(cameraPosition, m_Front->GetBBox().m_Min, m_Front->GetBBox().m_Max) < objectFarplane)
			{
				m_Front->CollectVobs(visibleVobs, cameraPosition, objectFarplane);
			}
		}

		if(m_Back)
		{
			if(Toolbox::ComputePointAABBDistance(cameraPosition, m_Back->GetBBox().m_Min, m_Back->GetBBox().m_Max) < objectFarplane)
			{
				m_Back->CollectVobs(visibleVobs, cameraPosition, objectFarplane);
			}
		}
	}


}

void GBspNode::AddVisibleIndoorVobs(const float3& cameraPosition, std::vector<GVobObject*>& visibleVobs, unsigned int frame)
{
	for (zCPolygon* p: m_PortalList)
	{
		// check if we are looking at the front of a portal with no front sector
		if (!p->IsBackfacing(cameraPosition) && !p->GetMaterial()->GetSectorFront())
		{
			zCBspSector* sector = p->GetMaterial()->GetSectorBack();
			if (sector)
			{
				sector->AddSectorVobsRec(cameraPosition, visibleVobs, frame, nullptr);
			}

		}
	}
}
