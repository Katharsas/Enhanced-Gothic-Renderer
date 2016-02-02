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
#include <RTexture.h>
#include <RTextureAtlas.h>
#include "Engine.h"
#include "GGame.h"
#include "GMainResources.h"

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

	// Calculate ground-size of this node. Leafs will not cover the world an the Y-Axis
	m_NodeSizeXZ = (float2(m_BBox.m_Min.x, m_BBox.m_Min.z) - float2(m_BBox.m_Max.x, m_BBox.m_Max.z)).Length();

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

}


/** Generates the mesh-information of this and all child-nodes
	and stores it as triangle-list in the vertices-vector.*/
void GBspNode::BuildTriangleList(std::vector<ExTVertexStruct>& vertices, std::vector<zCPolygon*>& polygons)
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

			// Set some magic number to note if we already collected this...
			if(poly->GetLastTimeDrawn() == 12345)
				continue;
			
			poly->SetLastTimeDrawn(12345);

			// Check for lightmap and the it's atlas if present
			RTextureAtlas* atlas = nullptr;
			std::pair<float2, float2> lightmapUVMod;

			// Make sure the lightmap is loaded...
			if(poly->GetLightmap() && poly->GetLightmap()->GetTexture())
				poly->GetLightmap()->GetTexture()->CacheIn(-1);

			// Make sure the texture is loaded
			if(poly->GetMaterial()->GetTexture())
				poly->GetMaterial()->GetTexture()->CacheIn(-1);

			// Get lightmap atlas if needed and also get the lightmap modifier
			if(poly->GetLightmap() && poly->GetLightmap()->GetTexture() && poly->GetLightmap()->GetTexture()->GetSurface())
			{
				DDSURFACEDESC2 ddsd;
				poly->GetLightmap()->GetTexture()->GetSurface()->GetSurfaceDesc(&ddsd);

				if(ddsd.dwHeight == ddsd.dwWidth)
				{
					atlas = Engine::Game->GetMainResources()->GetLightmapAtlas(INT2(ddsd.dwWidth, ddsd.dwHeight));
					lightmapUVMod = atlas->GetModifiedUV(poly->GetLightmap()->GetTexture());
				}
				else
				{
					LogWarn() << "Ignoring non-quadratic shaped lightmap";
				}
			}

			// Pack all vertices into a better datastructure
			std::vector<ExTVertexStruct> packed;

			if(poly->GetNumPolyVertices() == 3)
			{
				for(int j = poly->GetNumPolyVertices() - 1; j >= 0; j--) // Flip order, gothic has it different
				{
					ExTVertexStruct vx;
					poly->PackVertex(j, vx);

					if(poly->GetLightmap())
						vx.TexCoord2 = poly->GetLightmap()->GetLightmapTexCoords(vx.Position);
					else
						vx.TexCoord2 = float2(0,0);

					// Apply atlas-transform to the Lightmap-UV
					if(atlas)
					{
						vx.TexCoord2.x /= lightmapUVMod.second.x;
						vx.TexCoord2.y /= lightmapUVMod.second.y;
						vx.TexCoord2 += lightmapUVMod.first;
					}
					packed.push_back(vx);
				}
			}
			else
			{
				for(int j = 0; j < poly->GetNumPolyVertices(); j++) // Conversion from fan to list already changing order
				{
					ExTVertexStruct vx;
					poly->PackVertex(j, vx);

					if(poly->GetLightmap())
						vx.TexCoord2 = poly->GetLightmap()->GetLightmapTexCoords(vx.Position);
					else
						vx.TexCoord2 = float2(0,0);

					// Apply atlas-transform to the Lightmap-UV
					if(atlas)
					{
						vx.TexCoord2.x /= lightmapUVMod.second.x;
						vx.TexCoord2.y /= lightmapUVMod.second.y;
						vx.TexCoord2 += lightmapUVMod.first;
					}

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

					// Add the polygon of this triangle
					polygons.push_back(poly);
				}
			}

			// Put at the back of the vertices-vector
			vertices.insert(vertices.end(), validVertices.begin(), validVertices.end());
		}

	}else
	{
		// Build the triangle-lists for all subnodes, eventually reaching the leafs
		if(m_Front)
			m_Front->BuildTriangleList(vertices, polygons);

		if(m_Back)
			m_Back->BuildTriangleList(vertices, polygons);
	}
}



/** Does frustumculling and draws this node if it is the lowest acceptable */
void GBspNode::DrawNodeRecursive(float minNodeSizeXZ, RRenderQueueID queue, GBspTree::BSPRenderInfo info, std::vector<GVobObject*>& visibleVobs)
{
	// PB does something really weird with the tree. They are stacking Leafs?
	// Anyways, they seem to do something with the nodes BBox.y here, so do as they do...
	// Attention: This could also lead to the bug which happens when you look at the sky and all the vobs disappear?
	//zTBBox3D nodeBox = m_BBox;
	//nodeBox.m_Max.y = std::max(std::min(info.CameraPostion.y, info.WorldMaxY), m_BBox.m_Max.y); // TODO: Could compute the min part outside...
	
	// If the BBox is inside the frustum, we can just draw the contents of this node here
	zTCam_ClipType clip = zCCamera::BBox3DInFrustumCached(m_BBox, info.FrustumPlanes, info.FrustumSignBits, m_FrustumTestCache, info.ClipFlags);
	//zTCam_ClipType clip = zCCamera::GetActiveCamera()->BBox3DInFrustum(m_BBox, info.ClipFlags);

	// Trivial out?
	if (clip == ZTCAM_CLIPTYPE_OUT)
		return;
	
	// Check if we can just draw this. Trivial in?
	if(clip == ZTCAM_CLIPTYPE_IN || m_IsLeaf)
	{
		//RTools::LineRenderer.AddAABBMinMax(m_BBox.m_Min, m_BBox.m_Max, float4(1,0,0,1));

		CollectVobs(visibleVobs, info.CameraPostion, info.ObjectFarplane);
		return;
	}

	// Node is just crossing the frustum. Break it further down if possible.

	if (m_Front)m_Front->DrawNodeRecursive(minNodeSizeXZ, queue, info, visibleVobs);
	if (m_Back)m_Back->DrawNodeRecursive(minNodeSizeXZ, queue, info, visibleVobs);
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
			
			if (vob && vob->UpdateObjectCollectionState(frame))
			{
				bool isIndoor = (leaf->LeafVobList.Array[i]->GetGroundPolygon() && leaf->LeafVobList.Array[i]->GetGroundPolygon()->GetMaterial()->GetSectorFront());

				if(!isIndoor)
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
