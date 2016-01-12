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

const int DEFAULT_INSTANCE_BUFFER_SIZE = 1024 * sizeof(VobInstanceInfo);

GMainResources::GMainResources(void) : 
	m_ExPagedVertexBuffer(EBindFlags::B_VERTEXBUFFER),
	m_ExSkelPagedVertexBuffer(EBindFlags::B_VERTEXBUFFER),
	m_ExPagedIndexBuffer(EBindFlags::B_INDEXBUFFER),
	m_ExSkelPagedIndexBuffer(EBindFlags::B_INDEXBUFFER)
{
	m_VobInstanceBuffer = REngine::ResourceCache->CreateResource<RBuffer>();
	m_VobInstanceBuffer->Init(nullptr,
		DEFAULT_INSTANCE_BUFFER_SIZE,
		sizeof(VobInstanceInfo),
		EBindFlags::B_VERTEXBUFFER,
		EUsageFlags::U_DYNAMIC,
		ECPUAccessFlags::CA_WRITE,
		"Vob Instance Buffer");
}


GMainResources::~GMainResources(void)
{
	REngine::ResourceCache->DeleteResource(m_VobInstanceBuffer);
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