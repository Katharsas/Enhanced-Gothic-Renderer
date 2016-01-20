#include "pch.h"
#include "GMeshSoftSkin.h"
#include "GMainResources.h"
#include "Engine.h"
#include "GGame.h"
#include "zCModelMeshLib.h"
#include "GMaterial.h"
#include "GConstants.h"
#include "GMeshSoftSkinDrawable.h"
#include "GTexture.h"

GMeshSoftSkin::GMeshSoftSkin(zCVisual* sourceObject) : GVisual(sourceObject)
{
	GMainResources* res = Engine::Game->GetMainResources();
	zCMeshSoftSkin* softSkin = (zCMeshSoftSkin*)sourceObject;

	std::vector<ExTSkelVertexStruct> vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> submeshIndexStarts;

	// Pack all vertices into the vectors above
	softSkin->PackVerticesAndWeights<ExTSkelVertexStruct, unsigned int>(vertices, indices, 0, submeshIndexStarts);

	// Register ourselfs in the global index buffer collection to get notified whenever the
	// buffer gets rebuilt
	res->GetExSkelPagedIndexBuffer()->RegisterObserver(this);

	// Create vertexbuffer for this whole mesh
	m_LogicalVertexBuffer = res->GetExSkelPagedVertexBuffer()->AddLogicalBuffer(vertices.data(), vertices.size());
	m_LogicalIndexBuffer = res->GetExSkelPagedIndexBuffer()->AddLogicalBuffer(indices.data(), indices.size());

	// Create objects for all submeshes
	for (unsigned int i = 0; i<softSkin->GetNumSubmeshes(); i++)
	{
		m_SubMeshes.push_back(SubMesh());
		m_SubMeshes.back().m_SourceMaterial = softSkin->GetSubmesh(i).m_Material;
		m_SubMeshes.back().m_Material = GMaterial::GetFromSource(m_SubMeshes.back().m_SourceMaterial);
		m_SubMeshes.back().m_SubMeshIndexStart = submeshIndexStarts[i];
		m_SubMeshes.back().m_NumIndices = softSkin->GetSubmesh(i).m_TriangleList.m_NumInArray * 3;
	}
}


GMeshSoftSkin::~GMeshSoftSkin()
{
	GMainResources* res = Engine::Game->GetMainResources();
	res->GetExSkelPagedIndexBuffer()->RemoveObserver(this);
}

/**
* Called when the paged index buffer got rebuilt
*/
void GMeshSoftSkin::OnNotified(size_t id, void* userData)
{
	// The paged buffers put their final data into the userData
	unsigned int* indices = (unsigned int*)userData;

	// Modify the data to match the index offsets
	for (unsigned int i = 0; i<m_LogicalIndexBuffer->PageNumElements; i++)
	{
		// Add the page-offset of the vertexbuffer so the indices match the vertices
		indices[m_LogicalIndexBuffer->PageStart + i] += m_LogicalVertexBuffer->PageStart;
	}

	// Modify instance index
	for (auto& c : m_CreatedPipelineStates)
	{
		unsigned int i = 0;
		for (RPipelineState* s : c.second.PipelineStates)
		{
			s->StartIndexOffset = m_SubMeshes[i].m_SubMeshIndexStart + m_LogicalIndexBuffer->PageStart;
			i++;
		}
	}
}


/** Updates the statecache of the given drawable. If the drawable wasn't registered in
the statecache before, will create a new cache entry and return a new pointer to the
drawables state-cache. Otherwise the old pointer will be returned. */
GVisual::StateCache* GMeshSoftSkin::UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage)
{
	zCMeshSoftSkin* softSkin = (zCMeshSoftSkin*)m_SourceObject;
	StateCache& cache = m_CreatedPipelineStates[drawable];

	RPipelineState* defState = REngine::ResourceCache->GetCachedObject<RPipelineState>(GConstants::PipelineStates::BPS_SKEL_MESH);
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
	RPagedBuffer<ExTSkelVertexStruct>* pagedVB = Engine::Game->GetMainResources()->GetExSkelPagedVertexBuffer();
	RPagedBuffer<unsigned int>* pagedIB = Engine::Game->GetMainResources()->GetExSkelPagedIndexBuffer();
	RBuffer* instanceBuffer = Engine::Game->GetMainResources()->GetVobInstanceBuffer();

	sm.SetFromPipelineState(defState);

	// Set the paged buffers to the state machine
	sm.SetVertexBuffer(0, pagedVB->GetBuffer());
	sm.SetIndexBuffer(pagedIB->GetBuffer());

	// Set global instancing buffer
	sm.SetVertexBuffer(1, instanceBuffer);

	// Make sure we have enough space
	cache.PipelineStates.resize(m_SubMeshes.size());

	// Create a pipeline-state for each submesh
	unsigned int i = 0;
	for (SubMesh& m : m_SubMeshes)
	{
		// Clear old states
		REngine::ResourceCache->DeleteResource(cache.PipelineStates[i]);

		// Apply textures and other parameters
		//if(m.m_SourceMaterial != softSkin->GetSubmesh(i).m_Material)
		{
			m.m_SourceMaterial = softSkin->GetSubmesh(i).m_Material;
			m.m_Material = GMaterial::GetFromSource(m.m_SourceMaterial);
		}

		// Set the masked-state in the material if the mesh is using alpha-test
		// This isn't a nice solution, but I have no idea why the materials couldn't save this themselfes,
		// and PB decided to save a flag for a whole object
		if (softSkin->GetAlphaTestingEnabled())
			m.m_Material->GetSourceObject()->SetBlendFunc(zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_TEST);

		sm.SetPixelShader(m.m_Material->GetMaterialPixelShader(GConstants::RS_WORLD));

		// Apply textures and other parameters
		m.m_Material->ApplyStates();

		// Make drawcall using the values from the logical buffers. We only draw one instance here.
		cache.PipelineStates[i] = sm.MakeDrawCallIndexedInstanced(m.m_NumIndices, 1, m_LogicalIndexBuffer->PageStart + m.m_SubMeshIndexStart, 0, 1);

		// Tell the drawable
		drawable->OnReaquiredState(stage, cache.PipelineStates[i]);

		i++;
	}

	return &cache;
}

/** Creates a drawable for this visual */
void GMeshSoftSkin::CreateDrawables(std::vector<GBaseDrawable*>& v, int lodLevel)
{
	v.push_back(new GMeshSoftSkinDrawable(this));
	v.back()->ReaquireStateCache();
}

/** Updates the textures for the created pipelinestates */
void GMeshSoftSkin::UpdateTextures()
{
	for(auto& c : m_CreatedPipelineStates)
	{
		int i=0;
		for(auto& s : c.second.PipelineStates)
		{
			SubMesh& m = m_SubMeshes[i];

			m.m_Material->CacheTextures();
			RTexture* stored = s->Textures[EShaderType::ST_PIXEL].empty() ? nullptr : s->Textures[EShaderType::ST_PIXEL][0];
			RTexture* actual = m.m_Material->GetDiffuse() ? m.m_Material->GetDiffuse()->GetTexture() : nullptr;

			// Switched to nullptr?
			if(!actual)
			{
				s->Textures[EShaderType::ST_PIXEL].clear();
			}else if(stored != actual)
			{
				RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
				sm.SetFromPipelineState(s);

				// Assign textures
				m.m_Material->ApplyStates();

				// Update state object
				sm.AssignPipelineStateValues(s);
			}

			i++;
		}
	}
}

/** Caches the textures used by this visual */
void GMeshSoftSkin::CacheTextures(bool force)
{
	for(SubMesh& m : m_SubMeshes)
	{
		m.m_Material->CacheTextures();
	}
}