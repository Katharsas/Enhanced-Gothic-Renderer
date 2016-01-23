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

	// De-register, but check if it was already deleted first
	if(GVobObject::QueryFromSource(m_SourceObject))
		m_SourceObject->SetVobObject(nullptr);

	//LogInfo() << "Deleting Vob at 0x" << this << " with source of 0x" << m_SourceObject;

	if(!m_Visual)
		LogWarn() << "Deleting Vob without visual?";

	GASSERT(m_Visual->RemoveVob(this), "GVobObject must be registered in its Visual!");

	if(Engine::Game->GetActiveWorld()->RemoveVob(m_SourceObject))
	{
		LogWarn() << "Deleted GVobObject was still in the level!";
	}

	// Let the drawables know everything is okay
	for(int i=0;i<NUM_VISUAL_LOD_LEVELS;i++)
		for(auto d : m_Drawables[i])
			d->SetHomeVob(nullptr);


	if(GVisual::QueryFromSource(m_SourceObject->GetVisual()))
		for(int i = 0; i < NUM_VISUAL_LOD_LEVELS; i++)
			Toolbox::DeleteElements(m_Drawables[i]); // Only delete if the visual wasn't already deleted
		
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
	for(int i = 0; i < NUM_VISUAL_LOD_LEVELS; i++)
	{
		for(GBaseDrawable* d : m_Drawables[i])
		{
			d->SetInstanceInfo(m_InstanceInfo);
		}
	}

	UpdateRenderInstanceCache();
}

/** Makes a new renderinstances and puts it into the given vector */
void GVobObject::MakeRenderInstances(std::vector<RenderInstance>& instances, GConstants::ERenderStage stage, float lodDistanceNormalized)
{
	// Calculate current LOD-Level
	int lod = std::min((int)(lodDistanceNormalized * NUM_VISUAL_LOD_LEVELS + 0.5f), NUM_VISUAL_LOD_LEVELS-1);

	// Use everything from the cache we can
	for (unsigned int i = 0; i < std::min(m_Drawables[lod].size(), RENDERINSTANCECACHE_SIZE); i++)
	{
		auto& inst = m_RenderInstanceCache[lod][stage][i];
		instances.push_back(inst.second);

		// Only follow the pointer if the cache says we have to
		if (inst.first)
		{
			GBaseDrawable* d = m_Drawables[lod][i];
			d->OnDrawn();

			// FIXME: HACK! GModelVisuals will replace the visuals when the attachments change
			// if we got a new pointer after executing d->OnDrawn() it means our Drawable-List just got deleted and rebuilt!
			if(d != m_Drawables[lod][i])
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
	for (unsigned int i = RENDERINSTANCECACHE_SIZE; i<m_Drawables[lod].size(); i++)
	{
		GBaseDrawable* d = m_Drawables[lod][i];
		if (d->HasStatesForStage(stage))
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
	for(int i = 0; i < NUM_VISUAL_LOD_LEVELS; i++)
	{
		for(auto d : m_Drawables[i])
			d->SetHomeVob(nullptr);

		Toolbox::DeleteElements(m_Drawables[i]);
		m_Visual->CreateDrawables(m_Drawables[i], i);
	}

	UpdateRenderInstanceCache();

	m_DynamicDrawState = false;

	for(int i = 0; i < NUM_VISUAL_LOD_LEVELS; i++)
	{
		for(auto d : m_Drawables[i])
		{
			d->SetHomeVob(this);

			m_DynamicDrawState = d->ShouldInformVisual() ? true : m_DynamicDrawState;
		}
	}
	
}

/** Updates the cache of the render-instances */
void GVobObject::UpdateRenderInstanceCache()
{
	memset(m_RenderInstanceCache, 0, sizeof(m_RenderInstanceCache));

	for(int l = 0; l < NUM_VISUAL_LOD_LEVELS; l++)
	{
		for(unsigned int i = 0; i < std::min(m_Drawables[l].size(), RENDERINSTANCECACHE_SIZE); i++)
		{
			GBaseDrawable* d = m_Drawables[l][i];
			for(int s = 0; s < GConstants::ERenderStage::RS_NUM_STAGES; s++)
			{
				if(d->HasStatesForStage((GConstants::ERenderStage)s))
				{
					m_RenderInstanceCache[l][(GConstants::ERenderStage)s][i].second = (RenderInstance((uint32_t)d->GetVisual(), &d->GetInstanceInfo(), d, this));
					m_RenderInstanceCache[l][(GConstants::ERenderStage)s][i].first = d->ShouldInformVisual();
				}
			}
		}
	}
}