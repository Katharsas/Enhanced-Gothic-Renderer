#pragma once
#include "MyDirect3DVertexBuffer7.h"
#include <REngine.h>
#include <RDynamicBufferCache.h>

class MyDynamicDirect3DVertexBuffer7 : public MyDirect3DVertexBuffer7
{
public:
	MyDynamicDirect3DVertexBuffer7(const D3DVERTEXBUFFERDESC& originalDesc) : MyDirect3DVertexBuffer7(originalDesc)
	{
		DynamicBuffer = nullptr;
	}

	HRESULT STDMETHODCALLTYPE Lock(DWORD dwFlags, LPVOID* lplpData, LPDWORD lpdwSize) 
	{
		// Get new buffer
		auto newBuffer = REngine::DynamicBufferCache->GetDataBuffer(EBindFlags::B_VERTEXBUFFER, ComputeFVFSize(OriginalDesc.dwFVF) * OriginalDesc.dwNumVertices, ComputeFVFSize(OriginalDesc.dwFVF));

		// Give back the old buffer
		if(DynamicBuffer)
			REngine::DynamicBufferCache->DoneWith(DynamicBuffer, DynamicBufferFrame, EBindFlags::B_VERTEXBUFFER);

		// Overwrite old buffer
		DynamicBufferFrame = newBuffer.first;
		DynamicBuffer = newBuffer.second;

		if(!DynamicBuffer)
		{
			LogError() << "Failed to get dynamic vertexbuffer from cache!";
			return E_FAIL;
		}

		// Now we can lock it
		DynamicBuffer->Map(lplpData);
		
		if(lpdwSize)
			*lpdwSize = OriginalDesc.dwSize;

        return S_OK;
    }

	HRESULT STDMETHODCALLTYPE Unlock() 
	{
		if(!DynamicBuffer)
		{
			LogWarn() << "Unlock called on corrupted dynamic vertexbuffer!";
			return E_FAIL;
		}

		DynamicBuffer->Unmap();

        return S_OK;
    }

	/** Returns the real engine-buffer */
	RBuffer* GetEngineBuffer()
	{
		return DynamicBuffer;
	}

private:
	/** Current dynamic buffer. This is managed by the dynamic buffer-cache. */
	RBuffer* DynamicBuffer;
	unsigned int DynamicBufferFrame;
};