#include "pch.h"
#include "GWorld.h"
#include "GBspTree.h"
#include "zCWorld.h"
#include "GVobObject.h"
#include "Engine.h"
#include "GGame.h"
#include "GMainResources.h"
#include <RBuffer.h>
#include "zCVob.h"
#include "GVisual.h"
#include "zCVisual.h"
#include <algorithm>
#include "GBaseDrawable.h"
#include "zCSkyController.h"
#include "zCCamera.h"
#include <RTools.h>

GWorld::GWorld(zCWorld* sourceObject) : GzObjectExtension<zCWorld, GWorld>(sourceObject)
{
	m_BspTree = nullptr;
}


GWorld::~GWorld(void)
{
	delete m_BspTree;
	m_BspTree = nullptr;
}

/** 
 * Called when the game finished loading the world
 */
void GWorld::OnWorldLoaded(zTWorldLoadMode mode)
{
	if(m_BspTree)
		return;

	LogInfo() << "Initialiting BSP-Tree";

	// Get data from bsp-tree
	m_BspTree = new GBspTree(m_SourceObject->GetBspTree());

	LogInfo() << "BSP-Tree initialization complete!";
}


/**
 * Draws the world from the current camerapperspective
 */
void GWorld::Render()
{
	zCSkyController_Outdoor* sky = zCSkyController_Outdoor::GetActiveSkyControllerAsOutdoor();
	float fogNear = FLT_MAX, fogFar = FLT_MAX;
	if(sky)
		sky->GetFogPlanes(fogNear, fogFar);


	// Draw the first parts of the sky before getting the queues
	DrawSkyPre();

	RRenderQueueID worldMeshQueue = REngine::RenderingDevice->AcquireRenderQueue(true);
	RRenderQueueID vobsQueue = REngine::RenderingDevice->AcquireRenderQueue(true);

	// Push indices to the vobs and their sorting creterium into this list
	// Faster than sorting the list of vobs

	static std::vector<RenderInstance> s_sortIndexList;
	s_sortIndexList.clear();
	m_VobRenderList.clear();

	if(m_BspTree)
		m_BspTree->Draw(worldMeshQueue, m_VobRenderList, fogFar);

	// Done with this queue, process it asynchronously
	REngine::RenderingDevice->ProcessRenderQueue(worldMeshQueue);

	// TODO: Get rid of this pass?
	for(GVobObject* vob : m_VobRenderList)
	{
		vob->MakeRenderInstances(s_sortIndexList);
	}

	// Sort the list by visual
	std::sort(s_sortIndexList.begin(), s_sortIndexList.end(),
		[](const RenderInstance& a, const RenderInstance& b)
		{
			// Order by visual
			return a.m_SortKey < b.m_SortKey;
		}
	);


	// Make sure the buffer is big enough
	RBuffer* instanceDataBuffer = Engine::Game->GetMainResources()->GetVobInstanceBuffer();
	if(instanceDataBuffer->GetSizeInBytes() < s_sortIndexList.size() * instanceDataBuffer->GetStructuredByteSize())
		LEB(instanceDataBuffer->UpdateData(nullptr, s_sortIndexList.size() * instanceDataBuffer->GetStructuredByteSize()));
	
	// Map the buffer to put the instance data in there and push the renderstates
	VobInstanceInfo* instanceData;
	LEB(instanceDataBuffer->Map((void**)&instanceData));

	unsigned int i=0;
	unsigned int visualStart = 0;
	for(auto& instance : s_sortIndexList)
	{

		// Copy the instance info
		memcpy(instanceData + i, instance.m_Instance, sizeof(VobInstanceInfo));

		// Only push the last vob of its kind. Always push the last one.
		if (i == s_sortIndexList.size() - 1
			|| instance.m_SortKey != s_sortIndexList[i + 1].m_SortKey
			|| !instance.m_Drawable->SupportsInstancing())
		{
			// Push instances of the drawable with the right amount of instances
			instance.m_Drawable->PushRenderStateCache(GConstants::ERenderStage::RS_WORLD,
				vobsQueue,
				visualStart,
				instance.m_Drawable->SupportsInstancing() ? (i + 1 - visualStart) : 1);

			visualStart = i + 1;
		}

		i++;
	}

	LEB(instanceDataBuffer->Unmap());

	// Done with this queue, process it asynchronously
	REngine::RenderingDevice->ProcessRenderQueue(vobsQueue);
	
	// Then push the other parts of the sky into the following queues
	DrawSkyPost();
}

/**
* Draws the games original sky
*/
void GWorld::DrawSkyPre()
{
	zCSkyController* sky = zCSkyController::GetActiveSkyControler();
	if (!sky)
		return;

	// Do this just in case it hasn't been done before
	zCCamera::GetActiveCamera()->Activate();

	// Enter new clear-color for the main buffers
	// TODO: Move this, when MRTs are needed
	REngine::RenderingDevice->SetMainClearValues(RTools::DWORDToFloat4(sky->GetBackgroundColor()));

	sky->RenderSkyPre();
}

/**
* Draws the games original sky
*/
void GWorld::DrawSkyPost()
{
	zCSkyController* sky = zCSkyController::GetActiveSkyControler();
	if (!sky)
		return;

	// Do this just in case it hasn't been done before
	zCCamera::GetActiveCamera()->Activate();

	sky->RenderSkyPost(true);
}

/** Registers a zCVob into the world */
void GWorld::AddVob(zCVob* vob)
{
	if(m_SourceObject->IsInventoryWorld())
		return; // TODO: Need to render the inventory!

	if(!vob->GetVisual())
		return; // Don't need vobs without visual

	if(vob->GetHomeWorld() != m_SourceObject)
		return; // Don't need inventory vobs

	if(!GVisual::QueryFromSource(vob->GetVisual()))
	{
		//return;
		//LogInfo() << "Loaded visual (Deferred): " << vob->GetVisual()->GetObjectName() << " (0x" << vob->GetVisual() << ")";

		// Create our extension-object from this and add it to cache
		GVisual* vis = Engine::Game->GetMainResources()->CreateVisualFrom(vob->GetVisual());

		if(!vis)
			return; // Don't take vobs with unsupported visuals
		
		REngine::ResourceCache->AddToCache(Toolbox::HashObject(vob->GetVisual()), vis);		
	}
	GVobObject* vobj = GVobObject::GetFromSource(vob);

	m_VobSet.insert(vobj);
}

/** Removes a zCVob from the world */
bool GWorld::RemoveVob(zCVob* vob)
{
	if(m_SourceObject->IsInventoryWorld())
		return false; // TODO: Need to render the inventory!

	if(!vob->GetVisual())
		return false; // Don't need vobs without visual

	if(vob->GetHomeWorld() != m_SourceObject)
		return false; // Don't need inventory vobs

	GVobObject* vobj = GVobObject::QueryFromSource(vob);

	// Safety. Sometimes this seems to happens on calls from oCNPC_Disable.
	/*if (vobj && vobj != vob->GetVobObject())
	{
		LogWarn() << "Vob not initialized with GVobObject!";
		return false;
	}*/

	// TODO: Remove this and fix the above bug!
	/*auto fn = [&](){
		__try
		{
			auto fn3 = [&](){
				if(vobj)
				{
					m_VobSet.erase(vobj);
					delete vobj;
				}
			};
			fn3();
		}
		__except(true)
		{
			auto fn2 = [](){
				LogWarn() << "Exception while deleting vob!"; };
			fn2();
		}
	};

	fn();*/

	if(vobj)
	{
		size_t size = m_VobSet.size();
		m_VobSet.erase(vobj);

		if(size == m_VobSet.size())
			return false; // Couldn't find it in the level

		//delete vobj;
	}

	return true;
}

/** Returns the number of vobs registered in here */
unsigned int GWorld::GetNumRegisteredVobs()
{
	return m_VobSet.size();
}