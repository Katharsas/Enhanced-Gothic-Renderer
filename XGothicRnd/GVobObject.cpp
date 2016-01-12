#include "pch.h"
#include "GVobObject.h"
#include "zCVisual.h"
#include "GVisual.h"
#include "zCVob.h"
#include "GBaseDrawable.h"
#include "zCPolygon.h"
#include <RTools.h>
#include "Engine.h"
#include "GGame.h"
#include "GWorld.h"

GVobObject::GVobObject(zCVob* sourceObject) : GzObjectExtension<zCVob, GVobObject>(sourceObject)
{ 
	//LogInfo() << "Creating Vob at 0x" << this << " with source of 0x" << m_SourceObject << "(" << std::hex << sizeof(GVobObject) << " bytes)";



	// Register us in the source-vob
	sourceObject->SetVobObject(this);

	m_LastTimeCollected = 0;

	m_Visual = GVisual::QueryFromSource(sourceObject->GetVisual());
	
	if(!m_Visual)
	{
		LogError() << "Visual set to GVobObject has not been created!";
		return;
	}

#ifndef PUBLIC_RELEASE
	if(m_Visual->GetFileName() == "SKE_BODY2")
		hInvalidBreakpoint = SetHardwareBreakpoint(GetCurrentThread(), 
			HWBRK_TYPE_WRITE,HWBRK_SIZE_1,&m_Invalid);
	//m_ObjectName = ((zCObject*)sourceObject)->GetObjectName();
#endif


	GASSERT(m_Visual->AddVob(this), "GVobObject must not already be registered in its Visual!");

	ReaquireDrawables();

	UpdateVob();
}


GVobObject::~GVobObject(void)
{
#ifndef PUBLIC_RELEASE
	if(hInvalidBreakpoint)
		RemoveHardwareBreakpoint(hInvalidBreakpoint);
#endif

	//Toolbox::PrintStack();

	// De-register
	m_SourceObject->SetVobObject(nullptr);

	//LogInfo() << "Deleting Vob at 0x" << this << " with source of 0x" << m_SourceObject;

	if( std::string(m_SourceObject->GetObjectName()) == "MEATBUG"
		|| std::string(m_SourceObject->GetObjectName()) == "GIANT_BUG")
		sinf(1.0f);

	if(!m_Visual)
		LogWarn() << "Deleting Vob without visual?";

	GASSERT(m_Visual->RemoveVob(this), "GVobObject must be registered in its Visual!");

	if(Engine::Game->GetActiveWorld()->RemoveVob(m_SourceObject))
	{
		LogWarn() << "Deleted GVobObject was still in the level!";
	}

	// Let the drawables know everything is okay
	for(auto d : m_Drawables)
		d->SetHomeVob(nullptr);

	if(GVisual::QueryFromSource(m_SourceObject->GetVisual()))
		Toolbox::DeleteElements(m_Drawables); // Only delete if the visual wasn't already deleted
}

/** Called when the underlaying vob moved, for example */
void GVobObject::UpdateVob()
{
	// Get lighting from first feature		
	m_InstanceInfo.m_InstanceColor = m_SourceObject->GetStaticLighting();

	float3 position;
	Quaternion rotation;
	float3 scale;
	Matrix m = m_SourceObject->GetWorldMatrix();
	rotation = Quaternion::CreateFromRotationMatrix(m);
	position = m.TranslationT();

	// Vobs can't be scaled. Strip scale parameter to save some bandwidth.
	m_InstanceInfo.m_Position = position;
	m_InstanceInfo.m_Rotation = rotation;

	// Store worldmatrix as well, as we need a way to tell if the transforms changed
	m_WorldMatrix = m;

	// Set new instanceinfo for all drawables
	for (GBaseDrawable* d : m_Drawables)
	{
		d->SetInstanceInfo(m_InstanceInfo);
	}

	UpdateRenderInstanceCache();
}

/** Makes a new renderinstances and puts it into the given vector */
void GVobObject::MakeRenderInstances(std::vector<RenderInstance>& instances)
{
	// Use everything from the cache we can
	for (unsigned int i = 0; i < std::min(m_Drawables.size(), RENDERINSTANCECACHE_SIZE); i++)
	{
		auto& inst = m_RenderInstanceCache[GConstants::RS_WORLD][i];
		instances.push_back(inst.second);

		if(inst.second.m_Instance->m_InstanceColor == 0xcdcdcdcd)
			sinf(1.0f);

		// Only follow the pointer if the cache says we have to
		if (inst.first)
		{
			GBaseDrawable* d = m_Drawables[i];
			d->OnDrawn();

			// FIXME: HACK! GModelVisuals will replace the visuals when the attachments change
			// if we got a new pointer after executing d->OnDrawn() it means our Drawable-List just got deleted and rebuilt!
			if(d != m_Drawables[i])
			{
				// Remove already pushed drawables
				for(unsigned int j = 0; j < i+1; j++)
					instances.pop_back();

				// Start from beginning
				i=0;
			}
		}
	}

	// Create new states for the drawables that didn't fit into the cache
	for (unsigned int i = RENDERINSTANCECACHE_SIZE; i<m_Drawables.size(); i++)
	{
		GBaseDrawable* d = m_Drawables[i];
		if (d->HasStatesForStage(GConstants::ERenderStage::RS_WORLD))
		{
			instances.push_back(RenderInstance((uint32_t)d->GetVisual(), &d->GetInstanceInfo(), d, this));
			d->OnDrawn();
		}
	}
}

/** Reaquires the list of drawables */
void GVobObject::ReaquireDrawables()
{
	// Let the drawables know everything is okay
	for(auto d : m_Drawables)
		d->SetHomeVob(nullptr);

	Toolbox::DeleteElements(m_Drawables);

	m_Visual->CreateDrawables(m_Drawables);
	UpdateRenderInstanceCache();

	for(auto d : m_Drawables)
	{
		d->SetHomeVob(this);
	}
}

/** Updates the cache of the render-instances */
void GVobObject::UpdateRenderInstanceCache()
{
	memset(m_RenderInstanceCache, 0, sizeof(m_RenderInstanceCache));

	for (unsigned int i = 0; i < std::min(m_Drawables.size(), RENDERINSTANCECACHE_SIZE); i++)
	{
		GBaseDrawable* d = m_Drawables[i];
		if (d->HasStatesForStage(GConstants::ERenderStage::RS_WORLD))
		{
			m_RenderInstanceCache[GConstants::ERenderStage::RS_WORLD][i].second = (RenderInstance((uint32_t)d->GetVisual(), &d->GetInstanceInfo(), d, this));
			m_RenderInstanceCache[GConstants::ERenderStage::RS_WORLD][i].first = d->ShouldInformVisual();
		}
	}
}