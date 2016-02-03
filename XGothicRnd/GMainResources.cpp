#include "pch.h"
#include "GMainResources.h"
#include <RPagedBuffer.h>
#include "zCVisual.h"
#include "GVisual.h"
#include "GStaticMeshVisual.h"
#include "GVobObject.h"
#include "GBaseDrawable.h"
#include "GModelVisual.h"
#include "GMeshSoftSkin.h"
#include "zCMorphMesh.h"
#include "GMorphMesh.h"
#include "GParticleFXVisual.h"
#include <RTextureAtlas.h>

const int DEFAULT_INSTANCE_BUFFER_SIZE = 1024 * sizeof(VobInstanceInfo);

GMainResources::GMainResources(void) : 
	m_ExPagedVertexBuffer(RAPI::EBindFlags::B_VERTEXBUFFER),
	m_ExSkelPagedVertexBuffer(RAPI::EBindFlags::B_VERTEXBUFFER),
	m_ExPagedIndexBuffer(RAPI::EBindFlags::B_INDEXBUFFER),
	m_ExSkelPagedIndexBuffer(RAPI::EBindFlags::B_INDEXBUFFER)
{
	m_VobInstanceBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	m_VobInstanceBuffer->Init(nullptr,
		DEFAULT_INSTANCE_BUFFER_SIZE,
		sizeof(VobInstanceInfo),
		RAPI::EBindFlags::B_VERTEXBUFFER,
		RAPI::EUsageFlags::U_DYNAMIC,
		RAPI::ECPUAccessFlags::CA_WRITE,
		"Vob Instance Buffer");
}


GMainResources::~GMainResources(void)
{
	RAPI::REngine::ResourceCache->DeleteResource(m_VobInstanceBuffer);
}

RAPI::RTextureAtlas* GMainResources::GetLightmapAtlas(const INT2& singleTextureSize)
{
	GASSERT(singleTextureSize.x == singleTextureSize.y, "Lightmaps must have size x == y");

	auto p = std::make_pair(singleTextureSize.x, singleTextureSize.y);
	auto it = m_LightmapAtlasCache.find(p);
	if(it == m_LightmapAtlasCache.end())
	{
		// Create new atlas
		m_LightmapAtlasCache[p] = RAPI::REngine::ResourceCache->CreateResource<RAPI::RTextureAtlas>();
	}

	return m_LightmapAtlasCache[p];
}

// Constructs all lightmap atlases
void GMainResources::ConstructLightmapAtlases()
{
	for(auto p : m_LightmapAtlasCache)
	{
		p.second->Construct(RAPI::RInt2(p.first.first, p.first.second));
		p.second->GetTexture()->SaveToFileAPI("atlas_" + std::to_string(p.first.first) + "x" + std::to_string(p.first.second) + ".png");
	}
}

// Deletes all lightmap atlases
void GMainResources::ClearLightmapAtlases()
{
	for(auto p : m_LightmapAtlasCache)
	{
		RAPI::REngine::ResourceCache->DeleteResource(p.second);
	}
	m_LightmapAtlasCache.clear();
}

/** Creates the right type of visual from the source */
GVisual* GMainResources::CreateVisualFrom(zCVisual* sourceVisual)
{
	switch(sourceVisual->GetVisualType())
	{

	case zCVisual::VT_MODEL:
		return new GModelVisual(sourceVisual);
		break;

	case zCVisual::VT_PROGMESHPROTO:
		return new GStaticMeshVisual(sourceVisual);
		break;

	case zCVisual::VT_MESHSOFTSKIN:
		return new GMeshSoftSkin(sourceVisual);
		break;
	
	case zCVisual::VT_MORPHMESH:
		return new GMorphMesh(sourceVisual);
		break;

	case zCVisual::VT_PARTICLE_FX:
		return new GParticleFXVisual(sourceVisual);
		break;
	/*
	case zCVisual::VT_DECAL:
		return new GDecalVisual(sourceVisual);
		break;*/

	default:
		return NULL;
	}

	return NULL;
}