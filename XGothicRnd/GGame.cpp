#include "pch.h"
#include "GGame.h"
#include <REngine.h>
#include <RVertexShader.h>
#include <RInputLayout.h>
#include <RPixelShader.h>
#include <RTools.h>
#include "VertexTypes.h"
#include "RDepthStencilState.h"
#include "RBlendState.h"
#include "RSamplerState.h"
#include "RRasterizerState.h"
#include <RResourceCache.h>
#include <RBuffer.h>
#include "GConstantBuffers.h"
#include "GConstants.h"
#include "GWorld.h"
#include "GCamera.h"
#include "zCCamera.h"
#include "CGameManager.h"
#include <RViewport.h>
#include "GMainResources.h"
#include "zCView.h"
#include <iomanip>
#include <limits>
#include "zCInput.h"
#include "zGlobalHk.h"
#include "GVisual.h"
#include "zEngineHooks.h"
#include "zCSkyController.h"
#include "zClassDef.h"

GGame::GGame(void)
{
	m_ActiveWorld = nullptr;
	m_MainResources = new GMainResources();
	
}


GGame::~GGame(void)
{
	delete m_MainResources;
}

/**
 * Initializes basic resources like default pipeline states
 */
bool GGame::Initialize()
{
	// Load shaders:
	// Default vertexshader for WorldMesh
	RVertexShader* vs_ExWMM = RTools::LoadShader<RVertexShader>("system\\GD3D11\\Shaders\\VS_ExWorldMeshMain.hlsl", "VS_ExWorldMeshMain");
	RVertexShader* vs_ExVobInst = RTools::LoadShader<RVertexShader>("system\\GD3D11\\Shaders\\VS_ExVobInstanced.hlsl", "VS_ExVobInstanced");
	RVertexShader* vs_ExSkeletal = RTools::LoadShader<RVertexShader>("system\\GD3D11\\Shaders\\VS_ExSkelInstanced.hlsl", "VS_ExSkelInstanced");
	RVertexShader* vs_Particles = RTools::LoadShader<RVertexShader>("system\\GD3D11\\Shaders\\VS_Particles.hlsl", "VS_Particles");
	RInputLayout* ilExT = RTools::CreateInputLayoutFor<ExTVertexStruct>(vs_ExWMM);
	RInputLayout* ilExTInst = RTools::CreateInputLayoutFor<ExTVertexStructInstanced>(vs_ExVobInst);
	RInputLayout* ilExTSkel = RTools::CreateInputLayoutFor<ExTSkelVertexStruct>(vs_ExSkeletal);
	
	// Simple texturing pixelshader
	RPixelShader* ps_Simple = RTools::LoadShader<RPixelShader>("system\\GD3D11\\Shaders\\PS_Simple.hlsl", "PS_Simple");
	RPixelShader* ps_World = RTools::LoadShader<RPixelShader>("system\\GD3D11\\Shaders\\PS_World.hlsl", "PS_World");
	RPixelShader* ps_Particle = RTools::LoadShader<RPixelShader>("system\\GD3D11\\Shaders\\PS_Particle.hlsl", "PS_Particle");
	
	// TODO: Use/implement definition-system
	RPixelShader* ps_WorldMasked = RTools::LoadShader<RPixelShader>("system\\GD3D11\\Shaders\\PS_World.hlsl", 
		"PS_World_Masked",
		{ {"DO_ALPHATEST"} });

	// Make default per-frame constant buffer
	RBuffer* perFrameCB = REngine::ResourceCache->CreateResource<RBuffer>();
	REngine::ResourceCache->AddToCache(GConstants::ConstantBuffers::CB_WORLDPERFRAME, perFrameCB);
	perFrameCB->Init(nullptr, 
		sizeof(ConstantBuffers::PerFrameConstantBuffer), 
		sizeof(ConstantBuffers::PerFrameConstantBuffer), 
		EBindFlags::B_CONSTANTBUFFER, 
		EUsageFlags::U_DYNAMIC, 
		ECPUAccessFlags::CA_WRITE);

	// Create default states
	RDepthStencilState* defaultDSS = nullptr;
	RBlendState* defaultBS = nullptr;
	RSamplerState* defaultSS = nullptr;
	RRasterizerState* defaultRS = nullptr;
	RTools::MakeDefaultStates(&defaultDSS, &defaultSS, &defaultBS, &defaultRS);

	// Create default viewport
	ViewportInfo vpinfo;
	vpinfo.TopLeftX = 0;
	vpinfo.TopLeftY = 0;
	vpinfo.Height = (float)REngine::RenderingDevice->GetOutputResolution().y;
	vpinfo.Width = (float)REngine::RenderingDevice->GetOutputResolution().x;
	vpinfo.MinZ = 0.0f;
	vpinfo.MaxZ = 1.0f;

	// Try to get this from cache
	RViewport* vp = REngine::ResourceCache->GetCachedObject<RViewport>(Toolbox::HashObject(vpinfo));

	// Create new object if needed
	if(!vp)
	{
		vp = REngine::ResourceCache->CreateResource<RViewport>();
		vp->CreateViewport(vpinfo);
		REngine::ResourceCache->AddToCache(Toolbox::HashObject(vpinfo), vp);
	}

	// Create default pipeline states
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();

	sm.SetBlendState(defaultBS);
	sm.SetRasterizerState(defaultRS);
	sm.SetSamplerState(defaultSS);
	sm.SetDepthStencilState(defaultDSS);
	sm.SetViewport(vp);
	sm.SetConstantBuffer(0, perFrameCB, EShaderType::ST_VERTEX);
	sm.SetConstantBuffer(0, perFrameCB, EShaderType::ST_PIXEL);
	sm.SetInputLayout(ilExT);
	sm.SetPixelShader(ps_World);
	sm.SetVertexShader(vs_ExWMM);
	
	// Make default drawcall for a worldmesh
	RPipelineState* worldDefState = sm.MakeDrawCall(0,0);
	REngine::ResourceCache->AddToCache(GConstants::PipelineStates::BPS_WORLDMESH, worldDefState);

	// Set states for instanced vobs
	sm.SetVertexShader(vs_ExVobInst);
	sm.SetInputLayout(ilExTInst);
	
	// Make default drawcall for an instanced vob
	RPipelineState* instVobDefState = sm.MakeDrawCallIndexedInstanced(0, 0);
	REngine::ResourceCache->AddToCache(GConstants::PipelineStates::BPS_INSTANCED_VOB, instVobDefState);

	// Make default drawcall for skeletal meshes
	sm.SetVertexShader(vs_ExSkeletal);
	sm.SetInputLayout(ilExTSkel);

	RPipelineState* skelDefState = sm.MakeDrawCallIndexedInstanced(0, 0);
	REngine::ResourceCache->AddToCache(GConstants::PipelineStates::BPS_SKEL_MESH, skelDefState);

	// Make default drawcall for skeletal meshes
	sm.SetVertexShader(vs_Particles);
	sm.SetInputLayout(nullptr);
	sm.SetPixelShader(ps_Particle);

	RPipelineState* particleDefState = sm.MakeDrawCall(0, 0);
	REngine::ResourceCache->AddToCache(GConstants::PipelineStates::BPS_PARTICLES, particleDefState);

	// This is called right after we got the games window. Hook the message-callback.
	zGlobalHk::PlaceWndHook(REngine::RenderingDevice->GetOutputWindow());

	return true;
}

/**
* Called when the game started to load a new world
*/
void GGame::OnLoadWorld(zCWorld* world, const std::string& file, zTWorldLoadMode mode)
{
	// Disable input while loading so I can use the mouse
	zCInput::SetInputEnabled(false);

	// If the current world is completely different, switch it (STAT/STARTUP only happens at initial loading of a world
	if(mode == zTWorldLoadMode::ZTW_LOAD_GAME_SAVED_STAT || mode == zTWorldLoadMode::ZTW_LOAD_GAME_STARTUP)
	{
		// Everything is being reloaded. Not critically needed, but better to get rid of all potentially old data
		// DeleteLoadedVisualExtensions();

		// Everything is being reloaded
		//DeleteLoadedVisualExtensions();

		delete m_ActiveWorld; // Delete, in case it is old
		m_ActiveWorld = GWorld::GetFromSource(world); // Get a new instance

		// Since this is loading a completely different world
	}
	else
	{
		LogInfo() << "Merging load into existing GWorld";
	}
}

/**
* Called when the world got disposed 
*/
void GGame::OnWorldDisposed(zCWorld* world)
{
	
}

/**
 * Called when a zCWorld was loaded
 */
void GGame::OnWorldLoaded(zCWorld* world, const std::string& file, zTWorldLoadMode mode)
{
	if(mode != zTWorldLoadMode::ZTW_LOAD_GAME_SAVED_DYN)
		return;


	LogInfo() << "Loaded world: " << file;

	m_ActiveWorld->OnWorldLoaded(mode);

	// Enable input again (Disabled at load-start)
	//zCInput::SetInputEnabled(true);
}

/**
* Called when a new frame started
*/
void GGame::OnFrameStart()
{
	// Clear debug text so it can be filled up again this frame
	m_FrameDebugLines.clear();

	// Update pages in global buffers if needed
	m_MainResources->GetExPagedVertexBuffer()->RebuildPages();
	m_MainResources->GetExPagedIndexBuffer()->RebuildPages();
	m_MainResources->GetExSkelPagedVertexBuffer()->RebuildPages();
	m_MainResources->GetExSkelPagedIndexBuffer()->RebuildPages();

	/*static int frameCtr = 0;
	frameCtr++;

	if(frameCtr == 5)
	{
		CGameManager* gameManager = CGameManager::GetCGameManager();

		if(gameManager)
			gameManager->Read_Savegame(1);
	}*/

	// Do safe jobs here (Like deleting objects)
	ExecuteSaveFunctions();
}

/**
* Called when the frame ended
*/
void GGame::OnFrameEnd()
{
	// Update timer
	m_FPSTimer.Update();

	DrawStatistics();
}

/**
 * Called when a new frame should be rendered
 */
void GGame::OnRender()
{
#ifdef HOOK_RENDERING
	// TODO: Move
	OnFrameStart();

	// Update per-frame buffer
	RBuffer* perFrameCB = REngine::ResourceCache->GetCachedObject<RBuffer>(GConstants::ConstantBuffers::CB_WORLDPERFRAME);
	GCamera* camera = GCamera::GetFromSource(zCCamera::GetActiveCamera());

	// Can't render if we don't have a camera
	if(!camera)
		return;

	camera->GetSourceObject()->UpdateProjectionMatrix();
	camera->GetSourceObject()->Activate();

	// Update per-frame scene parameters
	ConstantBuffers::PerFrameConstantBuffer pfcb;
	pfcb.M_View = camera->GetSourceObject()->GetViewMatrix();
	pfcb.M_InverseView = pfcb.M_View.Invert();
	pfcb.M_Proj = camera->GetSourceObject()->GetProjectionMatrix().Transpose();

	pfcb.M_ViewProj = (pfcb.M_Proj * pfcb.M_View);

	

	// Fog and other values
	ExtractSkyParameters(pfcb);

	perFrameCB->UpdateData(&pfcb);

	if(m_ActiveWorld)
		m_ActiveWorld->Render();

#endif
	
}

/**
* Puts the sky-parameters into the constantbuffer
*/
void GGame::ExtractSkyParameters(ConstantBuffers::PerFrameConstantBuffer& cb)
{
	// Fog values
	zCSkyController* sky = zCSkyController::GetActiveSkyControler();

	if (sky && strcmp(sky->GetClassDef()->GetClassName(), "zCSkyControler_Outdoor") == 0)
	{
		zCSkyController_Outdoor* oSky = (zCSkyController_Outdoor*)sky;

		// Get fog planes and color
		oSky->GetFogPlanes(cb.PF_SceneParams.S_FogStart, cb.PF_SceneParams.S_FogEnd);
		cb.PF_SceneParams.S_FogColor = float4::FromColor(oSky->GetFogColor());

		// TODO: Implement a better switch for turning off the sky
		// these values are simply 0.0f when the sky isn't rendered
		if(cb.PF_SceneParams.S_FogStart == 0.0f && cb.PF_SceneParams.S_FogEnd == 0.0f)
		{
			cb.PF_SceneParams.S_FogStart = FLT_MAX;
			cb.PF_SceneParams.S_FogEnd = FLT_MAX;
		}
	}
	else
	{
		// Disable fob
		cb.PF_SceneParams.S_FogStart = FLT_MAX;
		cb.PF_SceneParams.S_FogEnd = FLT_MAX;
	}

	cb.PF_SceneParams.S_FogRange = cb.PF_SceneParams.S_FogEnd - cb.PF_SceneParams.S_FogStart;
}


/**
* Returns the time the last frame took in ms
*/
float GGame::GetFrameTimeMS()
{
	return m_FPSTimer.GetDelta();
}

/**
* Returns the Frames per second we are rendering at
*/
float GGame::GetFramesPerSecond()
{
	return 1.0f / m_FPSTimer.GetDelta();
}

/**
* Adds a single line to this frames debug-output
*/
void GGame::AddFrameDebugLine(const std::string& line)
{
	m_FrameDebugLines += line + "\n";
}

/**
* Draws the statistics of the renderer using ingame-methods
*/
void GGame::DrawStatistics()
{
	static int s = 0;
	s++;

	if ((s % 35) == 0)
	{
		std::stringstream stat;
		stat << "FPS: " << std::fixed << std::setprecision(2) << GetFramesPerSecond() << " (" << 1000.0f / GetFramesPerSecond() << "ms)" << "\n";
		SetWindowText(REngine::RenderingDevice->GetOutputWindow(), stat.str().c_str());
	}

	if (zCView::GetSessionView())
		zCView::GetSessionView()->Print(INT2(0,0), m_FrameDebugLines);
	

	return;
	/*std::stringstream stat;
	stat << "FPS: " << std::fixed << std::setprecision(2) << GetFramesPerSecond() << " (" << 1000.0f / GetFramesPerSecond() << "ms)" << "\n"
		<< "Queues: " << REngine::RenderingDevice->GetNumQueuesInUse() << "\n"
		<< "DrawCalls: " << REngine::RenderingDevice->GetNumRegisteredDrawCalls() << "\n";
	
	if(m_ActiveWorld)
		stat << "NumVobs: " << m_ActiveWorld->GetNumRegisteredVobs();

	stat << REngine::RenderingDevice->GetStateMachine().GetChangesCounts().ProduceString();
	memset(&REngine::RenderingDevice->GetStateMachine().GetChangesCounts(), 0, sizeof(RStateMachine::ChangesCountStruct));

	if (zCView::GetSessionView())
		zCView::GetSessionView()->Print(INT2(0,0), stat.str());*/
}

/**
* Called when we got a message from the games window
*/
void GGame::OnWindowMessage(HWND hwnd, DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		OnKeyEvent((byte)wParam, true);
		break;

	case WM_KEYUP:
		OnKeyEvent((byte)wParam, false);
		break;
	}
}

/**
* Called when we got a keydown or keyup-event
*/
void GGame::OnKeyEvent(byte key, bool pressed)
{
	switch (key)
	{
	case VK_F11:
		if(pressed)
		{
			// Toggle the games input-capturing on/off
			static bool inputOn = true;
			inputOn = !inputOn;
			zCInput::SetInputEnabled(inputOn);
		}
		break;

	case VK_F6:
		zCInput::SetInputEnabled(false);
		break;
	}
}

/**
* Kills all loaded visuals. Needs to be called before loading, since gothic reuses it's objects
* and I don't want to.
*/
void GGame::DeleteLoadedVisualExtensions()
{
	LogInfo() << "Cleaning loaded visuals";

	// Copy map, so we can delete from the internal one
	std::unordered_map<size_t, GVisual*> visuals = GVisual::GetFullCacheMap();
	for(auto& a : visuals)
	{
		delete a.second;
	}
}

/**
* Puts the given function into a queue so it can be executed when the code reaches a safe place.
* This is very useful for operations which may have something active on an other thread.
* These are executed right after beginning rendering a new frame.
*/
void GGame::QueueSafeFunction(std::function<void()> fn)
{
	m_SaveFunctions.push_back(fn);
}

/** 
* Executes the function stored as "save"
*/
void GGame::ExecuteSaveFunctions()
{
	for(auto fn : m_SaveFunctions)
	{
		fn();
	}

	m_SaveFunctions.clear();
}