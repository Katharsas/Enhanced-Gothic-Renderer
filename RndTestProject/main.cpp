
#include <crtdbg.h>

#define NOMINMAX
#include <Windows.h>
#include "../Shared/Logger.h"
#include <REngine.h>

#include "Window.h"
#include <RBuffer.h>
#include <RVertexShader.h>
#include <RPixelShader.h>
#include <RInputLayout.h>
#include <RRasterizerState.h>
#include <RSamplerState.h>
#include <RBlendState.h>
#include <RDepthStencilState.h>

#pragma comment(lib, "RenderingAPI.lib")

#if defined(DEBUG) | defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif

using namespace DirectX;

struct SimpleVertex
{
	Vector3 Position;
	Vector2 TexCoord;
	DWORD Diffuse;
};

struct PerFrameConstantBuffer
{
	XMMATRIX M_View;
	XMMATRIX M_Proj;
	XMMATRIX M_ViewProj;	 
};

RPipelineState* g_CubeState = nullptr;
RBuffer* g_perInstanceCB = nullptr;

struct PerInstanceConstantBuffer
{
	XMMATRIX M_World;
};

RBuffer* MakeBox(float extends, DWORD color)
{
	SimpleVertex vx[36];
	int i=0;

	vx[i].Position = Vector3(-1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,-1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,-1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,-1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,-1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,-1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,-1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,-1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,-1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,-1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,-1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	vx[i].Position = Vector3(-1.0f,1.0f,1.0f);
	vx[i++].Diffuse = color;

	// Loop through all vertices and apply the extends
	for(i = 0; i < 36; i++)
	{
		vx[i].Position *= extends;
	}

	RBuffer* buffer = REngine::ResourceCache->CreateResource<RBuffer>();
	buffer->Init(&vx[0], sizeof(vx), sizeof(SimpleVertex));

	return buffer;
}

void __cdecl RenderCallback()
{
	static float angle = 0.0f;
	angle += 0.001f;

	INT2 res = INT2(1024, 800);
	Matrix proj = XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.0f), (float)res.x/(float)res.y, 0.1f, 1000.0f);
	Matrix view = XMMatrixLookAtLH(Vector3(0.0f,0.0f,-5.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f, 1.0f, 0.0f));
	Matrix id = XMMatrixRotationRollPitchYaw(0, angle, 0);

	PerInstanceConstantBuffer picb;
	//picb.M_World = Matrix(id * view * proj).Transpose();
	//picb.M_World = proj.Transpose() * view.Transpose() * id.Transpose();
	picb.M_World = id.Transpose();

	void* data;
	g_perInstanceCB->Map(&data);
		memcpy(data, &picb, sizeof(picb));
	g_perInstanceCB->Unmap();

	REngine::RenderingDevice->DrawPipelineState(*g_CubeState);
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	//MessageBox(nullptr, L"---", L"---", MB_OK);
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF /* | _CRTDBG_CHECK_ALWAYS_DF */ );
	//_CrtSetBreakAlloc(35776);
#endif

	Log::Clear();
	REngine::InitializeEngine();
	REngine::RenderingDevice->CreateDevice();

	INT2 res = INT2(1024, 800);

	
	Matrix proj = XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.0f), (float)res.x/(float)res.y, 0.1f, 1000.0f);
	Matrix view = XMMatrixLookAtLH(Vector3(0.0f,0.0f,-5.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f, 1.0f, 0.0f));
	Matrix id = XMMatrixRotationRollPitchYaw(0, 0, 0);

	RBuffer* perInstanceCB = REngine::ResourceCache->CreateResource<RBuffer>();
	
	PerInstanceConstantBuffer picb;
	picb.M_World = id;

	perInstanceCB->Init(&picb, sizeof(picb), sizeof(picb), EBindFlags::B_CONSTANTBUFFER, EUsageFlags::U_DYNAMIC, ECPUAccessFlags::CA_WRITE);
	g_perInstanceCB = perInstanceCB;

	PerFrameConstantBuffer pfcb;
	pfcb.M_View = view.Transpose();
	pfcb.M_Proj = proj.Transpose();
	pfcb.M_ViewProj = proj.Transpose() * view.Transpose();
	//pfcb.M_ViewProj = view.Transpose() * proj.Transpose();
	//pfcb.M_ViewProj = Matrix(view * proj).Transpose();
	//pfcb.M_ViewProj = proj * view;

	RBuffer* cube = MakeBox(1.0f, 0xFFFFFFFF);
	RBuffer* perFrameCB = REngine::ResourceCache->CreateResource<RBuffer>();
	perFrameCB->Init(&pfcb, sizeof(pfcb), sizeof(pfcb), EBindFlags::B_CONSTANTBUFFER, EUsageFlags::U_DYNAMIC, ECPUAccessFlags::CA_WRITE);

	RVertexShader* vs = REngine::ResourceCache->CreateResource<RVertexShader>();
	vs->LoadShader("testVS.hlsl");

	RPixelShader* ps = REngine::ResourceCache->CreateResource<RPixelShader>();
	ps->LoadShader("testPS.hlsl");

	const INPUT_ELEMENT_DESC layout1[] =
	{
		{ "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
		{ "DIFFUSE", 0, FORMAT_R8G8B8A8_UNORM, 0, 0xFFFFFFFF, INPUT_PER_VERTEX_DATA, 0 },
	};
	RInputLayout* layout = REngine::ResourceCache->CreateResource<RInputLayout>();
	layout->CreateInputLayout(vs, layout1, ARRAYSIZE(layout1));

	RDepthStencilState* dss = REngine::ResourceCache->CreateResource<RDepthStencilState>();
	RDepthStencilStateInfo dssinfo = RDepthStencilStateInfo().SetDefault();
	dssinfo.DepthBufferEnabled = false;
	dss->CreateState(dssinfo);

	RSamplerState* ss = REngine::ResourceCache->CreateResource<RSamplerState>();
	ss->CreateState(RSamplerStateInfo().SetDefault());

	RBlendState* bs = REngine::ResourceCache->CreateResource<RBlendState>();
	bs->CreateState(RBlendStateInfo().SetDefault());

	RRasterizerState* rs = REngine::ResourceCache->CreateResource<RRasterizerState>();
	RRasterizerStateInfo rsinfo = RRasterizerStateInfo().SetDefault();
	rsinfo.CullMode = RRasterizerStateInfo::CM_CULL_NONE;
	rs->CreateState(rsinfo);

	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();
	sm.SetBlendState(bs);
	sm.SetRasterizerState(rs);
	sm.SetSamplerState(ss);
	sm.SetDepthStencilState(dss);

	sm.SetConstantBuffer(0, perFrameCB, EShaderType::ST_VERTEX);
	sm.SetConstantBuffer(1, perInstanceCB, EShaderType::ST_VERTEX);

	sm.SetInputLayout(layout);
	sm.SetPixelShader(ps);
	sm.SetVertexShader(vs);
	sm.SetVertexBuffer(0, cube);

	g_CubeState = sm.MakeDrawCall(36);



	LogInfo() << "Starting game!";

	Window Wnd;

	LogInfo() << "Creating window";
	Wnd.Create("Test", 50, 50, res.x, res.y, hInstance);

	REngine::RenderingDevice->SetWindow(Wnd.GetHandle());

	LogInfo() << "Starting logic...";
	Wnd.Run(RenderCallback);

	REngine::UninitializeEngine();

	return 0;
}