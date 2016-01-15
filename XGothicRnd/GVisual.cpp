#include "pch.h"
#include "GVisual.h"
#include "GBaseDrawable.h"
#include "zCVisual.h"
#include "Engine.h"
#include "GGame.h"
#include "GMainResources.h"
#include "zEngineHooks.h"

GVisual::GVisual(zCVisual* sourceObject) : GzObjectExtension<zCVisual, GVisual>(sourceObject)
{
	m_FileName = sourceObject->GetObjectName();

	m_VisualSize = sourceObject->GetBBox3D().Size();

#ifndef PUBLIC_RELEASE
	if(!m_FileName.empty())
		LogInfo() << "Loading Visual: " << m_FileName;
#endif
}


GVisual::~GVisual(void)
{
#ifndef PUBLIC_RELEASE
	if(!m_FileName.empty())
		LogInfo() << "Deleting Visual: " << m_FileName;
#endif

	GASSERT(m_Vobs.empty(), "Visual being deleted is still used by at least one vob!");

	for(GBaseDrawable* d : m_RegisteredDrawables)
	{
		DestroyDrawableStates(d);

		// These should be deleted by the vob containing them!
		// TODO: Add a check to find out if there are any memleaks!
	}
}

/** Creates an extension-object of the right visual type if none was created before */
GVisual* GVisual::CreateExtensionVisual(zCVisual* visual)
{
	// Query if this was already created
	GVisual* vis = GVisual::QueryFromSource(visual);
	if (vis) 
		return vis; // Already created

	// Create new GVisual
	vis = Engine::Game->GetMainResources()->CreateVisualFrom(visual);

	// Add to global resource cache
	if (vis)
		REngine::ResourceCache->AddToCache(Toolbox::HashObject(visual), vis);

	return vis;
}

/** Tells all drawables they should re-get their cached pipeline states */
void GVisual::InvalidateStateCaches()
{
	for(GBaseDrawable* d : m_RegisteredDrawables)
	{
		d->ReaquireStateCache();
	}
}

/** Registers a drawable */
void GVisual::RegisterDrawable(GBaseDrawable* drawable)
{
	m_RegisteredDrawables.insert(drawable);
}

/** Unregisters a drawable */
void GVisual::UnregisterDrawable(GBaseDrawable* drawable)
{
	// Destroy any created pipeline states
	DestroyDrawableStates(drawable);

	// And remove from observer list
	m_RegisteredDrawables.erase(drawable);
}

/** Destroys the pipelinestates of the given drawable */
void GVisual::DestroyDrawableStates(GBaseDrawable* drawable)
{
	auto& set = m_CreatedPipelineStates[drawable];
	Toolbox::DeleteElements(set.PipelineStates);

	// Also remove from map, since it's empty anyways
	m_CreatedPipelineStates.erase(drawable);
}

/** Adds a vob using this visual (directly and indirectly).
Returns true if not already in the list. */
bool GVisual::AddVob(GVobObject* vob)
{
	size_t s = m_Vobs.size();
	m_Vobs.insert(vob);

	return s != m_Vobs.size();
}

/** Removes a vob using this visual (directly and indirectly).
Returns true, if found. */
bool GVisual::RemoveVob(GVobObject* vob)
{
	size_t s = m_Vobs.size();

	m_Vobs.erase(vob);

	return s != m_Vobs.size();
}