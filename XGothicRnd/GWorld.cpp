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
#include <RDynamicBufferCache.h>
#include "D3D7\MyDirect3DDevice7.h"
#include "GCamera.h"
#include <RD3D11SyncCheck.h>

GWorld::GWorld(zCWorld* sourceObject) : GzObjectExtension<zCWorld, GWorld>(sourceObject)
{
	m_BspTree = nullptr;
	m_IsFullyLoaded = false;
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
	m_BspTree = GBspTree::GetFromSource(m_SourceObject->GetBspTree());
	m_BspTree->SetContainedWorld(this);

	LogInfo() << "BSP-Tree initialization complete!";

	m_IsFullyLoaded = true;
}


/**
 * Draws the world from the current camerapperspective
 */
void GWorld::Render()
{
	GCamera* activeCamera = GCamera::GetActiveCamera();

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
	static std::vector<GVobObject*> s_DynamicDrawStateVobs;
	s_sortIndexList.clear();
	m_VobRenderList.clear();
	s_DynamicDrawStateVobs.clear();

	if(m_BspTree)
		m_BspTree->Draw(worldMeshQueue, m_VobRenderList, fogFar);

	// Done with this queue, process it asynchronously
	REngine::RenderingDevice->ProcessRenderQueue(worldMeshQueue);



	// Calculate the squared inverse farplane, so we don't have to do all these
	// expensive square-roots and divisions for LOD-Calculation
	float3 activeCameraPosition = activeCamera->GetCameraPosition();

	// Scale farZ down. Farplane usually is around 16000. Need to do this so farZ^2 won't get too big
	double farZSquared = activeCamera->GetFarZ() * activeCamera->GetFarZ();
	double farZMod = activeCamera->GetFarZ() * 0.00001; 
	double farPlaneSquaredInv = 1.0f / (farZMod * farZMod);

	// Extract render-instances
	for(GVobObject* vob : m_VobRenderList)
	{
		//if(vob->HasDynamicDrawState())
		{
			float3 vobPosition = vob->GetWorldMatrix().TranslationT();
			double distanceSquared = (vobPosition - activeCameraPosition).LengthSquared();

			if(distanceSquared < farZSquared)
			{
				double distanceSquaredMod = distanceSquared * 0.00001 * 0.00001;
				vob->MakeRenderInstances(s_sortIndexList, GConstants::RS_WORLD, distanceSquaredMod * farPlaneSquaredInv);
			}
		}
	}

	/*static std::vector<std::vector<RenderInstance>> s_sortIndexList_local;
	s_sortIndexList_local.resize(omp_get_max_threads());
	for(int i = 0; i < m_VobRenderList.size(); i++)
		s_sortIndexList_local.clear();

#pragma omp parallel sections
	{
	#pragma omp section
		{
			auto& list = s_sortIndexList_local[omp_get_thread_num()];
			list.clear();

			// Extract render-instances
			for(GVobObject* vob : m_VobRenderList)
			{
				if(vob->HasDynamicDrawState())
				{
					float3 vobPosition = vob->GetWorldMatrix().TranslationT();
					double distanceSquared = (vobPosition - activeCameraPosition).LengthSquared();

					if(distanceSquared < farZSquared)
					{
						double distanceSquaredMod = distanceSquared * 0.00001 * 0.00001;
						vob->MakeRenderInstances(list, GConstants::RS_WORLD, distanceSquaredMod * farPlaneSquaredInv);
					}
				}
			}

			#pragma omp critical (s_sortIndexList)
			s_sortIndexList.insert(s_sortIndexList.end(), list.begin(), list.end());
		}

	#pragma omp section
		{
		// Extract vobs with dynamic drawstates
	#pragma omp parallel
			{
				auto& list = s_sortIndexList_local[omp_get_thread_num()];
				list.clear();

				// Draw all the others
		#pragma omp for
				for(int i = 0; i < m_VobRenderList.size(); i++)
				{
					GVobObject* vob = m_VobRenderList[i];

					if(!vob->HasDynamicDrawState())
					{
						float3 vobPosition = vob->GetWorldMatrix().TranslationT();
						double distanceSquared = (vobPosition - activeCameraPosition).LengthSquared();

						if(distanceSquared < farZSquared)
						{
							double distanceSquaredMod = distanceSquared * 0.00001 * 0.00001;
							vob->MakeRenderInstances(list, GConstants::RS_WORLD, distanceSquaredMod * farPlaneSquaredInv);
						}
					}
				}

				// Reduce the lists to s_sortIndexList
				#pragma omp critical (s_sortIndexList)
				s_sortIndexList.insert(s_sortIndexList.end(), list.begin(), list.end());
			}
		}
	}*/

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

	Engine::Game->AddFrameDebugLine(std::string("Queues: ") + std::to_string(REngine::RenderingDevice->GetNumQueuesInUse()));
	Engine::Game->AddFrameDebugLine(std::string("DrawCalls: ") + std::to_string(REngine::RenderingDevice->GetNumRegisteredDrawCalls()));

	float tcpu, tgpu;
	__ctx_sync_check::GlobalSyncCheckStash.GetTimesLostAndReset(tcpu, tgpu);
	Engine::Game->AddFrameDebugLine(std::string("Buffer Updates: (CPU: ") + std::to_string(tcpu) + "ms) (GPU: " + std::to_string(tgpu) + "ms)");
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

/**
* The inventory-cells are actually consisting out of a lot of tiny world-objects with only
* one vob. This is a special render-call for these kinds of worlds, which acts according to
* the inventory-grid */
void GWorld::RenderInventoryCell()
{
	GASSERT(m_SourceObject->IsInventoryWorld(), "RenderInventoryCell() can only be used on inventory worlds!");

	// We should only have one vob in these worlds
	GASSERT(m_VobSet.size() == 1, "InventoryWorld contains a different number of vobs than 1!");

	// Queue for this single vob
	RRenderQueueID queue = REngine::RenderingDevice->AcquireRenderQueue();

	// Dynamic buffer for the frame information of this vob
	RCachedDynamicBuffer frameBuffer = REngine::DynamicBufferCache->GetDataBuffer(EBindFlags::B_CONSTANTBUFFER, sizeof(ConstantBuffers::PerFrameConstantBuffer), sizeof(ConstantBuffers::PerFrameConstantBuffer));
	
	ConstantBuffers::PerFrameConstantBuffer pfcb;
	pfcb.M_View = zCCamera::GetActiveCamera()->GetViewMatrix();
	pfcb.M_InverseView = pfcb.M_View.Invert();
	pfcb.M_Proj = zCCamera::GetActiveCamera()->GetProjectionMatrix().Transpose();
	pfcb.M_ViewProj = (pfcb.M_Proj * pfcb.M_View);

	// Push to GPU
	frameBuffer.Buffer->UpdateData(&pfcb);

	// Modify default pipeline-state for this vob
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
	RPipelineState* state = REngine::ResourceCache->GetCachedObject<RPipelineState>(GConstants::PipelineStates::BPS_INSTANCED_VOB_INVENTORY);
	
	sm.SetFromPipelineState(state);

	// The game sets the rendering-viewport to the FF-Pipe, get it.
	// TODO: Get this from the zCView itself?
	sm.SetViewport(MyDirect3DDevice7::GetActiveDevice()->GetViewport());

	// Update the state
	REngine::ResourceCache->RemoveFromCache<RPipelineState>(GConstants::PipelineStates::BPS_INSTANCED_VOB_INVENTORY);
	state = sm.MakeDrawCall(0,0);
	REngine::ResourceCache->AddToCache<RPipelineState>(GConstants::PipelineStates::BPS_INSTANCED_VOB_INVENTORY, state);

	// Just draw everything in this world, it should only be one vob anyways!
	for(GVobObject* vob : m_VobSet)
	{
		std::vector<RenderInstance> instances;
		vob->MakeRenderInstances(instances, GConstants::RS_INVENTORY);

		for(RenderInstance& inst : instances)
		{
			inst.m_Drawable->PushRenderStateCache(GConstants::RS_INVENTORY, queue, 0, 0);
		}
	}

	// Mark as free for next frame
	REngine::DynamicBufferCache->DoneWith(frameBuffer);
}