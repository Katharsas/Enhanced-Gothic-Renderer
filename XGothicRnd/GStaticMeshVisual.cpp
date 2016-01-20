#include "pch.h"
#include "GStaticMeshVisual.h"
#include "zCProgMeshProto.h"
#include "GMaterial.h"
#include "Engine.h"
#include "GGame.h"
#include "GMainResources.h"
#include "GStaticMeshDrawable.h"
#include <RTools.h>

const std::vector<float> LOD_LEVEL_MIN_SIZES = {0, 400.0f, 700.0f, 1100.0f};

GStaticMeshVisual::GStaticMeshVisual(zCVisual* sourceObject) : GVisual(sourceObject)
{
	// This must be a zCProgMeshProto.
	zCProgMeshProto* mesh = (zCProgMeshProto *)sourceObject;
	GMainResources* res = Engine::Game->GetMainResources();

	std::vector<ExTVertexStruct> vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> submeshIndexStarts;

	// Pack all vertices into the vectors above
	mesh->PackVertices<ExTVertexStruct, unsigned int>(vertices, indices, 0, submeshIndexStarts);

	// Register ourselfs in the global index buffer collection to get notified whenever the
	// buffer gets rebuilt
	res->GetExPagedIndexBuffer()->RegisterObserver(this);

	// Create vertexbuffer for this whole mesh
	m_LogicalVertexBuffer = res->GetExPagedVertexBuffer()->AddLogicalBuffer(vertices.data(), vertices.size());
	m_LogicalIndexBuffer = res->GetExPagedIndexBuffer()->AddLogicalBuffer(indices.data(), indices.size());

	// Create objects for all submeshes
	for(unsigned int i=0;i<mesh->GetNumSubmeshes();i++)
	{		
		m_SubMeshes.push_back(SubMesh());
		m_SubMeshes.back().m_Material = GMaterial::GetFromSource(mesh->GetSubmesh(i).m_Material);
		m_SubMeshes.back().m_SourceMaterial = &mesh->GetSubmesh(i).m_Material;
		m_SubMeshes.back().m_SubMeshIndexStart = submeshIndexStarts[i];	
		m_SubMeshes.back().m_NumIndices = mesh->GetSubmesh(i).m_TriangleList.m_NumInArray * 3;
	}
}


GStaticMeshVisual::~GStaticMeshVisual(void)
{
	GMainResources* res = Engine::Game->GetMainResources();
	res->GetExPagedIndexBuffer()->RemoveObserver(this);
}


/**
 * Called when the paged index buffer got rebuilt
 */
void GStaticMeshVisual::OnNotified(size_t id, void* userData)
{
	// The paged buffers put their final data into the userData
	unsigned int* indices = (unsigned int*)userData;

	// Modify the data to match the index offsets
	for(unsigned int i=0;i<m_LogicalIndexBuffer->PageNumElements;i++)
	{
		// Add the page-offset of the vertexbuffer so the indices match the vertices
		indices[m_LogicalIndexBuffer->PageStart + i] += m_LogicalVertexBuffer->PageStart;
	}

	// Modify instance index
	for(auto& c : m_CreatedPipelineStates)
	{
		unsigned int i=0;
		for(RPipelineState* s : c.second.PipelineStates)
		{
			s->StartIndexOffset = m_SubMeshes[i].m_SubMeshIndexStart + m_LogicalIndexBuffer->PageStart;
			i++;
		}
	}
}

/** Updates the statecache of the given drawable. If the drawable wasn't registered in
	the statecache before, will create a new cache entry and return a new pointer to the
	drawables state-cache. Otherwise the old pointer will be returned. */
GVisual::StateCache* GStaticMeshVisual::UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage)
{
	StateCache& cache = m_CreatedPipelineStates[drawable];

	zCProgMeshProto* mesh = (zCProgMeshProto*)m_SourceObject;
	RPipelineState* defState = nullptr;
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
	RPagedBuffer<ExTVertexStruct>* pagedVB = Engine::Game->GetMainResources()->GetExPagedVertexBuffer();
	RPagedBuffer<unsigned int>* pagedIB = Engine::Game->GetMainResources()->GetExPagedIndexBuffer();
	RBuffer* instanceBuffer = Engine::Game->GetMainResources()->GetVobInstanceBuffer();

	// Get the right state for the stage
	switch(stage)
	{
	case GConstants::RS_WORLD:
		defState = REngine::ResourceCache->GetCachedObject<RPipelineState>(GConstants::PipelineStates::BPS_INSTANCED_VOB);
		break;

	case GConstants::RS_INVENTORY:
		defState = REngine::ResourceCache->GetCachedObject<RPipelineState>(GConstants::PipelineStates::BPS_INSTANCED_VOB_INVENTORY);
		break;
	}

	if(!defState)
		return nullptr;

	sm.SetFromPipelineState(defState);

	// Check for alpha-test

	// Set the paged buffers to the state machine
	sm.SetVertexBuffer(0, pagedVB->GetBuffer());
	sm.SetIndexBuffer(pagedIB->GetBuffer());

	// Set global instancing buffer
	sm.SetVertexBuffer(1, instanceBuffer);

	// Make sure we have enough space
	cache.PipelineStates.resize(m_SubMeshes.size());

	// Create a pipeline-state for each submesh
	unsigned int i=0;
	for(SubMesh& m : m_SubMeshes)
	{
		// Clear old states
		REngine::ResourceCache->DeleteResource(cache.PipelineStates[i]);

		// Sanity
		if(m.m_Material != GMaterial::QueryFromSource(*m.m_SourceMaterial))
		{
			m.m_Material = GMaterial::QueryFromSource(*m.m_SourceMaterial);
			LogWarn() << "Hotswapped GMaterial because a SetMaterial-Call was missed!";
		}

		if(!m.m_Material)
		{
			cache.PipelineStates.clear();
			LogWarn() << "Failed to find GMaterial or SubMesh-Material was NULL.";
			return &cache;
		}
		
		// Set the masked-state in the material if the mesh is using alpha-test
		// This isn't a nice solution, but I have no idea why the materials couldn't save this themselfes,
		// and PB decided to save a flag for a whole object
		if (mesh->GetAlphaTestingEnabled())
			m.m_Material->GetSourceObject()->SetBlendFunc(zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_TEST);

		sm.SetPixelShader(m.m_Material->GetMaterialPixelShader(stage));

		// Apply textures and other parameters
		m.m_Material->ApplyStates();

		// Make drawcall using the values from the logical buffers. Set -1 as instance index as a placeholder
		switch(stage)
		{
		case GConstants::RS_WORLD:
			cache.PipelineStates[i] = sm.MakeDrawCallIndexedInstanced(m.m_NumIndices, 1, m_LogicalIndexBuffer->PageStart + m.m_SubMeshIndexStart, 0, (unsigned int)-1);
			break;

		case GConstants::RS_INVENTORY:
			cache.PipelineStates[i] = sm.MakeDrawCallIndexed(m.m_NumIndices, m_LogicalIndexBuffer->PageStart + m.m_SubMeshIndexStart, 0);
			break;
		}

#ifndef PUBLIC_RELEASE
		//cache.PipelineStates[i]->SubmeshIdx = i;
		//cache.PipelineStates[i]->SourceObject = this;
#endif

		// Tell the drawable
		drawable->OnReaquiredState(stage, cache.PipelineStates[i]);

		i++;
	}

	return &cache;
}

/** Creates a drawable for this visual */
void GStaticMeshVisual::CreateDrawables(std::vector<GBaseDrawable*>& v, int lodLevel)
{
	// Don't create drawable if we are too small for a far lod-level
	float fLodSizeIdx = ((float)lodLevel / (float)NUM_VISUAL_LOD_LEVELS) * LOD_LEVEL_MIN_SIZES.size();
	int lodSizeIdx = (int)(fLodSizeIdx + 0.5f);
	if( m_VisualSize < LOD_LEVEL_MIN_SIZES[lodSizeIdx])
		return;

	v.push_back(new GStaticMeshDrawable(this));
	v.back()->ReaquireStateCache();
}

/** Caches the textures used by this visual */
void GStaticMeshVisual::CacheTextures(bool force)
{
	for(SubMesh& m : m_SubMeshes)
	{
		if(m.m_Material)
			m.m_Material->CacheTextures();
	}
}