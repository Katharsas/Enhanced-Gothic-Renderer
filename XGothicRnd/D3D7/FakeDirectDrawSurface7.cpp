#include "pch.h"
#include "FakeDirectDrawSurface7.h"
#include <ddraw.h>
#include <RTexture.h>
#include "MyDirectDrawSurface7.h"
#include "../Shared/Toolbox.h"

#undef max
#undef min

using namespace RAPI;

FakeDirectDrawSurface7::FakeDirectDrawSurface7()
{
	RefCount = 1;
	Data = NULL;
	DataLocal = NULL;
}

void FakeDirectDrawSurface7::InitFakeSurface(const DDSURFACEDESC2* desc, MyDirectDrawSurface7* resource, int mipLevel)
{
	OriginalDesc = *desc;
	this->Resource = resource;
	MipLevel = mipLevel;
}

unsigned char * FakeDirectDrawSurface7::GetCachedData()
{
	return Data;
}

void FakeDirectDrawSurface7::DeleteCachedData()
{
	delete[] Data;
	Data = NULL;
}

HRESULT FakeDirectDrawSurface7::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	return S_OK; //return originalSurface->QueryInterface( riid, ppvObj );
}

ULONG FakeDirectDrawSurface7::AddRef()
{
	RefCount++;
	return RefCount;
}

ULONG FakeDirectDrawSurface7::Release()
{
	RefCount--;

	if(RefCount == 0)
	{
		delete[] Data;
		delete this;
		return 0;
	}

	return RefCount;
}

HRESULT FakeDirectDrawSurface7::AddAttachedSurface( LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface )
{
	AttachedSurfaces.push_back((FakeDirectDrawSurface7 *)lpDDSAttachedSurface);
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::AddOverlayDirtyRect( LPRECT lpRect )
{
	return S_OK; //return originalSurface->AddOverlayDirtyRect( lpRect );
}

HRESULT FakeDirectDrawSurface7::Blt( LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
{
	// Gothic never really blts
	return S_OK; //return originalSurface->Blt( lpDestRect, ((FakeDirectDrawSurface7 *)lpDDSrcSurface)->GetOriginalSurface(), lpSrcRect, dwFlags, lpDDBltFx );
}

HRESULT FakeDirectDrawSurface7::BltBatch( LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags )
{
	return S_OK; //return originalSurface->BltBatch( lpDDBltBatch, dwCount, dwFlags );
}

HRESULT FakeDirectDrawSurface7::BltFast( DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans )
{
	return S_OK; //return originalSurface->BltFast( dwX, dwY, lpDDSrcSurface, lpSrcRect, dwTrans );
}

HRESULT FakeDirectDrawSurface7::DeleteAttachedSurface( DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface )
{
	return S_OK; //return originalSurface->DeleteAttachedSurface( dwFlags, lpDDSAttachedSurface );
}

HRESULT FakeDirectDrawSurface7::EnumAttachedSurfaces( LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback )
{
	return S_OK; //return originalSurface->EnumAttachedSurfaces( lpContext, lpEnumSurfacesCallback );
}

HRESULT FakeDirectDrawSurface7::EnumOverlayZOrders( DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpfnCallback )
{
	return S_OK; //return originalSurface->EnumOverlayZOrders( dwFlags, lpContext, lpfnCallback );
}

HRESULT FakeDirectDrawSurface7::Flip( LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride, DWORD dwFlags )
{
	return S_OK; // Dont actually flip this
}

HRESULT FakeDirectDrawSurface7::GetAttachedSurface( LPDDSCAPS2 lpDDSCaps2, LPDIRECTDRAWSURFACE7* lplpDDAttachedSurface )
{
	if(AttachedSurfaces.empty())
		return E_FAIL;

	*lplpDDAttachedSurface = (LPDIRECTDRAWSURFACE7)AttachedSurfaces[0]; // Mipmap chains only have one entry
	AttachedSurfaces[0]->AddRef();
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::GetBltStatus( DWORD dwFlags )
{
	return S_OK; //return originalSurface->GetBltStatus( dwFlags );
}

HRESULT FakeDirectDrawSurface7::GetCaps( LPDDSCAPS2 lpDDSCaps2 )
{
	*lpDDSCaps2 = OriginalDesc.ddsCaps;

	return S_OK; //return originalSurface->GetCaps( lpDDSCaps2 );
}

HRESULT FakeDirectDrawSurface7::GetClipper( LPDIRECTDRAWCLIPPER* lplpDDClipper )
{
	return S_OK; //return originalSurface->GetClipper( lplpDDClipper );
}

HRESULT FakeDirectDrawSurface7::GetColorKey( DWORD dwFlags, LPDDCOLORKEY lpDDColorKey )
{
	return S_OK; //return originalSurface->GetColorKey( dwFlags, lpDDColorKey );
}

HRESULT FakeDirectDrawSurface7::GetDC( HDC* lphDC )
{
	return S_OK; //return originalSurface->GetDC( lphDC );
}

HRESULT FakeDirectDrawSurface7::GetFlipStatus( DWORD dwFlags )
{
	return S_OK; //return originalSurface->GetFlipStatus( dwFlags );
}

HRESULT FakeDirectDrawSurface7::GetOverlayPosition( LPLONG lplX, LPLONG lplY )
{
	return S_OK; //return originalSurface->GetOverlayPosition( lplX, lplY );
}

HRESULT FakeDirectDrawSurface7::GetPalette( LPDIRECTDRAWPALETTE* lplpDDPalette )
{
	return S_OK; //return originalSurface->GetPalette( lplpDDPalette );
}

HRESULT FakeDirectDrawSurface7::GetPixelFormat( LPDDPIXELFORMAT lpDDPixelFormat )
{
	return S_OK; //return originalSurface->GetPixelFormat( lpDDPixelFormat );
}

HRESULT FakeDirectDrawSurface7::GetSurfaceDesc( LPDDSURFACEDESC2 lpDDSurfaceDesc )
{
	*lpDDSurfaceDesc = OriginalDesc;
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::Initialize( LPDIRECTDRAW lpDD, LPDDSURFACEDESC2 lpDDSurfaceDesc )
{
	return S_OK; //return originalSurface->Initialize( lpDD, lpDDSurfaceDesc );
}

HRESULT FakeDirectDrawSurface7::IsLost()
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::Lock( LPRECT lpDestRect, LPDDSURFACEDESC2 lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent )
{
	*lpDDSurfaceDesc = OriginalDesc;

	// Compute size of this mip
	int px = (int)std::max(1.0f, floor(OriginalDesc.dwWidth / pow(2.0f, MipLevel)));
	int py = (int)std::max(1.0f, floor(OriginalDesc.dwHeight / pow(2.0f, MipLevel)));

	lpDDSurfaceDesc->dwWidth = px;
	lpDDSurfaceDesc->dwHeight = py;

	// Half everything if this is 16-bit
	int redBits = Toolbox::GetNumberOfBits(OriginalDesc.ddpfPixelFormat.dwRBitMask);
	if(redBits == 5)
	{
		// Allocate some temporary data
		unsigned int size = RBaseTexture::ComputeSizeInBytes(MipLevel,
			RAPI::RInt2(OriginalDesc.dwWidth, OriginalDesc.dwHeight),
			Resource->GetInternalTextureFormat()) / 2;
		DataLocal = new unsigned char[size];

		// Give out data and pitch
		lpDDSurfaceDesc->lpSurface = DataLocal;
		lpDDSurfaceDesc->lPitch = RAPI::RTexture::ComputeRowPitchBytes(MipLevel,
			RAPI::RInt2(OriginalDesc.dwWidth, OriginalDesc.dwHeight),
			Resource->GetInternalTextureFormat()) / 2;
	}
	else
	{
		// Allocate some temporary data
		unsigned int size = RBaseTexture::ComputeSizeInBytes(MipLevel,
			RAPI::RInt2(OriginalDesc.dwWidth, OriginalDesc.dwHeight),
			Resource->GetInternalTextureFormat());
		DataLocal = new unsigned char[size];

		// Give out data and pitch
		lpDDSurfaceDesc->lpSurface = DataLocal;
		lpDDSurfaceDesc->lPitch = RAPI::RTexture::ComputeRowPitchBytes(MipLevel,
			RAPI::RInt2(OriginalDesc.dwWidth, OriginalDesc.dwHeight),
			Resource->GetInternalTextureFormat());
	}


	return S_OK;
}

HRESULT FakeDirectDrawSurface7::Unlock( LPRECT lpRect )
{
	int redBits = Toolbox::GetNumberOfBits(OriginalDesc.ddpfPixelFormat.dwRBitMask);
	int greenBits = Toolbox::GetNumberOfBits(OriginalDesc.ddpfPixelFormat.dwGBitMask);
	int blueBits = Toolbox::GetNumberOfBits(OriginalDesc.ddpfPixelFormat.dwBBitMask);
	int alphaBits = Toolbox::GetNumberOfBits(OriginalDesc.ddpfPixelFormat.dwRGBAlphaBitMask);

	int bpp = redBits + greenBits + blueBits + alphaBits;

	// Mip finished loading
	Data = DataLocal;

	if(bpp == 16)
	{

	}else
	{
		// Copy data to miplevel
		/*void* data;
		if(Resource->GetEngineTexture()->Map(&data, MipLevel))
		{
			memcpy(data, Data, Resource->GetEngineTexture()->ComputeSizeInBytes(MipLevel));
			Resource->GetEngineTexture()->Unmap(MipLevel);
		}
		*/
		Resource->IncreaseQueuedMipMapCount();
		Resource->OnMipMapLoaded(this);
	}

	//delete[] DataLocal;
	//DataLocal = NULL;

	

	return S_OK;
}

HRESULT FakeDirectDrawSurface7::ReleaseDC( HDC hDC )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::Restore()
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::SetClipper( LPDIRECTDRAWCLIPPER lpDDClipper )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::SetColorKey( DWORD dwFlags, LPDDCOLORKEY lpDDColorKey )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::SetOverlayPosition( LONG lX, LONG lY )
{
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::SetPalette( LPDIRECTDRAWPALETTE lpDDPalette )
{
	return S_OK; 
}



HRESULT FakeDirectDrawSurface7::UpdateOverlay( LPRECT lpSrcRect, LPDIRECTDRAWSURFACE7 lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::UpdateOverlayDisplay( DWORD dwFlags )
{
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::UpdateOverlayZOrder( DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSReference )
{
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::GetDDInterface( LPVOID* lplpDD )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::PageLock( DWORD dwFlags )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::PageUnlock( DWORD dwFlags )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::SetSurfaceDesc( LPDDSURFACEDESC2 lpDDSurfaceDesc, DWORD dwFlags )
{
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::SetPrivateData( REFGUID guidTag, LPVOID lpData, DWORD cbSize, DWORD dwFlags )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::GetPrivateData( REFGUID guidTag, LPVOID lpBuffer, LPDWORD lpcbBufferSize )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::FreePrivateData( REFGUID guidTag )
{
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::GetUniquenessValue( LPDWORD lpValue )
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::ChangeUniquenessValue()
{
	return S_OK; 
}

HRESULT FakeDirectDrawSurface7::SetPriority( DWORD dwPriority )
{
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::GetPriority( LPDWORD dwPriority )
{
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::SetLOD( DWORD dwLOD )
{
	return S_OK;
}

HRESULT FakeDirectDrawSurface7::GetLOD( LPDWORD dwLOD )
{
	return S_OK;
}
