#pragma once

#include "d3d.h"
#include "MyDirect3DVertexBuffer7.h"
#include <stdio.h>
#include "../../shared/Logger.h"
#include "MyDirectDrawSurface7.h"
#include <REngine.h>
#include <RDevice.h>
#include <RBuffer.h>
#include <RDepthStencilState.h>
#include <RSamplerState.h>
#include <RBlendState.h>
#include <RRasterizerState.h>
#include "../FixedFunctionState.h"
#include "../VertexTypes.h"
#include <RVertexShader.h>
#include <RInputLayout.h>
#include <RPixelShader.h>
#include <RDynamicBufferCache.h>
#include <RViewport.h>
#include <RTools.h>
#include <RBufferCollection.h>
#include "../GGame.h"
#include "../Engine.h"
#include "../GTexture.h"

// Currently active device
class MyDirect3DDevice7;
__declspec(selectany) MyDirect3DDevice7* s_ActiveDevice;

const int DRAW_PRIM_INDEX_BUFFER_SIZE = 4096 * sizeof(VERTEX_INDEX);
const int DRAW_PRIM_VERTEX_BUFFER_SIZE = 4096 * GOTHIC_FVF_XYZRHW_DIF_T1_SIZE;

class MyDirect3DDevice7 : public IDirect3DDevice7 {
public:
    MyDirect3DDevice7(IDirect3D7* direct3D7, IDirect3DDevice7* direct3DDevice7){

		RefCount = 1;

		// The game initializes a couple of them, but keeps the first one working
		// Save this here for better accessability
		s_ActiveDevice = this;

		// Load the caps we created the "device" with
		FILE* f = fopen("system\\GD3D11\\data\\DeviceEnum.bin", "rb");
		if(!f)
		{
			LogError() << "Failed to open the system\\GD3D11\\data\\DeviceEnum.bin file. Can't fake a device for Gothic now!";
			return;
		}

		char desc[256]; 
		char name[256]; 

		fread(desc, 256, 1, f);
		fread(name, 256, 1, f);
		fread(&FakeDeviceDesc, sizeof(D3DDEVICEDESC7), 1, f);

		fclose(f);

		memset(BoundSurfaces, 0, sizeof(BoundSurfaces));

		InitFixedFunctionEmulator();

		
    }

	~MyDirect3DDevice7()
	{
		ClearFixedFunctionEmulatorResources();
	}

    /*** IUnknown methods ***/
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObj) {
        return S_OK; //return this->direct3DDevice7->QueryInterface(riid, ppvObj);
    }

    ULONG STDMETHODCALLTYPE AddRef() {
		RefCount++;
        return S_OK; //return this->direct3DDevice7->AddRef();
    }

    ULONG STDMETHODCALLTYPE Release() {
        if (0 == RefCount) {
            delete this;
			return 0;
        }

        return RefCount;
    }

    /*** IDirect3DDevice7 methods ***/
    HRESULT STDMETHODCALLTYPE GetCaps(LPD3DDEVICEDESC7 lpD3DDevDesc ) {
		/*ZeroMemory(lpD3DDevDesc, sizeof(D3DDEVICEDESC7));
		lpD3DDevDesc->dwDevCaps = D3DDEVCAPS_DRAWPRIMTLVERTEX | D3DDEVCAPS_FLOATTLVERTEX;
		lpD3DDevDesc->dwMaxTextureWidth = 4096;
		lpD3DDevDesc->dwMaxTextureHeight = 4096;
		lpD3DDevDesc->dwTextureOpCaps = D3DTEXOPCAPS_ADD | D3DTEXOPCAPS_MODULATE | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_SELECTARG2;*/

		// Tell Gothic what it wants to hear
		*lpD3DDevDesc = FakeDeviceDesc;

        return S_OK; //return this->direct3DDevice7->GetCaps(lpD3DDevDesc);
    }

    HRESULT STDMETHODCALLTYPE GetClipPlane(DWORD Index,float* pPlane) {
        return S_OK; //return this->direct3DDevice7->GetClipPlane(Index, pPlane);
    }

    HRESULT STDMETHODCALLTYPE SetClipPlane(DWORD dwIndex, D3DVALUE* pPlaneEquation) {
        return S_OK; //return this->direct3DDevice7->SetClipPlane(dwIndex, pPlaneEquation);
    }

    HRESULT STDMETHODCALLTYPE GetClipStatus(LPD3DCLIPSTATUS lpD3DClipStatus) {
        return S_OK; //return this->direct3DDevice7->GetClipStatus(lpD3DClipStatus);
    }

    HRESULT STDMETHODCALLTYPE SetClipStatus(LPD3DCLIPSTATUS lpD3DClipStatus) {
        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE GetDirect3D(IDirect3D7** ppD3D) {
        *ppD3D = NULL;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetInfo(DWORD dwDevInfoID, LPVOID pDevInfoStruct, DWORD dwSize) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetLight(DWORD dwLightIndex, LPD3DLIGHT7 lpLight) {
        return S_OK;
    }



    HRESULT STDMETHODCALLTYPE GetLightEnable(DWORD Index,BOOL* pEnable) {
        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE GetMaterial(LPD3DMATERIAL7 lpMaterial) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetMaterial(LPD3DMATERIAL7 lpMaterial) {
        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE GetRenderState(D3DRENDERSTATETYPE State,DWORD* pValue) {
        return S_OK;
    }



#define LOG_RENDERSTATE(s, v) if(State == s){LogInfo() << "Set " #s " to value " << v << " (uint), " << *(float *)&v << " (float)";}
#define LOG_RENDERSTATE_COLOR(s, v) if(State == s){LogInfo() << "Set " #s " to value " << v << " (uint), " << LogColorHelper(v) << " (D3DCOLOR)";}
#define LOG_UNIMPLMENTED_RENDERSTATE(x) {LogWarn() << "Unimplemented Renderstate : " << #x << " (uint: " << Value << ")";}

    HRESULT STDMETHODCALLTYPE SetRenderState(D3DRENDERSTATETYPE State,DWORD Value) {
		//DebugWrite("MyDirect3DDevice7::SetRenderState");

		// Extract the needed renderstates
		switch(State)
		{
		case D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGENABLE:
			FixedFunctionStageInfo.FF_FogWeight = Value != 0 ? 1.0f : 0.0f;
			break;

		case D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGSTART:
			FixedFunctionStageInfo.FF_FogNear = *(float *)&Value;
			break;

		case D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGEND:
			FixedFunctionStageInfo.FF_FogFar = *(float *)&Value;
			//LogInfo() << "Set fogFar to " << data->fogFar;
			break;

		case D3DRENDERSTATETYPE::D3DRENDERSTATE_FOGCOLOR:
			{
				//BYTE a = Value >> 24;
				BYTE r = (Value >> 16) & 0xFF;
				BYTE g = (Value >> 8 ) & 0xFF;
				BYTE b = Value & 0xFF;
				FixedFunctionStageInfo.FF_FogColor = Vector3(r / 255.0f, g / 255.0f, b / 255.0f);
			}
			break;

		case D3DRENDERSTATETYPE::D3DRENDERSTATE_AMBIENT:
			{
				//BYTE a = Value >> 24;
				BYTE r = (Value >> 16) & 0xFF;
				BYTE g = (Value >> 8 ) & 0xFF;
				BYTE b = Value & 0xFF;
				FixedFunctionStageInfo.FF_AmbientLighting = Vector3(r / 255.0f, g / 255.0f, b / 255.0f);
				
				// Does this enable the ambientlighting?
				//data->lightEnabled = 1.0f;
			}
			break;
			
		case D3DRENDERSTATE_ZWRITEENABLE			: DepthStencilState.DepthWriteEnabled = Value != 0; break;
		case D3DRENDERSTATE_ZENABLE            		: DepthStencilState.DepthBufferEnabled = Value != 0; break;
		case D3DRENDERSTATE_ALPHATESTENABLE    		: FixedFunctionStageInfo.SetGraphicsSwitch(GSWITCH_ALPHAREF, Value != 0);	break;
		case D3DRENDERSTATE_SRCBLEND           		: BlendState.SrcBlend = (RBlendStateInfo::EBlendFunc)Value; break;
		case D3DRENDERSTATE_DESTBLEND          		: BlendState.DestBlend = (RBlendStateInfo::EBlendFunc)Value; break;
		case D3DRENDERSTATE_CULLMODE           		: RasterizerState.CullMode = (RRasterizerStateInfo::ECullMode)Value; break;
		case D3DRENDERSTATE_ZFUNC              		: DepthStencilState.DepthBufferCompareFunc = (RDepthStencilStateInfo::ECompareFunc)Value; break;
		case D3DRENDERSTATE_ALPHAREF           		: FixedFunctionStageInfo.FF_AlphaRef = (float)Value / 255.0f; break; // Ref for masked
		case D3DRENDERSTATE_ALPHABLENDENABLE   		: BlendState.BlendEnabled = Value != 0; break;	
		case D3DRENDERSTATE_ZBIAS              		: RasterizerState.ZBias = Value; break;
		case D3DRENDERSTATE_TEXTUREFACTOR      		: FixedFunctionStageInfo.FF_TextureFactor = Vector4::FromColor(Value); break;
		case D3DRENDERSTATE_LIGHTING           		: FixedFunctionStageInfo.SetGraphicsSwitch(GSWITCH_LIGHING, Value != 0); break;

		default:
			break;
		}

        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE GetRenderTarget(LPDIRECTDRAWSURFACE7 *lplpRenderTarget ) {
		LogWarn() << "GetRenderTarget not supported!";
		*lplpRenderTarget = NULL;

        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE SetRenderTarget(LPDIRECTDRAWSURFACE7 lpNewRenderTarget, DWORD dwFlags) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetTexture(DWORD dwStage, LPDIRECTDRAWSURFACE7 *lplpTexture) {
		LogWarn() << "GetTexture not supported!";
        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE SetTexture(DWORD dwStage, LPDIRECTDRAWSURFACE7 lplpTexture) {	
		// Bind the texture
		MyDirectDrawSurface7* surface = (MyDirectDrawSurface7 *)lplpTexture;
		
		BoundSurfaces[dwStage] = surface;

        return S_OK; 
	}

    HRESULT STDMETHODCALLTYPE GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue) {
        return S_OK; 
	}

    HRESULT STDMETHODCALLTYPE SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value) {
		// Update stateblock
		
		switch(Type)
			{
			case D3DTSS_COLOROP: 
				if(Stage < 2)
					FixedFunctionStageInfo.FF_Stages[Stage].ColorOp = (FixedFunctionStage::EColorOp)Value;
				else
					LogWarn() << "Gothic uses more than 2 TextureStages!";
				break; 

			case D3DTSS_COLORARG1:
				if(Stage < 2)
					FixedFunctionStageInfo.FF_Stages[Stage].ColorArg1 = (FixedFunctionStage::ETextureArg)Value;
				break;	

			case D3DTSS_COLORARG2:
				if(Stage < 2)
					FixedFunctionStageInfo.FF_Stages[Stage].ColorArg2 = (FixedFunctionStage::ETextureArg)Value;
				break;

			case D3DTSS_ALPHAOP:
				if(Stage < 2)
					FixedFunctionStageInfo.FF_Stages[Stage].AlphaOp = (FixedFunctionStage::EColorOp)Value;
				 break;	   

			case D3DTSS_ALPHAARG1: 	
				if(Stage < 2)
					FixedFunctionStageInfo.FF_Stages[Stage].ColorArg1 = (FixedFunctionStage::ETextureArg)Value;
				break;

			case D3DTSS_ALPHAARG2:    
				if(Stage < 2)
					FixedFunctionStageInfo.FF_Stages[Stage].ColorArg2 = (FixedFunctionStage::ETextureArg)Value;
				break;

			case D3DTSS_BUMPENVMAT00: break;  
			case D3DTSS_BUMPENVMAT01: break;  
			case D3DTSS_BUMPENVMAT10: break;  
			case D3DTSS_BUMPENVMAT11: break;  
			case D3DTSS_TEXCOORDINDEX: 
				if(Value > 7) // This means that some other flag was set, and the only case that happens is for reflections
				{
					FixedFunctionStageInfo.SetGraphicsSwitch(GSWITCH_REFLECTIONS, true);
				}else
				{
					FixedFunctionStageInfo.SetGraphicsSwitch(GSWITCH_REFLECTIONS, false);
				}
				break;

			case D3DTSS_ADDRESS: 
				SamplerState.AddressU = (ETextureAddress)Value;
				SamplerState.AddressV = (ETextureAddress)Value;
				break;

			case D3DTSS_ADDRESSU:   
				SamplerState.AddressU = (ETextureAddress)Value; 
				break;	   

			case D3DTSS_ADDRESSV:   
				SamplerState.AddressV = (ETextureAddress)Value; 
				break;   

			case D3DTSS_BORDERCOLOR: break;   
			case D3DTSS_MAGFILTER: break;   
			case D3DTSS_MINFILTER: break;   
			case D3DTSS_MIPFILTER: break;   
			case D3DTSS_MIPMAPLODBIAS: break;
			case D3DTSS_MAXMIPLEVEL: break; 
			case D3DTSS_MAXANISOTROPY: break;
			case D3DTSS_BUMPENVLSCALE: break;
			case D3DTSS_BUMPENVLOFFSET: break;
			case D3DTSS_TEXTURETRANSFORMFLAGS: 
				break;
			}
        return S_OK; //return this->direct3DDevice7->SetTextureStageState(Stage, Type, Value);
    }

    HRESULT STDMETHODCALLTYPE GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix) {
        return S_OK; //return this->direct3DDevice7->GetTransform(State, pMatrix);
    }

    HRESULT STDMETHODCALLTYPE SetTransform(D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix) {
		//LogInfo() << "SetTransform: " << dtstTransformStateType;

		switch(dtstTransformStateType)
		{
		case D3DTRANSFORMSTATE_WORLD:
			FixedFunctionStageInfo.WorldMatrix = ((Matrix*)lpD3DMatrix)->Transpose();
			break;

		case D3DTRANSFORMSTATE_VIEW:
			FixedFunctionStageInfo.ViewMatrix = ((Matrix*)lpD3DMatrix)->Transpose();
			break;

		case D3DTRANSFORMSTATE_PROJECTION:
			FixedFunctionStageInfo.ProjMatrix = ((Matrix*)lpD3DMatrix)->Transpose();
			break;
		}

		FixedFunctionStageInfo.WorldViewProj = (FixedFunctionStageInfo.ProjMatrix * FixedFunctionStageInfo.ViewMatrix * FixedFunctionStageInfo.WorldMatrix);
		//FixedFunctionStageInfo.WorldViewProj = (FixedFunctionStageInfo.ViewMatrix * FixedFunctionStageInfo.ProjMatrix);

		return S_OK; //return this->direct3DDevice7->SetTransform(dtstTransformStateType, lpD3DMatrix);
    }

    HRESULT STDMETHODCALLTYPE GetViewport(LPD3DVIEWPORT7 lpViewport) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetViewport(LPD3DVIEWPORT7 lpViewport) {
		ViewportInfo vpinfo;
		vpinfo.TopLeftX = (float)(lpViewport->dwX);
		vpinfo.TopLeftY = (float)(lpViewport->dwY);
		vpinfo.Height = (float)(lpViewport->dwHeight);
		vpinfo.Width = (float)(lpViewport->dwWidth);
		vpinfo.MinZ = lpViewport->dvMinZ;
		vpinfo.MaxZ = lpViewport->dvMaxZ;

		// Try to get this from cache
		RViewport* vp = REngine::ResourceCache->GetCachedObject<RViewport>(Toolbox::HashObject(vpinfo));
		
		// Create new object if needed
		if(!vp)
		{
			vp = REngine::ResourceCache->CreateResource<RViewport>();
			vp->CreateViewport(vpinfo);
			REngine::ResourceCache->AddToCache(Toolbox::HashObject(vpinfo), vp);
		}

		// Apply this to ff-state
		FixedFunctionStageInfo.FF_TopLeftX = vpinfo.TopLeftX;
		FixedFunctionStageInfo.FF_TopLeftY = vpinfo.TopLeftY;
		FixedFunctionStageInfo.FF_Width = vpinfo.Width;
		FixedFunctionStageInfo.FF_Height = vpinfo.Height;


		Viewport = vp;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ApplyStateBlock(DWORD dwBlockHandle) {
		return S_OK;
    }

    HRESULT STDMETHODCALLTYPE BeginScene() {
		SetRenderQueueName("D3D7");
		REngine::RenderingDevice->OnFrameStart();
        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE BeginStateBlock() {
        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE CaptureStateBlock(DWORD dwBlockHandle) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Clear(DWORD dwCount, LPD3DRECT lpRects, DWORD dwFlags, D3DCOLOR dwColor, D3DVALUE dvZ, DWORD dwStencil) {
		BYTE a = dwColor >> 24;
		BYTE r = (dwColor >> 16) & 0xFF;
		BYTE g = (dwColor >> 8 ) & 0xFF;
		BYTE b = dwColor & 0xFF;

		ClearColor = Vector4(r / 255.0f, g/ 255.0f, b / 255.0f, a / 255.0f);
		//Engine::GraphicsEngine->Clear(float4(r / 255.0f, g/ 255.0f, b / 255.0f, a / 255.0f));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ComputeSphereVisibility(LPD3DVECTOR lpCenters, LPD3DVALUE lpRadii, DWORD dwNumSpheres, DWORD dwFlags, LPDWORD lpdwReturnValues) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CreateStateBlock(D3DSTATEBLOCKTYPE d3dsbType, LPDWORD lpdwBlockHandle) {
		return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DeleteStateBlock(DWORD dwBlockHandle) {
		return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, DWORD dwVertexTypeDesc, LPVOID lpvVertices, DWORD dwVertexCount, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE dptPrimitiveType, DWORD dwVertexTypeDesc, LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD dwVertexCount, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags) {
        return S_OK; 
    }


    HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer, DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags) {
		if(d3dptPrimitiveType == D3DPRIMITIVETYPE::D3DPT_TRIANGLEFAN)
			return S_OK;

		//DrawPrimIndexBuffer->UpdateBuffer(lpwIndices, dwIndexCount * sizeof(VERTEX_INDEX));

		//Engine::GraphicsEngine->DrawVOBDirect(((MyDirect3DVertexBuffer7 *)lpd3dVertexBuffer)->GetVertexBuffer(), DrawPrimIndexBuffer, dwIndexCount, dwStartVertex);
			
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DrawPrimitive(D3DPRIMITIVETYPE dptPrimitiveType, DWORD dwVertexTypeDesc, LPVOID lpvVertices, DWORD dwVertexCount, DWORD dwFlags) {
		if(dptPrimitiveType != D3DPT_TRIANGLEFAN) // Handle lines here
		{
			return S_OK;
		}

		RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();

		//sm.Invalidate();

		// Apply state of this fake-device to our statemachine
		AssignState();
		
		unsigned int offset = 0;
		switch(dwVertexTypeDesc)
		{
		case GOTHIC_FVF_XYZRHW_DIF_T1:
			sm.SetVertexShader(VS_XYZRHW_DIF_T1);
			sm.SetInputLayout(InputLayout_XYZRHW_DIF_T1);
			
			// Make buffer
			offset = PreprocessTriangleFan((Gothic_XYZRHW_DIF_T1_Vertex*)lpvVertices, dwVertexCount, ImmediateBufferCollection_XYZRHW_DIF_T1);

			sm.SetVertexBuffer(0, ImmediateBufferCollection_XYZRHW_DIF_T1.GetBuffer());
			break;

		case GOTHIC_FVF_XYZRHW_DIF_SPEC_T1:
			sm.SetVertexShader(VS_XYZRHW_DIF_SPEC_T1);
			sm.SetInputLayout(InputLayout_XYZRHW_DIF_SPEC_T1);

			// Make buffer
			offset = PreprocessTriangleFan((Gothic_XYZRHW_DIF_SPEC_T1_Vertex*)lpvVertices, dwVertexCount, ImmediateBufferCollection_XYZRHW_DIF_SPEC_T1);

			sm.SetVertexBuffer(0, ImmediateBufferCollection_XYZRHW_DIF_SPEC_T1.GetBuffer());
			break;

		default:
			return S_OK;
		}

		// Make drawcall. (x - 2)*3 because that is how many vertices a fan has as a triangle list.
		RPipelineState* drawcall = sm.MakeDrawCall((dwVertexCount - 2) * 3, offset);
		FramePipelineStates.push_back(drawcall);

		sm.Invalidate();

		// Get a new queue for each of these, since it's critical that the original code draws in order
		RRenderQueueID q = REngine::RenderingDevice->AcquireRenderQueue(false, RenderQueueName + " #" + std::to_string(RenderQueueIndex));
		RenderQueueIndex++;

		REngine::RenderingDevice->QueuePipelineState(drawcall, q);
		REngine::RenderingDevice->ProcessRenderQueue(q);

       return S_OK;
    }

	// Not used by Gothic!
    HRESULT STDMETHODCALLTYPE DrawPrimitiveStrided(D3DPRIMITIVETYPE dptPrimitiveType, DWORD dwVertexTypeDesc, LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD dwVertexCount, DWORD dwFlags) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DrawPrimitiveVB(D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer, DWORD dwStartVertex, DWORD dwNumVertices, DWORD dwFlags) {
		if(d3dptPrimitiveType < 4) // Handle lines here
		{
			return S_OK;
		}

		if (d3dptPrimitiveType != D3DPT_TRIANGLELIST)
			LogWarn() << "DP-VB: Unimplemented primitive type: " << d3dptPrimitiveType;
		//return S_OK;

		RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
		MyDirect3DVertexBuffer7* buffer = (MyDirect3DVertexBuffer7*)lpd3dVertexBuffer;
		D3DVERTEXBUFFERDESC desc;
		buffer->GetVertexBufferDesc(&desc);

		//sm.Invalidate();

		// Apply state of this fake-device to our statemachine
		AssignState();

		sm.SetVertexBuffer(0, buffer->GetEngineBuffer());


		switch(desc.dwFVF)
		{
		case GOTHIC_FVF_XYZ_DIF_T1:
			//return S_OK;
			sm.SetVertexShader(VS_XYZ_DIF_T1);
			sm.SetInputLayout(InputLayout_XYZ_DIF_T1);
			break;

		// Used for the sky
		case GOTHIC_FVF_XYZRHW_DIF_T1:
			sm.SetVertexShader(VS_XYZRHW_DIF_T1);
			sm.SetInputLayout(InputLayout_XYZRHW_DIF_T1);
			break;

		default:
			return S_OK;
		}

		// Make drawcall
		RPipelineState* drawcall = sm.MakeDrawCall(dwNumVertices, dwStartVertex);
		FramePipelineStates.push_back(drawcall);

		sm.Invalidate();

		// Get a new queue for each of these, since it's critical that the original code draws in order
		RRenderQueueID q = REngine::RenderingDevice->AcquireRenderQueue(false, RenderQueueName + " #" + std::to_string(RenderQueueIndex));
		RenderQueueIndex++;
		REngine::RenderingDevice->QueuePipelineState(drawcall, q);
		REngine::RenderingDevice->ProcessRenderQueue(q);

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE EndScene() {

		// Construct immediate buffer
		ImmediateBufferCollection_XYZRHW_DIF_T1.Construct(EBindFlags::B_VERTEXBUFFER, EUsageFlags::U_DYNAMIC);
		ImmediateBufferCollection_XYZRHW_DIF_SPEC_T1.Construct(EBindFlags::B_VERTEXBUFFER, EUsageFlags::U_DYNAMIC);

		Engine::Game->OnFrameEnd();
		REngine::RenderingDevice->OnFrameEnd();
		REngine::RenderingDevice->Present();

		for each (RPipelineState* ps in FramePipelineStates)
		{
			REngine::ResourceCache->DeleteResource(ps);
		}
		FramePipelineStates.clear();

		for(auto b : FFConstantBufferByHash)
		{
			REngine::ResourceCache->DeleteResource<RBuffer>(b.second);
		}
		FFConstantBufferByHash.clear();

		REngine::DynamicBufferCache->OnFrameEnded();
	
	return S_OK;
    }

    HRESULT STDMETHODCALLTYPE EndStateBlock(LPDWORD lpdwBlockHandle) {
        return S_OK; 
    }

	struct DeviceEnumInfo
	{
		LPD3DENUMPIXELFORMATSCALLBACK originalFn;
		LPVOID originalUserArg;
	};
	/** This function has to be used with a working D3D7-Device to spit out all textureformats gothic needs */
	static HRESULT WINAPI PixelFormatCallback( LPDDPIXELFORMAT fmt, LPVOID lpContext)
	{
		LPD3DENUMPIXELFORMATSCALLBACK fn = ((DeviceEnumInfo *)lpContext)->originalFn;
		static int num=0;
		LogInfo() << "Wrote PixelFormat #" << num;
		num++;

		FILE* f = fopen("system\\GD3D11\\data\\FormatEnum.bin", "ab");
		fwrite(fmt, sizeof(DDPIXELFORMAT), 1, f);
		fclose(f);

		return (*fn)(fmt, ((DeviceEnumInfo *)lpContext)->originalUserArg);
	}

    HRESULT STDMETHODCALLTYPE EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, LPVOID lpArg) {
		//FILE* f = fopen("system\\GD3D11\\data\\FormatEnum.bin", "wb");
		//fclose(f);

		// Gothic only calls this once, so saving the working format is fine
		FILE* f = fopen("system\\GD3D11\\data\\FormatEnum.bin", "rb");

		while(!feof(f))
		{
			DDPIXELFORMAT fmt;
			fread(&fmt, sizeof(DDPIXELFORMAT), 1, f);
			(*lpd3dEnumPixelProc)(&fmt, lpArg);
		} 

		fclose(f);

        return S_OK;
    }



    HRESULT STDMETHODCALLTYPE Load(LPDIRECTDRAWSURFACE7 lpDestTex, LPPOINT lpDestPoint, LPDIRECTDRAWSURFACE7 lpSrcTex, LPRECT lprcSrcRect, DWORD dwFlags) {
        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE MultiplyTransform(D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix) {
        return S_OK; 
    }

    HRESULT STDMETHODCALLTYPE PreLoad(LPDIRECTDRAWSURFACE7 lpddsTexture) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ValidateDevice(DWORD* pNumPasses) {
        return S_OK; 
    }

	HRESULT STDMETHODCALLTYPE LightEnable(DWORD Index,BOOL Enable) {
        return S_OK;
    }

	HRESULT STDMETHODCALLTYPE SetLight(DWORD dwLightIndex, LPD3DLIGHT7 lpLight) {
        return S_OK;
    }
	
	/**
	 * Returns the last clear-color
	 */
	const Vector4& GetClearColor()
	{
		return ClearColor;
	}

	/**
	 * Assigns the current state to the pipeline, except buffers and vertexshader/inputlayout
	 */
	void AssignState()
	{
		RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
		RResourceCache& cache = *REngine::ResourceCache;

		// Force backface-culling to off to get around a bug with the sky when looking into the sun
		RasterizerState.CullMode = RRasterizerStateInfo::CM_CULL_NONE;

		// Try to get some from cache
		RDepthStencilState* dss = cache.GetCachedObject<RDepthStencilState>(Toolbox::HashObject(DepthStencilState));
		RSamplerState* ss = cache.GetCachedObject<RSamplerState>(Toolbox::HashObject(SamplerState));
		RBlendState* bs = cache.GetCachedObject<RBlendState>(Toolbox::HashObject(BlendState));
		RRasterizerState* rs = cache.GetCachedObject<RRasterizerState>(Toolbox::HashObject(RasterizerState));

		// Create states if missing
		if(!dss)
		{
			dss = cache.CreateResource<RDepthStencilState>();
			dss->CreateState(DepthStencilState);
			cache.AddToCache(Toolbox::HashObject(DepthStencilState), dss);
		}

		if(!ss)
		{
			ss = cache.CreateResource<RSamplerState>();
			ss->CreateState(SamplerState);
			cache.AddToCache(Toolbox::HashObject(SamplerState), ss);
		}

		if(!bs)
		{
			bs = cache.CreateResource<RBlendState>();
			bs->CreateState(BlendState);
			cache.AddToCache(Toolbox::HashObject(BlendState), bs);
		}

		if(!rs)
		{
			rs = cache.CreateResource<RRasterizerState>();
			rs->CreateState(RasterizerState);
			cache.AddToCache(Toolbox::HashObject(RasterizerState), rs);
		}

		// Bind states
		sm.SetRasterizerState(rs);
		sm.SetBlendState(bs);
		sm.SetDepthStencilState(dss);
		sm.SetSamplerState(ss);

		// Bind textures
		for(int i=0;i<8;i++)
		{
			if(BoundSurfaces[i] 
				&& BoundSurfaces[i]->GetEngineTexture()
				&& BoundSurfaces[i]->GetEngineTexture()->GetTexture()->IsInitialized())
				sm.SetTexture(i, BoundSurfaces[i]->GetEngineTexture()->GetTexture(), EShaderType::ST_PIXEL);
			else if(BoundSurfaces[i])
				sm.SetTexture(i, nullptr, EShaderType::ST_PIXEL);
		}

		size_t ffStateHash = Toolbox::HashObject(FixedFunctionStageInfo);

		// Only update buffer if we changed something
		if(ffStateHash != LastBoundFFStateHash)
		{
			LastBoundFFStateHash = ffStateHash;

			// Check if we already have such a buffer
			auto it = FFConstantBufferByHash.find(ffStateHash);
			if(it == FFConstantBufferByHash.end())
			{
				// Create a new cached state
				RBuffer* b =REngine::ResourceCache->CreateResource<RBuffer>();
				b->Init(&FixedFunctionStageInfo, sizeof(FixedFunctionGraphicsState), sizeof(FixedFunctionGraphicsState), EBindFlags::B_CONSTANTBUFFER);

				FFConstantBufferByHash[ffStateHash] = b;
				LastBoundFFStateCB = b;
			}
			else
			{
				// Got one from cache
				LastBoundFFStateCB = (*it).second;
			}
		}

		// Set FF-State
		sm.SetConstantBuffer(0, LastBoundFFStateCB, EShaderType::ST_VERTEX);
		sm.SetConstantBuffer(0, LastBoundFFStateCB, EShaderType::ST_PIXEL);

		sm.SetPixelShader(PS_FixedFunctionEmulator);

		sm.SetViewport(Viewport);
		sm.SetPrimitiveTopology(EPrimitiveType::PT_TRIANGLE_LIST);
	}

	/** Initializes resources needed by the renderer */
	HRESULT InitFixedFunctionEmulator()
	{
		InputLayout_XYZ_DIF_T1 = REngine::ResourceCache->CreateResource<RInputLayout>();
		InputLayout_XYZRHW_DIF_T1 = REngine::ResourceCache->CreateResource<RInputLayout>();
		InputLayout_XYZRHW_DIF_SPEC_T1 = REngine::ResourceCache->CreateResource<RInputLayout>();
		InputLayout_XYZ_NRM_T1 = REngine::ResourceCache->CreateResource<RInputLayout>();

		char path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, path);

		
		VS_XYZ_DIF_T1 = RTools::LoadShader<RVertexShader>("system\\GD3D11\\Shaders\\VS_XYZ_DIF_T1.hlsl", "VS_XYZ_DIF_T1");
		VS_XYZRHW_DIF_T1 = RTools::LoadShader<RVertexShader>("system\\GD3D11\\Shaders\\VS_XYZRHW_DIF_T1.hlsl", "VS_XYZRHW_DIF_T1");
		VS_XYZRHW_DIF_SPEC_T1 = RTools::LoadShader<RVertexShader>("system\\GD3D11\\Shaders\\VS_XYZRHW_DIF_SPEC_T1.hlsl", "VS_XYZRHW_DIF_SPEC_T1");
		VS_XYZ_NRM_T1 = RTools::LoadShader<RVertexShader>("system\\GD3D11\\Shaders\\VS_XYZ_NRM_T1.hlsl", "VS_XYZ_NRM_T1");
		PS_FixedFunctionEmulator = RTools::LoadShader<RPixelShader>("system\\GD3D11\\Shaders\\PS_FixedFunctionPipe.hlsl", "PS_FixedFunctionEmulator");

		const INPUT_ELEMENT_DESC layout_XYZ_DIF_T1[] =
		{
			{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
			{ "DIFFUSE", 0, FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
		};

		const INPUT_ELEMENT_DESC layout_XYZRHW_DIF_T1[] =
		{
			{ "POSITION", 0, FORMAT_R32G32B32A32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
			{ "DIFFUSE", 0, FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
		};

		const INPUT_ELEMENT_DESC layout_XYZRHW_DIF_SPEC_T1[] =
		{
			{ "POSITION", 0, FORMAT_R32G32B32A32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
			{ "DIFFUSE", 0, FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
			{ "SPECULAR", 0, FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
		
		};

		const INPUT_ELEMENT_DESC layout_XYZ_NRM_T1[] =
		{
			{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
		};

		InputLayout_XYZ_DIF_T1->CreateInputLayout(VS_XYZ_DIF_T1, layout_XYZ_DIF_T1, ARRAYSIZE(layout_XYZ_DIF_T1));
		InputLayout_XYZRHW_DIF_T1->CreateInputLayout(VS_XYZRHW_DIF_T1, layout_XYZRHW_DIF_T1, ARRAYSIZE(layout_XYZRHW_DIF_T1));
		InputLayout_XYZRHW_DIF_SPEC_T1->CreateInputLayout(VS_XYZRHW_DIF_SPEC_T1, layout_XYZRHW_DIF_SPEC_T1, ARRAYSIZE(layout_XYZRHW_DIF_SPEC_T1));
		//InputLayout_XYZ_NRM_T1->CreateInputLayout(VS_XYZ_DIF_T1, layout_XYZ_NRM_T1, ARRAYSIZE(layout_XYZ_NRM_T1));
		
		//DrawPrimVertexBuffer = REngine::DynamicBufferCache->GetDataBuffer(EBindFlags::B_VERTEXBUFFER, DRAW_PRIM_VERTEX_BUFFER_SIZE, 1);

		LastBoundFFStateCB = nullptr;
		LastBoundFFStateHash = 0;

		return S_OK;
	}

	void ClearFixedFunctionEmulatorResources()
	{
		REngine::ResourceCache->DeleteResource(InputLayout_XYZRHW_DIF_SPEC_T1);
		REngine::ResourceCache->DeleteResource(InputLayout_XYZRHW_DIF_T1);
		REngine::ResourceCache->DeleteResource(InputLayout_XYZ_DIF_T1);
		REngine::ResourceCache->DeleteResource(InputLayout_XYZ_NRM_T1);

		REngine::ResourceCache->DeleteResource(VS_XYZRHW_DIF_SPEC_T1);
		REngine::ResourceCache->DeleteResource(VS_XYZRHW_DIF_T1);
		REngine::ResourceCache->DeleteResource(VS_XYZ_DIF_T1);
		REngine::ResourceCache->DeleteResource(VS_XYZ_NRM_T1);
		REngine::ResourceCache->DeleteResource(PS_FixedFunctionEmulator);

		for(auto b : FFConstantBufferByHash)
		{
			REngine::ResourceCache->DeleteResource<RBuffer>(b.second);
		}
	}

	/** Processes a trianglefan of the given vertex-type for drawing. Returns offset to the immediate buffer */
	template<typename T>
	unsigned int PreprocessTriangleFan(T* vertices, unsigned int numVertices, RBufferCollection<T>& collection)
	{
		// Convert the fan to a list
		// Static to help with cache, since this has to be fast
		static std::vector<T> list;
		list.clear();

		RTools::TriangleFanToList((T *)vertices, numVertices, list);

		// Put them into a buffer
		return collection.AddData(&list[0], list.size());
	}

	/** Returns the active viewport, set in the FF-Pipe */
	RViewport* GetViewport()
	{
		return Viewport;
	}

	/** Sets the name the renderqueues should use */
	void SetRenderQueueName(const std::string& name)
	{
		RenderQueueName = name;
		RenderQueueIndex = 0;
	}

	/** Singleton-like accessor to the currently active fake-device */
	static MyDirect3DDevice7* GetActiveDevice()
	{
		return s_ActiveDevice;
	}

private:
	
	D3DDEVICEDESC7 FakeDeviceDesc;
	int RefCount;

	// State-values
	RDepthStencilStateInfo DepthStencilState;
	RSamplerStateInfo SamplerState;
	RBlendStateInfo BlendState;
	RRasterizerStateInfo RasterizerState;
	FixedFunctionGraphicsState FixedFunctionStageInfo;
	size_t LastBoundFFStateHash;
	RBuffer* LastBoundFFStateCB;
	RViewport* Viewport;
	MyDirectDrawSurface7* BoundSurfaces[8];
	std::map<size_t, RBuffer*> FFConstantBufferByHash;
	RInputLayout* InputLayout_XYZ_DIF_T1;
	RInputLayout* InputLayout_XYZRHW_DIF_T1;
	RInputLayout* InputLayout_XYZRHW_DIF_SPEC_T1;
	RInputLayout* InputLayout_XYZ_NRM_T1;
	RVertexShader* VS_XYZ_DIF_T1;
	RVertexShader* VS_XYZRHW_DIF_T1;
	RVertexShader* VS_XYZRHW_DIF_SPEC_T1;
	RVertexShader* VS_XYZ_NRM_T1;
	RPixelShader* PS_FixedFunctionEmulator;
	RBufferCollection<Gothic_XYZRHW_DIF_T1_Vertex> ImmediateBufferCollection_XYZRHW_DIF_T1;
	RBufferCollection<Gothic_XYZRHW_DIF_SPEC_T1_Vertex> ImmediateBufferCollection_XYZRHW_DIF_SPEC_T1;
	
	// Debug
	//std::pair<unsigned int, RBuffer*> DrawPrimVertexBuffer;

	// Last clear-color we got. Gothic only clears one target, so we can simply grab it here
	// before rendering
	Vector4 ClearColor;

	// List of all rendered pipelinestates. Cleared at frame-end.
	std::vector<RPipelineState*> FramePipelineStates;

	// Name to use for current RenderingQueues. These will get an index appended for each new queue
	std::string RenderQueueName;
	unsigned int RenderQueueIndex;
};
