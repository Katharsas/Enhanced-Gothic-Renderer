#pragma once
#include "MyDirect3DVertexBuffer7.h"
#include <REngine.h>
#include <RDynamicBufferCache.h>

class MyDynamicDirect3DVertexBuffer7 : public MyDirect3DVertexBuffer7
{
public:
	MyDynamicDirect3DVertexBuffer7(const D3DVERTEXBUFFERDESC& originalDesc) : MyDirect3DVertexBuffer7(originalDesc)
	{
		ProxyLockedData = nullptr;
	}

	HRESULT STDMETHODCALLTYPE Lock(DWORD dwFlags, LPVOID* lplpData, LPDWORD lpdwSize) 
	{
		/*if(!Engine::Game->GetRenderSettings().m_AllowD3D7Proxy)
		{
			*lplpData = new byte[OriginalDesc.dwSize];
			ProxyLockedData = (byte*)*lplpData;

			if(lpdwSize)
				*lpdwSize = OriginalDesc.dwSize;
			return S_OK;
		}*/

		// Get new buffer
		auto newBuffer = REngine::DynamicBufferCache->GetDataBuffer(EBindFlags::B_VERTEXBUFFER, ComputeFVFSize(OriginalDesc.dwFVF) * OriginalDesc.dwNumVertices, ComputeFVFSize(OriginalDesc.dwFVF));

		// Give back the old buffer
		if(DynamicBuffer.Buffer)
			REngine::DynamicBufferCache->DoneWith(DynamicBuffer);

		DynamicBuffer = newBuffer;

		if(!DynamicBuffer.Buffer)
		{
			LogError() << "Failed to get dynamic vertexbuffer from cache!";
			return E_FAIL;
		}

		// Now we can lock it
		DynamicBuffer.Buffer->Map(lplpData);
		
		if(lpdwSize)
			*lpdwSize = OriginalDesc.dwSize;

        return S_OK;
    }

	HRESULT STDMETHODCALLTYPE Unlock() 
	{
		/*if(!Engine::Game->GetRenderSettings().m_AllowD3D7Proxy)
		{
			delete[] ProxyLockedData;
			return S_OK;
		}*/

		if(!DynamicBuffer.Buffer)
		{
			LogWarn() << "Unlock called on corrupted dynamic vertexbuffer!";
			return E_FAIL;
		}

		DynamicBuffer.Buffer->Unmap();

        return S_OK;
    }

	/** Returns the real engine-buffer */
	RBuffer* GetEngineBuffer()
	{
		return DynamicBuffer.Buffer;
	}

private:
	/** Current dynamic buffer. This is managed by the dynamic buffer-cache. */
	RCachedDynamicBuffer DynamicBuffer;

	/** Only used when d3d7-proxy is disabled */
	byte* ProxyLockedData;
};