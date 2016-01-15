#include "pch.h"
#include "GMorphMesh.h"
#include "GMorphMeshDrawable.h"
#include "Engine.h"
#include "GGame.h"
#include "GMainResources.h"
#include <RBuffer.h>
#include "GMaterial.h"
#include "zCMorphMesh.h"
#include "zCProgMeshProto.h"
#include <RBuffer.h>
#include "GTexture.h"

GMorphMesh::GMorphMesh(zCVisual* sourceObject) : GVisual(sourceObject)
{
	// This must be a zCMorphMesh.
	zCMorphMesh* morphMesh = (zCMorphMesh*)sourceObject;
	zCProgMeshProto* mesh = morphMesh->GetStaticMesh();

	std::vector<ExTVertexStruct> vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> submeshIndexStarts;

	// Pack all vertices into the vectors above
	mesh->PackVertices<ExTVertexStruct, unsigned int>(vertices, indices, 0, submeshIndexStarts);

	// Create vertex and index-buffer for this mesh
	m_VertexBuffer = REngine::ResourceCache->CreateResource<RBuffer>();
	m_VertexBuffer->Init(vertices.data(), 
		vertices.size() * sizeof(ExTVertexStruct),
		sizeof(ExTVertexStruct),
		EBindFlags::B_VERTEXBUFFER,
		EUsageFlags::U_DYNAMIC,
		ECPUAccessFlags::CA_WRITE);

	m_IndexBuffer = REngine::ResourceCache->CreateResource<RBuffer>();
	m_IndexBuffer->Init(indices.data(), 
		indices.size() * sizeof(unsigned int),
		sizeof(unsigned int),
		EBindFlags::B_INDEXBUFFER);

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


GMorphMesh::~GMorphMesh()
{
}

/** Updates the statecache of the given drawable. If the drawable wasn't registered in
the statecache before, will create a new cache entry and return a new pointer to the
drawables state-cache. Otherwise the old pointer will be returned. */
GVisual::StateCache* GMorphMesh::UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage)
{
	zCMorphMesh* morphMesh = (zCMorphMesh*)m_SourceObject;
	zCProgMeshProto* mesh = morphMesh->GetStaticMesh();

	StateCache& cache = m_CreatedPipelineStates[drawable];

	RPipelineState* defState = REngine::ResourceCache->GetCachedObject<RPipelineState>(GConstants::PipelineStates::BPS_INSTANCED_VOB);
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
	RBuffer* instanceBuffer = Engine::Game->GetMainResources()->GetVobInstanceBuffer();

	sm.SetFromPipelineState(defState);

	// Set the paged buffers to the state machine
	sm.SetVertexBuffer(0, m_VertexBuffer);
	sm.SetIndexBuffer(m_IndexBuffer);

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

		sm.SetPixelShader(m.m_Material->GetMaterialPixelShader(GConstants::RS_WORLD));

		// Apply textures and other parameters
		m.m_Material->ApplyStates();

		// Make drawcall using the values from the buffers
		cache.PipelineStates[i] = sm.MakeDrawCallIndexedInstanced(m.m_NumIndices, 1,  m.m_SubMeshIndexStart, 0, 1);

		// Tell the drawable
		drawable->OnReaquiredState(stage, cache.PipelineStates[i]);

		i++;
	}

	return &cache;
}

/** Creates a drawable for this visual */
void GMorphMesh::CreateDrawables(std::vector<GBaseDrawable*>& v, int lodLevel)
{
	v.push_back(new GMorphMeshDrawable(this));
	v.back()->ReaquireStateCache();
}

/** Updates the textures for the created pipelinestates */
void GMorphMesh::UpdateTextures()
{
	for(auto& c : m_CreatedPipelineStates)
	{
		int i=0;
		for(auto& s : c.second.PipelineStates)
		{
			SubMesh& m = m_SubMeshes[i];

			if(!m.m_Material)
				continue;

			if(m.m_Material->GetSourceObject() != *m.m_SourceMaterial)
			{
				m.m_Material = GMaterial::QueryFromSource(*m.m_SourceMaterial);
				LogWarn() << "Hotswapped GMaterial because a SetMaterial-Call was missed!";
			}

			if(!m.m_Material)
				continue;

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

/** Called when a drawable got drawn. Only if the InformVisual-Flag is set on it, though! */
void GMorphMesh::OnDrawableDrawn(GBaseDrawable* drawable)
{
	zCMorphMesh* morphMesh = (zCMorphMesh*)m_SourceObject;
	morphMesh->GetTexAniState()->UpdateTexList();
	UpdateTextures();
}