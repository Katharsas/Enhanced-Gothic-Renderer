#pragma once
#ifndef MYDIRECTDRAW_H
#define MYDIRECTDRAW_H

#include "../pch.h"
#include "MyDirect3D7.h"
#include "MyDirectDrawSurface7.h"
#include <comdef.h>
#include "FakeDirectDrawSurface7.h"
#include "MyClipper.h"
#include "../../Shared/Logger.h"
#include <REngine.h>
#include <RDevice.h>

const GUID IID_IDirect3D7 = { 0xf5049e77,0x4861,0x11d2, { 0xa4,0x7,0x0,0xa0,0xc9,0x6,0x29,0xa8 } }; 

class MyDirectDraw : public IDirectDraw7  {
public:
	MyDirectDraw(IDirectDraw7* directDraw7) : directDraw7(directDraw7) {
		RefCount = 1;

		ZeroMemory(&DisplayMode, sizeof(DDSURFACEDESC2));

		// Gothic calls GetDisplayMode without Setting it first, so do it here
		SetDisplayMode(800, 600, 32, 60, 0);
	}

	/*** IUnknown methods ***/
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObj) {
		HRESULT hr = S_OK;//this->directDraw7->QueryInterface(riid, ppvObj);
		if (riid == IID_IDirect3D7) {
			*ppvObj = new MyDirect3D7((IDirect3D7*)*ppvObj);
		}

		return hr;
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		RefCount++;
		return RefCount;
	}

	ULONG STDMETHODCALLTYPE Release() {
		RefCount--;
		if (RefCount == 0) {
			delete this;
			return 0;
		}

		return RefCount;
	}

	/*** IDirectDraw7 methods ***/
	HRESULT STDMETHODCALLTYPE GetAvailableVidMem(LPDDSCAPS2 lpDDSCaps2, LPDWORD lpdwTotal, LPDWORD lpdwFree) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetCaps(LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetCooperativeLevel(HWND hWnd, DWORD dwFlags) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 lpdddi, DWORD dwFlags) {
		ZeroMemory(lpdddi, sizeof(LPDDDEVICEIDENTIFIER2));
		strcpy(lpdddi->szDescription, "D3D11-Renderer");
		strcpy(lpdddi->szDriver, "D3D11-Renderer");
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetDisplayMode(LPDDSURFACEDESC2 lpDDSurfaceDesc2) {
		*lpDDSurfaceDesc2 = DisplayMode;
		
		return S_OK; 
	}

	HRESULT STDMETHODCALLTYPE SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags) {

		//LogInfo() << "SetDisplayMode:";
		//LogInfo() << "Width: " << dwWidth;
		//LogInfo() << "Height: " << dwHeight;
		//LogInfo() << "BPP: " << dwBPP;

		DisplayMode.dwWidth = dwWidth;
		DisplayMode.dwHeight = dwHeight;
		DisplayMode.dwRefreshRate = dwRefreshRate;
		DisplayMode.dwFlags = dwFlags;

		DisplayMode.ddpfPixelFormat.dwRGBBitCount = dwBPP;
		DisplayMode.ddpfPixelFormat.dwPrivateFormatBitCount = dwBPP;
		DisplayMode.dwFlags |= DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		DisplayMode.dwSize = sizeof(DisplayMode);
		DisplayMode.ddpfPixelFormat.dwSize = sizeof(DisplayMode.ddpfPixelFormat);
	
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetFourCCCodes(LPDWORD lpNumCodes, LPDWORD lpCodes) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetGDISurface(LPDIRECTDRAWSURFACE7 FAR *lplpGDIDDSSurface) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetMonitorFrequency(LPDWORD lpdwFrequency) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetScanLine(LPDWORD lpdwScanLine) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetSurfaceFromDC(HDC hdc,	LPDIRECTDRAWSURFACE7 * lpDDS) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetVerticalBlankStatus(LPBOOL lpbIsInVB) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Compact() {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter) {
		MyClipper* clipper = new MyClipper;
		clipper->AddRef();
		*lplpDDClipper = clipper;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE FAR *lplpDDPalette, IUnknown FAR *pUnkOuter) {
		return S_OK;
	}

	

	static HRESULT WINAPI EnumSurfacesCallback2(LPDIRECTDRAWSURFACE7 lpDDSurface, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext)
	{
		DDSURFACEDESC2 desc = *lpDDSurfaceDesc;

		LogInfo() << "MMDESC:\n";
		LogInfo() << "\n - MMSUBLEVEL: " << ((desc.ddsCaps.dwCaps2 & DDSCAPS2_MIPMAPSUBLEVEL) != 0);
		LogInfo() << "\n - MM: " << ((desc.ddsCaps.dwCaps & DDSCAPS_MIPMAP) != 0);
		LogInfo() << "\n - MMCOUNT: " << desc.dwMipMapCount;		
		LogInfo() << "\n - SIZE: " << desc.dwWidth << ", " << desc.dwHeight;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPDIRECTDRAWSURFACE7 FAR *lplpDDSurface,  IUnknown FAR *pUnkOuter) 
	{
		if(lpDDSurfaceDesc2->ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN)
		{
			LogInfo() << "Forcing DDSCAPS_OFFSCREENPLAIN-Surface to 24-Bit";
			// Set up the pixel format for 24-bit RGB (8-8-8).
			lpDDSurfaceDesc2->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
			lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags= DDPF_RGB;
			lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount = (DWORD)3*8;
			lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask    = 0x00FF0000;
			lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask    = 0x0000FF00;
			lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask    = 0x000000FF;
		}

		// Calculate bpp
		int redBits = Toolbox::GetNumberOfBits(lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask);
		//int greenBits = Toolbox::GetNumberOfBits(lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask);
		//int blueBits = Toolbox::GetNumberOfBits(lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask);
		//int alphaBits = Toolbox::GetNumberOfBits(lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBAlphaBitMask);

		// Figure out format
		DXGI_FORMAT fmt;
		int bpp = 0;
		if((lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags & DDPF_FOURCC) == DDPF_FOURCC)
		{
			switch(lpDDSurfaceDesc2->ddpfPixelFormat.dwFourCC)
			{
			case FOURCC_DXT1:
				fmt = DXGI_FORMAT_BC1_UNORM;
				break;
			case FOURCC_DXT2:
			case FOURCC_DXT3:
				fmt = DXGI_FORMAT_BC2_UNORM;
				break;
			case FOURCC_DXT4:
			case FOURCC_DXT5:
				fmt = DXGI_FORMAT_BC3_UNORM;
				break;
			}
		}else
		{
			fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
			bpp = 32;
		}

		if (redBits == 5)
		{
			bpp = 16;

			// Hack: Don't allow MIPs on 16-bit surfaces
			// Only a very small number of mods use these (L'Hiver is the only one I know so far)
			// and thus it shouldn't be too much of a problem
			//lpDDSurfaceDesc2->dwMipMapCount = 1;
		}
		// Create surface
		MyDirectDrawSurface7* mySurface = new MyDirectDrawSurface7();

		// Create a fake mipmap chain if needed
		if(lpDDSurfaceDesc2->ddsCaps.dwCaps & DDSCAPS_MIPMAP)
		{
			DDSURFACEDESC2 desc = *lpDDSurfaceDesc2;

			// First level was already created above
			FakeDirectDrawSurface7* lastMip = NULL;
			int level = 1;
			while(desc.dwMipMapCount > 1)
			{

				FakeDirectDrawSurface7* mip = new FakeDirectDrawSurface7;
				desc.dwMipMapCount--;
				//desc.dwHeight /= 2;
				//desc.dwWidth /= 2;
				desc.ddsCaps.dwCaps2 |= DDSCAPS2_MIPMAPSUBLEVEL;
				mip->InitFakeSurface(&desc, mySurface, level);

				mySurface->AddAttachedSurface((LPDIRECTDRAWSURFACE7)mip);

				if(lastMip)
				{
					lastMip->AddAttachedSurface(mip);
				}
				lastMip = mip;
				level++;
			}


		}

		*lplpDDSurface = mySurface;

		mySurface->SetSurfaceDesc(lpDDSurfaceDesc2, 0);

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE DuplicateSurface(LPDIRECTDRAWSURFACE7 lpDDSurface, LPDIRECTDRAWSURFACE7 FAR *lplpDupDDSurface) {
		return S_OK;
	}

	struct ModesEnumInfo
	{
		LPDDENUMMODESCALLBACK2 originalFn;
		LPVOID originalUserArg;
	};

	static HRESULT WINAPI EnumModesCallback2(LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext)
	{
		LogInfo() << "RefreshRate: " << lpDDSurfaceDesc->dwRefreshRate;
		LogInfo() << "\nWidth: " << lpDDSurfaceDesc->dwWidth;
		LogInfo() << "\nHeight: " << lpDDSurfaceDesc->dwHeight;
		
		/*FILE* f = fopen("system\\GD3D11\\data\\ModesEnum.bin", "ab");

		fwrite(lpDDSurfaceDesc, sizeof(DDSURFACEDESC2), 1, f);

		fclose(f);*/

		return (*((ModesEnumInfo *)lpContext)->originalFn)(lpDDSurfaceDesc, ((ModesEnumInfo *)lpContext)->originalUserArg);
	}

	HRESULT STDMETHODCALLTYPE EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPVOID lpContext, LPDDENUMMODESCALLBACK2 lpEnumModesCallback) {
		ModesEnumInfo info;
		info.originalFn = lpEnumModesCallback;
		info.originalUserArg = lpContext;

		std::vector<DisplayModeInfo> modes;
		REngine::RenderingDevice->GetDisplayModeList(modes, false);

		for(unsigned int i=0;i<modes.size();i++)
		{
			DDSURFACEDESC2 desc;
			ZeroMemory(&desc, sizeof(desc));

			desc.dwHeight = modes[i].Height;
			desc.dwWidth = modes[i].Width;
			desc.ddpfPixelFormat.dwRGBBitCount = modes[i].Bpp;

			(*lpEnumModesCallback)(&desc, lpContext);
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSD2, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE EvaluateMode(DWORD dwFlags, DWORD  *pSecondsUntilTimeout) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE FlipToGDISurface() {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Initialize(GUID FAR *lpGUID) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE RestoreDisplayMode() {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE RestoreAllSurfaces() {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE StartModeTest(LPSIZE lpModesToTest, DWORD dwNumEntries, DWORD dwFlags) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE TestCooperativeLevel() {
		return S_OK;
	}

private:
	IDirectDraw7* directDraw7;
	int RefCount;
	DDSURFACEDESC2 DisplayMode;
};

#endif
