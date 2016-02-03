#include "pch.h"
#include "MyDirectDrawSurface7.h"
#include <ddraw.h>
#include <RResourceCache.h>
#include <REngine.h>
#include <RTexture.h>
#include "../../Shared/Toolbox.h"
#include "../GGame.h"
#include "../Engine.h"
#include "../GTexture.h"
#include "FakeDirectDrawSurface7.h"

#define DebugWriteTex(x)  DebugWrite(x)

MyDirectDrawSurface7::MyDirectDrawSurface7()
{
	refCount = 1;
	LockedData = NULL;
	IsReady = false;

	EngineTexture = NULL;

	QueuedMipMaps = 0;
}

MyDirectDrawSurface7::~MyDirectDrawSurface7()
{
	if(EngineTexture)
		EngineTexture->OnSurfaceDeleted();

	// Sometimes gothic doesn't unlock a surface or this is a movie-buffer
	delete[] LockedData;
}

/** Pointer to the internal texture-object. Needs to be set for this to work */
void MyDirectDrawSurface7::SetExternalEngineTexture(class GTexture* texture)
{
	EngineTexture = texture;
}

/** Binds this texture */
void MyDirectDrawSurface7::BindToSlot(int slot)
{
	
}

/** Loads additional resources if possible */
void MyDirectDrawSurface7::LoadAdditionalResources(zCTexture* ownedTexture)
{
	
}

HRESULT MyDirectDrawSurface7::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	return S_OK; 
}

ULONG MyDirectDrawSurface7::AddRef()
{
	refCount++;
	return refCount;
}

ULONG MyDirectDrawSurface7::Release()
{
	refCount--;
	ULONG uRet = refCount;

	if(uRet == 0)
	{
		delete this;
	}

	return uRet;
}

HRESULT MyDirectDrawSurface7::AddAttachedSurface( LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface )
{
	attachedSurfaces.push_back((MyDirectDrawSurface7 *)lpDDSAttachedSurface);
	return S_OK;
}

HRESULT MyDirectDrawSurface7::AddOverlayDirtyRect( LPRECT lpRect )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::Blt( LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::BltBatch( LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::BltFast( DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::DeleteAttachedSurface( DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::EnumAttachedSurfaces( LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::EnumOverlayZOrders( DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpfnCallback )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::Flip( LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride, DWORD dwFlags )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::GetAttachedSurface( LPDDSCAPS2 lpDDSCaps2, LPDIRECTDRAWSURFACE7* lplpDDAttachedSurface )
{
	if(attachedSurfaces.empty())
		return E_FAIL;

	*lplpDDAttachedSurface = attachedSurfaces[0];
	attachedSurfaces[0]->AddRef();

	return S_OK;
}

HRESULT MyDirectDrawSurface7::GetBltStatus( DWORD dwFlags )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::GetCaps( LPDDSCAPS2 lpDDSCaps2 )
{
	*lpDDSCaps2 = OriginalSurfaceDesc.ddsCaps;

	return S_OK;
}

HRESULT MyDirectDrawSurface7::GetClipper( LPDIRECTDRAWCLIPPER* lplpDDClipper )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::GetColorKey( DWORD dwFlags, LPDDCOLORKEY lpDDColorKey )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::GetDC( HDC* lphDC )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::GetFlipStatus( DWORD dwFlags )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::GetOverlayPosition( LPLONG lplX, LPLONG lplY )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::GetPalette( LPDIRECTDRAWPALETTE* lplpDDPalette )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::GetPixelFormat( LPDDPIXELFORMAT lpDDPixelFormat )
{
	*lpDDPixelFormat = OriginalSurfaceDesc.ddpfPixelFormat;

	return S_OK;
}

HRESULT MyDirectDrawSurface7::GetSurfaceDesc( LPDDSURFACEDESC2 lpDDSurfaceDesc )
{
	*lpDDSurfaceDesc = OriginalSurfaceDesc;
	return S_OK;
}

HRESULT MyDirectDrawSurface7::Initialize( LPDIRECTDRAW lpDD, LPDDSURFACEDESC2 lpDDSurfaceDesc )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::IsLost()
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::Lock( LPRECT lpDestRect, LPDDSURFACEDESC2 lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent )
{
	

	// rowPitchBytes of the ACTUAL texture
	unsigned int rowPitchBytes = RAPI::RTexture::ComputeRowPitchBytes(0, RAPI::RInt2(OriginalSurfaceDesc.dwWidth, OriginalSurfaceDesc.dwHeight), TextureFormat);
	unsigned int sizeInBytes = RAPI::RTexture::ComputeSizeInBytes(0,  RAPI::RInt2(OriginalSurfaceDesc.dwWidth, OriginalSurfaceDesc.dwHeight), TextureFormat);
	unsigned int bpp = ComputeBitsPerPixel();

	// Modify the size to match the data we give out, since we convert it afterwards
	if(bpp == 16)
	{
		rowPitchBytes /= 2;
		sizeInBytes /= 2;
	}

	if (!LoadedTextureData.size()) // Pass some data to the game for the lock. Usually there is no texture-object allocated yet.
		LoadedTextureData.resize(sizeInBytes);

	lpDDSurfaceDesc->lpSurface = LoadedTextureData.data();
	lpDDSurfaceDesc->lPitch = rowPitchBytes;

	// Store type of lock so we know what to do at unlock
	LockFlags = dwFlags;

	return S_OK;
}

HRESULT MyDirectDrawSurface7::Unlock( LPRECT lpRect )
{
	RAPI::RInt2 resolution = RAPI::RInt2(OriginalSurfaceDesc.dwWidth, OriginalSurfaceDesc.dwHeight);

	// Only save data on Write-Unlock
	if((LockFlags & DDLOCK_WRITEONLY) != 0)				 // Hack, we need to know if the texture was initialized here.
														 // Reason being that Gothic thinks it can lock textures to read from them, without
														 // specifying the read-flag, but using the write flag. Or something like that.
														 // We don't want to init the texture twice.
														 // ^- Not needed anymore since I killed the original render-function? TODO!
	{
		// If this is a 16-bit surface, we need to convert it to 32-bit first
		int bpp = ComputeBitsPerPixel();

		if(bpp == 16)
		{		
			UINT size = RAPI::RTexture::ComputeSizeInBytes(0, resolution, TextureFormat);

			//byte s = Toolbox::SumBytes(LockedData, size / 2);
			// Convert
			unsigned char* dst = new unsigned char[size];
			for(unsigned int i=0;i<size / 4;i++)
			{
				unsigned char temp0 = LoadedTextureData[i * 2 + 0];
				unsigned char temp1 = LoadedTextureData[i * 2 + 1];
				unsigned pixel_data = temp1 << 8 | temp0;

				unsigned char blueComponent  = (pixel_data & 0x1F);
				unsigned char greenComponent = (pixel_data >> 5 ) & 0x3F;
				unsigned char redComponent   = (pixel_data >> 11) & 0x1F;

				// Extract red, green and blue components from the 16 bits
				dst[4*i+0] = (unsigned char)((redComponent  / 32.0) * 255.0f);
				dst[4*i+1] = (unsigned char)((greenComponent  / 64.0) * 255.0f);
				dst[4*i+2] = (unsigned char)((blueComponent  / 32.0) * 255.0f);
				dst[4*i+3] = 255;
			}

			// Move the data to the engine-object
			if(EngineTexture)
				EngineTexture->OnSurfaceUnlocked(dst, size);

			delete[] dst;
		}else if(bpp == 24)
		{
			// Movie-frame
		}else
		{
			IncreaseQueuedMipMapCount();
		}
	}


	return S_OK;
}

/** Pushes all loaded data of this mip-chain to the gpu */
void MyDirectDrawSurface7::FinalizeTexture()
{
	RAPI::RInt2 resolution = RAPI::RInt2(OriginalSurfaceDesc.dwWidth, OriginalSurfaceDesc.dwHeight);
	UINT size = RAPI::RTexture::ComputeSizeInBytes(0, resolution, TextureFormat);

	// Fill a vector of pointers to mipdata
	std::vector<void*> mipData;

	// Don't use any mips for 16-bit!
	if(ComputeBitsPerPixel() != 16)
	{
		for each (MyDirectDrawSurface7* srf in attachedSurfaces)
		{
			FakeDirectDrawSurface7* mip = (FakeDirectDrawSurface7*)srf;
			mipData.push_back(mip->GetCachedData());
		}
	}

	// Move the data to the engine-object
	if (EngineTexture)
		EngineTexture->OnSurfaceUnlocked(LoadedTextureData.data(), size, mipData);

	// Delete mipmaps
	if (EngineTexture->GetTexture())
	{
		for each (MyDirectDrawSurface7* srf in attachedSurfaces)
		{
			FakeDirectDrawSurface7* mip = (FakeDirectDrawSurface7*)srf;
			if (mip->GetCachedData())
			{
				mip->DeleteCachedData();
			}
		}
	}

	LoadedTextureData.clear();
	LoadedTextureData.shrink_to_fit();
}

HRESULT MyDirectDrawSurface7::ReleaseDC( HDC hDC )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::Restore()
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::SetClipper( LPDIRECTDRAWCLIPPER lpDDClipper )
{
	HWND hWnd;
	lpDDClipper->GetHWnd(&hWnd);

	RAPI::REngine::RenderingDevice->SetWindow(hWnd);
	Engine::Game->Initialize();

	return S_OK;
}

HRESULT MyDirectDrawSurface7::SetColorKey( DWORD dwFlags, LPDDCOLORKEY lpDDColorKey )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::SetOverlayPosition( LONG lX, LONG lY )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::SetPalette( LPDIRECTDRAWPALETTE lpDDPalette )
{
	return S_OK;
}


HRESULT MyDirectDrawSurface7::UpdateOverlay( LPRECT lpSrcRect, LPDIRECTDRAWSURFACE7 lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::UpdateOverlayDisplay( DWORD dwFlags )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::UpdateOverlayZOrder( DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSReference )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::GetDDInterface( LPVOID* lplpDD )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::PageLock( DWORD dwFlags )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::PageUnlock( DWORD dwFlags )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::SetSurfaceDesc( LPDDSURFACEDESC2 lpDDSurfaceDesc, DWORD dwFlags )
{

	OriginalSurfaceDesc = *lpDDSurfaceDesc;

	// Check if this is the rendertarget or something else we dont need
	if(lpDDSurfaceDesc->dwWidth == 0)
	{
		return S_OK;
	}

	// Find out mip-level count
	unsigned int mipMapCount = 1;
	if(lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
	{
		mipMapCount = lpDDSurfaceDesc->dwMipMapCount;
	}else
	{
		lpDDSurfaceDesc->dwMipMapCount = 1;
	}

	int redBits = Toolbox::GetNumberOfBits(lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask);
	int greenBits = Toolbox::GetNumberOfBits(lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask);
	int blueBits = Toolbox::GetNumberOfBits(lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask);
	int alphaBits = Toolbox::GetNumberOfBits(lpDDSurfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask);

	int bpp = redBits + greenBits + blueBits + alphaBits;

	// Find out format
	RAPI::ETextureFormat format = RAPI::ETextureFormat::TF_R8G8B8A8;
	switch(bpp)
	{
	case 16:
		format = RAPI::ETextureFormat::TF_R8G8B8A8;
		break;

	case 24:
	case 32:
		format = RAPI::ETextureFormat::TF_R8G8B8A8;

	case 0:
		{
			// DDS-Texture
			if((lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_FOURCC) == DDPF_FOURCC)
			{
				switch(lpDDSurfaceDesc->ddpfPixelFormat.dwFourCC)
				{
				case FOURCC_DXT1:
					format = RAPI::ETextureFormat::TF_DXT1;
					break;

				case FOURCC_DXT2:
				case FOURCC_DXT3:
					format = RAPI::ETextureFormat::TF_DXT3;
					break;

				case FOURCC_DXT4:
				case FOURCC_DXT5:
					format = RAPI::ETextureFormat::TF_DXT5;
					break;
				}
			}
		}
		break;
	}

	TextureFormat = format;

	return S_OK;
}

HRESULT MyDirectDrawSurface7::SetPrivateData( REFGUID guidTag, LPVOID lpData, DWORD cbSize, DWORD dwFlags )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::GetPrivateData( REFGUID guidTag, LPVOID lpBuffer, LPDWORD lpcbBufferSize )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::FreePrivateData( REFGUID guidTag )
{
	return S_OK;
}

HRESULT MyDirectDrawSurface7::GetUniquenessValue( LPDWORD lpValue )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::ChangeUniquenessValue()
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::SetPriority( DWORD dwPriority )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::GetPriority( LPDWORD dwPriority )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::SetLOD( DWORD dwLOD )
{
	return S_OK; 
}

HRESULT MyDirectDrawSurface7::GetLOD( LPDWORD dwLOD )
{
	return S_OK; 
}

/** Adds one to the queued mipmap count */
void MyDirectDrawSurface7::IncreaseQueuedMipMapCount()
{
	QueuedMipMaps++;

	if (QueuedMipMaps == OriginalSurfaceDesc.dwMipMapCount)
	{
		FinalizeTexture();
	}
}

/** Called when a mipmap got loaded */
void MyDirectDrawSurface7::OnMipMapLoaded(FakeDirectDrawSurface7* mip)
{
	
}

/** Computes the bits-per-pixel used by this surface */
UINT MyDirectDrawSurface7::ComputeBitsPerPixel()
{
	// If this is a 16-bit surface, we need to convert it to 32-bit first
	int redBits = Toolbox::GetNumberOfBits(OriginalSurfaceDesc.ddpfPixelFormat.dwRBitMask);
	int greenBits = Toolbox::GetNumberOfBits(OriginalSurfaceDesc.ddpfPixelFormat.dwGBitMask);
	int blueBits = Toolbox::GetNumberOfBits(OriginalSurfaceDesc.ddpfPixelFormat.dwBBitMask);
	int alphaBits = Toolbox::GetNumberOfBits(OriginalSurfaceDesc.ddpfPixelFormat.dwRGBAlphaBitMask);

	return redBits + greenBits + blueBits + alphaBits;
}