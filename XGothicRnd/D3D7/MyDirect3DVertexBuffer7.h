#pragma once
#ifndef MYDIRECT3DVERTEXBUFFER7_H
#define MYDIRECT3DVERTEXBUFFER7_H

#include "../pch.h"
#include <d3d.h>
#include "../Shared/Logger.h"
#include <vector>
#include <RBuffer.h>
#include <REngine.h>
#include <RResourceCache.h>
#include "../VertexTypes.h"

class MyDirect3DVertexBuffer7 : public IDirect3DVertexBuffer7 {
public:
    MyDirect3DVertexBuffer7(const D3DVERTEXBUFFERDESC& originalDesc) {
		// Save original desc
		OriginalDesc = originalDesc;

		// Start with 1 reference
		RefCount = 1;

		EngineBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
		EngineBuffer->Init(nullptr, originalDesc.dwNumVertices * ComputeFVFSize(originalDesc.dwFVF), ComputeFVFSize(originalDesc.dwFVF), RAPI::EBindFlags::B_VERTEXBUFFER, RAPI::EUsageFlags::U_DYNAMIC, RAPI::ECPUAccessFlags::CA_WRITE);
    }

	~MyDirect3DVertexBuffer7()
	{
		RAPI::REngine::ResourceCache->DeleteResource(EngineBuffer);
		//if(EngineBuffer)
		//	RAPI::REngine::ResourceCache->DeleteResource(EngineBuffer);
	}

	
    /*** IUnknown methods ***/
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObj) {
        //DebugWrite("MyDirect3DVertexBuffer7::QueryInterface\n");
        //return this->direct3DVertexBuffer7->QueryInterface(riid, ppvObj);

		LogError() << "QueryInterface on Vertexbuffer not supported!";
		// Lets hope this never gets called
		return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        RefCount++;

		return RefCount;
    }

    ULONG STDMETHODCALLTYPE Release() {
		RefCount--;

        ULONG count = RefCount;
        if (0 == count) {
            delete this;
			return 0;
        }

        return count;
    }

    /*** IDirect3DVertexBuffer7 methods ***/
    HRESULT STDMETHODCALLTYPE GetVertexBufferDesc(LPD3DVERTEXBUFFERDESC lpVBDesc) {
		if(lpVBDesc)*lpVBDesc = OriginalDesc;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Lock(DWORD dwFlags, LPVOID* lplpData, LPDWORD lpdwSize) {

		if (OriginalDesc.dwFVF == GOTHIC_FVF_XYZRHW_DIF_T1)
		{
			Vertices.resize(OriginalDesc.dwNumVertices);
			*lplpData = Vertices.data();

			return S_OK;
		}

		if (OriginalDesc.dwFVF == GOTHIC_FVF_XYZ_DIF_T1)
		{
			VerticesXYZ_DIF_T1.resize(OriginalDesc.dwNumVertices);
			*lplpData = VerticesXYZ_DIF_T1.data();

			return S_OK;
		}

		if(!EngineBuffer->Map(lplpData))
			LogWarn() << "Failed to map buffer!";

		if(lpdwSize)
			*lpdwSize = EngineBuffer->GetSizeInBytes();

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Optimize(LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags)
	{
		// Not needed
		return S_OK;
	}

    HRESULT STDMETHODCALLTYPE ProcessVertices(DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, LPDIRECT3DVERTEXBUFFER7 lpSrcBuffer, DWORD dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags)
	{
		LogWarn() << "Unimplemented method: MyDirect3DVertexBuffer7::ProcessVertices";
		return S_OK;
	}

    HRESULT STDMETHODCALLTYPE ProcessVerticesStrided(DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags)
	{
		LogWarn() << "Unimplemented method: MyDirect3DVertexBuffer7::ProcessVerticesStrided";
		return S_OK;
	}

    HRESULT STDMETHODCALLTYPE Unlock() {
		
		if (OriginalDesc.dwFVF == GOTHIC_FVF_XYZRHW_DIF_T1)
		{
			void* data;
			EngineBuffer->Map(&data);
			memcpy(data, Vertices.data(), sizeof(Gothic_XYZRHW_DIF_T1_Vertex) * Vertices.size());
		}

		if (OriginalDesc.dwFVF == GOTHIC_FVF_XYZ_DIF_T1)
		{
			void* data;
			EngineBuffer->Map(&data);
			memcpy(data, VerticesXYZ_DIF_T1.data(), sizeof(Gothic_XYZ_DIF_T1_Vertex) * VerticesXYZ_DIF_T1.size());
		}

		EngineBuffer->Unmap();

        return S_OK;
    }

	static int ComputeFVFSize( DWORD fvf )
	{
		//Those -inc s are the offset for the vertexptr to get to the data you want..
		//i.e. :if you want the normal data you can do vertexptr+nromalinc and you have a pointer to it
		int normalinc=0;
		int texcoordinc=0;
		int size=0;
		DWORD test = fvf;
		//test which fvf-code are included in fvf:

		if((fvf & D3DFVF_XYZ)==D3DFVF_XYZ) {
			size+=3*sizeof(float);
			test &= ~D3DFVF_XYZ;
		}
		else if((fvf & D3DFVF_XYZRHW)==D3DFVF_XYZRHW){
			size+=4*sizeof(float);
			test &= ~D3DFVF_XYZRHW;
		}
		if((fvf & D3DFVF_NORMAL)==D3DFVF_NORMAL)
		{
			normalinc=size;	
			size+=3*sizeof(float);
			test &= ~D3DFVF_NORMAL;

		}
		if((fvf & D3DFVF_DIFFUSE)==D3DFVF_DIFFUSE){ 
			size+=sizeof(D3DCOLOR);
			test &= ~D3DFVF_DIFFUSE;
		}
		if((fvf & D3DFVF_SPECULAR)==D3DFVF_SPECULAR){
			size+=sizeof(D3DCOLOR);
			test &= ~D3DFVF_SPECULAR;
		}
		if((fvf & D3DFVF_TEX1)==D3DFVF_TEX1)
		{
			texcoordinc=size;
			size+=2*sizeof(float);
			test &= ~D3DFVF_TEX1;
		}else if((fvf & D3DFVF_TEX2)==D3DFVF_TEX2)
		{
			texcoordinc=size;
			size+=2*sizeof(float)*2;
			test &= ~D3DFVF_TEX2;

		}else
		if((fvf & D3DFVF_TEX3)==D3DFVF_TEX3)
		{
			texcoordinc=size;
			size+=2*sizeof(float)*3;
			test &= ~D3DFVF_TEX3;
		}else
		if((fvf & D3DFVF_TEX4)==D3DFVF_TEX4)
		{
			texcoordinc=size;
			size+=2*sizeof(float)*4;
			test &= ~D3DFVF_TEX4;
		}else
		if((fvf & D3DFVF_TEX5)==D3DFVF_TEX5)
		{
			texcoordinc=size;
			size+=2*sizeof(float)*5;
			test &= ~D3DFVF_TEX5;
			LogInfo() << "FVF Contains: D3DFVF_TEX5";
		}else
		if((fvf & D3DFVF_TEX6)==D3DFVF_TEX6)
		{
			texcoordinc=size;
			size+=2*sizeof(float)*6;
			test &= ~D3DFVF_TEX6;
		}else
		if((fvf & D3DFVF_TEX7)==D3DFVF_TEX7)
		{
			texcoordinc=size;
			size+=2*sizeof(float)*7;
			test &= ~D3DFVF_TEX7;
		}else
		if((fvf & D3DFVF_TEX8)==D3DFVF_TEX8)
		{
			texcoordinc=size;
			size+=2*sizeof(float)*8;
			test &= ~D3DFVF_TEX8;
		}

		if(test != 0)
			LogWarn() << "FVF contains unknown bits! " << test << " leftover";


		//Here is the uncompleted code for the other fvfs...

		return size;
	}

	/** Returns the number of vertices inside this buffer */
	int GetNumVertices()
	{
		return OriginalDesc.dwNumVertices;
	}

	/** Returns the real engine-buffer */
	virtual RAPI::RBuffer* GetEngineBuffer()
	{
		return EngineBuffer;
	}

protected:

	/** Original desc D3D7 created the buffer with */
	D3DVERTEXBUFFERDESC OriginalDesc;

	/** Referencecount on this */
	int RefCount;

	/** Real buffer */
	RAPI::RBuffer* EngineBuffer;

	// TEMP
	std::vector<Gothic_XYZRHW_DIF_T1_Vertex> Vertices;
	std::vector<Gothic_XYZ_DIF_T1_Vertex> VerticesXYZ_DIF_T1;
};

#endif
