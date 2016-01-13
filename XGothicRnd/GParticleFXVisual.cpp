#include "pch.h"
#include "GParticleFXVisual.h"
#include <REngine.h>
#include <RDevice.h>
#include <RBuffer.h>
#include "zCParticleFX.h"
#include "GTexture.h"
#include "GBaseDrawable.h"
#include "GParticleFXDrawable.h"
#include <RBlendState.h>
#include <RDepthStencilState.h>
#include "Engine.h"
#include "GGame.h"
#include "GWorld.h"

struct ParticleInfo
{
	float3 m_WorldPosition;
	float3 m_Velocity;
	float2 m_Scale;
	float4 m_Color;
};

GParticleFXVisual::GParticleFXVisual(zCVisual* sourceVisual) : GVisual(sourceVisual)
{
	m_ParticleInfoBuffer = REngine::ResourceCache->CreateResource<RBuffer>();
	m_ParticleInfoBuffer->Init(nullptr, sizeof(ParticleInfo), sizeof(ParticleInfo), B_SHADER_RESOURCE, U_DYNAMIC, CA_WRITE, "PFX-Buffer");
	m_PFXPipelineState = nullptr;
}


GParticleFXVisual::~GParticleFXVisual()
{
	delete m_ParticleInfoBuffer;
}

/** Updates the statecache of the given drawable. If the drawable wasn't registered in
the statecache before, will create a new cache entry and return a new pointer to the
drawables state-cache. Otherwise the old pointer will be returned. */
GVisual::StateCache* GParticleFXVisual::UpdatePipelineStatesFor(GBaseDrawable* drawable, GConstants::ERenderStage stage)
{
	StateCache& cache = m_CreatedPipelineStates[drawable];

	// Don't update twice for other passes
	if(m_LastFrameUpdated == REngine::RenderingDevice->GetFrameCounter())
		return &cache;

	m_LastFrameUpdated = REngine::RenderingDevice->GetFrameCounter();

	zCParticleFX* pfx = (zCParticleFX*)m_SourceObject;
	zCParticleEmitter* emt = pfx->GetEmitter();
	

	RPipelineState* defState = REngine::ResourceCache->GetCachedObject<RPipelineState>(GConstants::PipelineStates::BPS_PARTICLES);
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
	GTexture* visTexture = GTexture::GetFromSource(emt->visTexture);

	sm.SetFromPipelineState(defState);

	// Set the paged buffers to the state machine
	sm.SetVertexBuffer(0, nullptr);
	sm.SetIndexBuffer(nullptr);
	sm.SetStructuredBuffer(0, m_ParticleInfoBuffer, EShaderType::ST_VERTEX);

	// Make sure we have enough space
	cache.PipelineStates.resize(1);

	
	// Clear old state
	REngine::ResourceCache->DeleteResource(cache.PipelineStates[0]);

	// Apply texture
	if(visTexture)
		sm.SetTexture(0, visTexture->GetTexture(), EShaderType::ST_PIXEL);

	// Apply blending
	RBlendStateInfo bsi;
	switch (emt->visAlphaFunc)
	{
	case zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_ADD:
		bsi.SetAdditiveBlending();
		break;

	case zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_BLEND:
		bsi.SetAlphaBlending();
		break;

	case zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_MUL:
		bsi.SetModulateBlending();
		break;
	}

	// Turn off depth-writes
	RDepthStencilStateInfo dssi;
	dssi.DepthWriteEnabled = false;

	// Get the blendstate
	RBlendState* bs = REngine::ResourceCache->GetCachedObject<RBlendState>(Toolbox::HashObject(bsi));
	if (!bs)
	{
		bs = REngine::ResourceCache->CreateResource<RBlendState>();
		bs->CreateState(bsi);
		REngine::ResourceCache->AddToCache(Toolbox::HashObject(bsi), bs);
	}

	// Get the depth-state
	RDepthStencilState* dss = REngine::ResourceCache->GetCachedObject<RDepthStencilState>(Toolbox::HashObject(dssi));
	if (!dss)
	{
		dss = REngine::ResourceCache->CreateResource<RDepthStencilState>();
		dss->CreateState(dssi);
		REngine::ResourceCache->AddToCache(Toolbox::HashObject(dssi), dss);
	}

	sm.SetBlendState(bs);
	sm.SetDepthStencilState(dss);

	// Make drawcall using the values from the buffer.
	cache.PipelineStates[0] = sm.MakeDrawCall(6 * m_ParticleInfoBuffer->GetSizeInBytes() / m_ParticleInfoBuffer->GetStructuredByteSize());
	m_PFXPipelineState = cache.PipelineStates[0];

#ifndef PUBLIC_RELEASE
	//cache.PipelineStates[i]->SubmeshIdx = i;
	//cache.PipelineStates[i]->SourceObject = this;
#endif

		// Tell the drawable
	drawable->OnReaquiredState(stage, cache.PipelineStates[0]);



	return &cache;
}

/** Called when a drawable got drawn. Only if the InformVisual-Flag is set on it, though! */
void GParticleFXVisual::OnDrawableDrawn(GBaseDrawable* drawable)
{
	// Update particles
	UpdateEffect();
}

/** Updates the underlaying particle effect to get it ready for rendering */
void GParticleFXVisual::UpdateEffect()
{
	zCParticleFX* pfx = (zCParticleFX*)m_SourceObject;
	zCParticleEmitter* emt = pfx->GetEmitter();

	if(!pfx->PrepareForRendering())
		return; // TODO: Flag something so the drawcall won't be submitted with 0 particles

	// Plug the texture into the state
	m_PFXPipelineState->Textures[EShaderType::ST_PIXEL].resize(1);
	if(emt->visTexture 
		&& emt->visTexture->GetSurface()
		&& emt->visTexture->GetSurface()->GetEngineTexture())
		m_PFXPipelineState->Textures[EShaderType::ST_PIXEL][0] = emt->visTexture->GetSurface()->GetEngineTexture()->GetTexture();
	else
		m_PFXPipelineState->Textures[EShaderType::ST_PIXEL].clear();

	// Process first particle
	pfx->ProcessParticle(nullptr);

	// Get access to particle-buffer
	ParticleInfo* particleData;
	UINT maxNumParticles = m_ParticleInfoBuffer->GetSizeInBytes() / m_ParticleInfoBuffer->GetStructuredByteSize();
	LEB(m_ParticleInfoBuffer->Map((void**)&particleData));

	// Loop through all active particles
	UINT n = 0;
	for(zTParticle* p = pfx->GetFirstParticle(); p != nullptr; p = p->m_Next, n++)
	{
		// Trash the next particle, if invalid or outdated
		pfx->ProcessParticle(p);

		// In case we reached the maximum, increase the buffers size
		if(n == maxNumParticles)
		{
			// Backup what we have so far
			ParticleInfo* backup = new ParticleInfo[n];
			memcpy(backup, particleData, sizeof(ParticleInfo) * n);

			// Unmap and resize the buffer
			LEB(m_ParticleInfoBuffer->Unmap());
			m_ParticleInfoBuffer->UpdateData(nullptr, sizeof(ParticleInfo) * n * 2); // Double the size of the buffer

			// Get new datapointer and get our backup in there
			LEB(m_ParticleInfoBuffer->Map((void**)&particleData));
			memcpy(particleData, backup, sizeof(ParticleInfo) * n);

			maxNumParticles = n * 2;

			delete[] backup;
		}

		ParticleInfo i;
		i.m_WorldPosition = p->m_PositionWS;
		i.m_Color.x = p->m_Color.x / 255.0f;
		i.m_Color.y = p->m_Color.y / 255.0f;
		i.m_Color.z = p->m_Color.z / 255.0f;
		i.m_Velocity = p->m_Vel;

		// Calculate alpha-value
		if(emt->visTexAniIsLooping != 2) // 2 seems to be some magic case with sinus smoothing
		{
			i.m_Color.w = std::min(p->m_Alpha, 255.0f) / 255.0f;
		}else
		{
			i.m_Color.w = std::min((zCParticleFX::SinSmooth(fabs((p->m_Alpha - emt->visAlphaStart) * emt->m_ooAlphaDist)) * p->m_Alpha) / 255.0f, 255.0f);
		}

		i.m_Color.w = std::max(i.m_Color.w, 0.0f);

		// The game draws some particles as big triangles with a texture masked to look like a quad. 
		// We don't do this for simplicity. Since these particles are twice as big, scale them down.
		i.m_Scale =  emt->visTexIsQuadPoly ? p->m_Size : p->m_Size * 0.5f; 

		particleData[n] = i;

		// Let the game engine do some work on the particle
		// TODO: Maybe do this in an other loop because of cache-misses?
		pfx->UpdateParticle(p);
	}

	LEB(m_ParticleInfoBuffer->Unmap());

	// Update particle count
	if(m_PFXPipelineState)
	{
		m_PFXPipelineState->NumDrawElements = 6 * n;
	}

	// Tell the game we are done rendering
	pfx->FinalizeUpdate();
}

/** Creates a drawable for this visual */
void GParticleFXVisual::CreateDrawables(std::vector<GBaseDrawable*>& v)
{
	v.push_back(new GParticleFXDrawable(this));
	v.back()->ReaquireStateCache();
}