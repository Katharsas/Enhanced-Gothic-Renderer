#pragma once
#include "zDefinitions.h"
#include "pch.h"
#include "zCMatrixStack.h"
#include "zSTRING.h"
#include "oCGame.h"
class zCVob;

struct zTViewport {					
	int	xmin;				
	int	ymin;
	int	xdim;				
	int	ydim;
	float xminFloat;			
	float yminFloat;
	float xmaxFloat;			
	float ymaxFloat;
	float xdimFloat;			
	float ydimFloat;
	float xdimFloatMinus1;
	float ydimFloatMinus1;
	float xcenter;			
	float ycenter;			
};

enum zTCamTrafoType
{
	zCAM_TRAFO_WORLD,
	zCAM_TRAFO_VIEW,
	zCAM_TRAFO_WORLDVIEW,
	zCAM_TRAFO_WORLDVIEW_INV,
	zCAM_TRAFO_VIEW_INV
};

class zCPolygon;
class zCMesh;
class zCMaterial;

class zCCamera
{
public:
	
	/** Returns the currently set camera */
	static zCCamera* GetActiveCamera()
	{
		oCGame* game = oCGame::GetGame();
		return game ? game->GetSessionCamera() : nullptr;
		//return *(zCCamera**)MemoryLocations::Gothic::zCCamera_p_zCCamera__activeCam;
	}

	/** Returns the world-position of the currently active camera */
	static const float3& GetActiveCameraPosition()
	{
		return *(float3 *)MemoryLocations::Gothic::zVEC3_zCCamera__activeCamPos;
	}

	/** Sets the camera as active and recomputes the matrices */
	void Activate()
	{
		XCALL(MemoryLocations::Gothic::zCCamera__Activate_void);
	}

	/** Recalculates the projectionmatrix */
	void UpdateProjectionMatrix()
	{
		XCALL(MemoryLocations::Gothic::zCCamera__UpdateProjectionMatrix_void);
	}
	
	/** Returns the current view-matrix */
	const Matrix& GetViewMatrix()
	{
		return m_WorldViewMatrix;
	}

	/** Returns the world matrix of the camera */
	const Matrix& GetCameraWorldMatrix()
	{
		return m_CameraWorldMatrix;
	}

	/** Returns the inverse of the view-matrix */
	const Matrix& GetInverseViewMatrix()
	{
		return m_CameraWorldMatrix;
	}

	/** Returns the direction the camera is facing in */
	float3 GetCameraDirection()
	{
		return m_CameraWorldMatrix.Forward();
	}

	/** Returns the currently active projection matrix.
		You often need to transpose this! This and ViewMatrix 
		have different formats... Only PB knows why. */
	const Matrix& GetProjectionMatrix()
	{
		return m_ProjectionMatrix;
	}

	/** Returns the combined view-proj matrix */
	Matrix GetViewProjMatrix()
	{
		return m_ProjectionMatrix.Transpose() * m_ViewMatrix;
	}

	/** Return far and nearplane */
	float GetFarPlane(){return m_FarPlane;}
	float GetNearPlane(){return m_NearPlane;}


	/** Checks whether a given boundingbox is inside the given frustum. The index in "cache" is tested first, if it isn't set to -1 */
	static zTCam_ClipType BBox3DInFrustumCached(const zTBBox3D& bbox3D, const zTPlane* frustumPlanes, const byte* signbits, int& cache, int& clipFlags)
	{
		// This resembles gothics method for checking the frustum, but with enancements

		// Use cache first, if possible
		int tmpCache = cache;
		int	i = (cache != -1) ? cache : 5;
		int skip = -1;

		// Check all planes
		do {
			// Don't test the cached plane twice
			if (i == skip)
				continue;

			// Still not sure how these clipflags work
			// Seems to store some binary value for each frustum-plane... Maybe already checked planes?
			if (!(clipFlags & (1 << i)))
				continue;

			float dist;
			const zTPlane& plane = frustumPlanes[i];
			switch (signbits[i])
			{
			case 0:	// 000, ZYX
				dist = bbox3D.m_Min.x * plane.m_Normal.x + bbox3D.m_Min.y * plane.m_Normal.y + bbox3D.m_Min.z * plane.m_Normal.z;	
				if (dist<plane.m_Distance) { cache = i; return ZTCAM_CLIPTYPE_OUT; }
				dist = bbox3D.m_Max.x * plane.m_Normal.x + bbox3D.m_Max.y * plane.m_Normal.y + bbox3D.m_Max.z * plane.m_Normal.z;	
				if (dist >= plane.m_Distance) clipFlags &= ~(1 << i);
				break;
			case 1:	// 001
				dist = bbox3D.m_Max.x * plane.m_Normal.x + bbox3D.m_Min.y * plane.m_Normal.y + bbox3D.m_Min.z * plane.m_Normal.z;	
				if (dist<plane.m_Distance) { cache = i; return ZTCAM_CLIPTYPE_OUT; }
				dist = bbox3D.m_Min.x * plane.m_Normal.x + bbox3D.m_Max.y * plane.m_Normal.y + bbox3D.m_Max.z * plane.m_Normal.z;	
				if (dist >= plane.m_Distance) clipFlags &= ~(1 << i);
				break;
			case 2:	// 010
				dist = bbox3D.m_Min.x * plane.m_Normal.x + bbox3D.m_Max.y * plane.m_Normal.y + bbox3D.m_Min.z * plane.m_Normal.z;	
				if (dist<plane.m_Distance) { cache = i; return ZTCAM_CLIPTYPE_OUT; }
				dist = bbox3D.m_Max.x * plane.m_Normal.x + bbox3D.m_Min.y * plane.m_Normal.y + bbox3D.m_Max.z * plane.m_Normal.z;	
				if (dist >= plane.m_Distance) clipFlags &= ~(1 << i);
				break;
			case 3:	// 011
				dist = bbox3D.m_Max.x * plane.m_Normal.x + bbox3D.m_Max.y * plane.m_Normal.y + bbox3D.m_Min.z * plane.m_Normal.z;	
				if (dist<plane.m_Distance) { cache = i; return ZTCAM_CLIPTYPE_OUT; }
				dist = bbox3D.m_Min.x * plane.m_Normal.x + bbox3D.m_Min.y * plane.m_Normal.y + bbox3D.m_Max.z * plane.m_Normal.z;	
				if (dist >= plane.m_Distance) clipFlags &= ~(1 << i);
				break;
			case 4:	// 100
				dist = bbox3D.m_Min.x * plane.m_Normal.x + bbox3D.m_Min.y * plane.m_Normal.y + bbox3D.m_Max.z * plane.m_Normal.z;	
				if (dist<plane.m_Distance) { cache = i; return ZTCAM_CLIPTYPE_OUT; }
				dist = bbox3D.m_Max.x * plane.m_Normal.x + bbox3D.m_Max.y * plane.m_Normal.y + bbox3D.m_Min.z * plane.m_Normal.z;	
				if (dist >= plane.m_Distance) clipFlags &= ~(1 << i);
				break;
			case 5:	// 101
				dist = bbox3D.m_Max.x * plane.m_Normal.x + bbox3D.m_Min.y * plane.m_Normal.y + bbox3D.m_Max.z * plane.m_Normal.z;	
				if (dist<plane.m_Distance) { cache = i; return ZTCAM_CLIPTYPE_OUT; }
				dist = bbox3D.m_Min.x * plane.m_Normal.x + bbox3D.m_Max.y * plane.m_Normal.y + bbox3D.m_Min.z * plane.m_Normal.z;	
				if (dist >= plane.m_Distance) clipFlags &= ~(1 << i);
				break;
			case 6:	// 110
				dist = bbox3D.m_Min.x * plane.m_Normal.x + bbox3D.m_Max.y * plane.m_Normal.y + bbox3D.m_Max.z * plane.m_Normal.z;	
				if (dist<plane.m_Distance) { cache = i; return ZTCAM_CLIPTYPE_OUT; }
				dist = bbox3D.m_Max.x * plane.m_Normal.x + bbox3D.m_Min.y * plane.m_Normal.y + bbox3D.m_Min.z * plane.m_Normal.z;	
				if (dist >= plane.m_Distance) clipFlags &= ~(1 << i);
				break;
			case 7:	// 111
				dist = bbox3D.m_Max.x * plane.m_Normal.x + bbox3D.m_Max.y * plane.m_Normal.y + bbox3D.m_Max.z * plane.m_Normal.z;	
				if (dist<plane.m_Distance) { cache = i; return ZTCAM_CLIPTYPE_OUT; }
				dist = bbox3D.m_Min.x * plane.m_Normal.x + bbox3D.m_Min.y * plane.m_Normal.y + bbox3D.m_Min.z * plane.m_Normal.z;	
				if (dist >= plane.m_Distance) clipFlags &= ~(1 << i);
				break;
			}

			// If this was a cached check, return to normal
			if (tmpCache != -1)
			{
				skip = tmpCache;
				tmpCache = -1;
				i = 6; // Would be 5, but we are decrementing it right after this
			}
		} while (i--);

		// If we got this far, the box is visible and we can reset the cache
		cache = -1;

		return (clipFlags> 0) ? ZTCAM_CLIPTYPE_CROSSING : ZTCAM_CLIPTYPE_IN;
	}

	zTCam_ClipType BBox3DInFrustum(const zTBBox3D& box, int& clipFlags)
	{
		XCALL(MemoryLocations::Gothic::zCCamera__BBox3DInFrustum_zTBBox3D_const_r_int_r);
	}

	/** Returns the frustum-planes */
	const Plane* GetFrustumPlanes()
	{
		return m_Frustumplanes;
	}

	/** Returns the sign-bits of the frustum-planes */
	const byte* GetFrustumSignBits()
	{
		return m_Signbits;
	}

	/** Returns the view-distances */
	float2 GetViewDistances()
	{
		return float2(m_ViewDistanceX, m_ViewDistanceY);
	}
private:

	Plane m_Frustumplanes[6];
	unsigned char m_Signbits[6];

	// Viewport and view to draw to
	zTViewport m_ActiveViewport;
	class zCViewBase* m_TargetView;

	// Matrices set for each vob. They put the vobs world-matrix
	// in there for each drawcall for some reason. These shouldn't
	// be used.
	Matrix m_WorldViewMatrix;
	Matrix m_WorldViewMatrixInv;

	// Not sure, probably some values regarding shake-effects
	// Most of them seem to be floats
	byte data1[36];

	// Transformation matrices
	Matrix m_ViewMatrix;
	Matrix m_CameraWorldMatrix; // Same as inverse view-matrix
	Matrix m_DrawCallWorldMatrix; // Worldmatrix of the currently drawn vob, in Gothics renderer at least

	zCMatrixStack<Matrix,8>	m_MatrixViewStack;
	zCMatrixStack<Matrix,8>	m_MatrixWorldStack;
	zCMatrixStack<Matrix,8>	m_MatrixWorldViewStack;

	// Weird datalayout. Probably haven't got that right?
	Matrix m_ProjectionMatrix;

	// DrawSimplePoly
	enum { zTCAM_POLY_NUM_VERT = 4 };
	struct zTCamVertSimple 
	{
		float x,y,z;
		float2 texuv;
		DWORD color;
	};

	zTCamVertSimple m_PolyCamVerts[zTCAM_POLY_NUM_VERT];
	zCPolygon *m_Poly;
	zCMesh *m_PolyMesh;
	zCMaterial *m_PolyMaterial;

	// Screen-Effects
	zBOOL m_ScreenFadeEnabled;
	DWORD m_ScreenFadeColor;

#if DATASET_VERSION == VERSION_2_6_FIX
	zSTRING m_ScreenFadeTexture;
	float m_ScreenFadeTextureAniFPS;
	zTRnd_AlphaBlendFunc m_ScreenFadeTextureBlendFunc;
#endif

	zBOOL m_CinemaScopeEnabled;
	DWORD m_CinemaScopeColor;

	int m_ProjectionType;
	int m_DrawMode;
	int m_ShadeMode;
	zBOOL m_DrawWire;

	// View distances and clipping planes
	float m_FarPlane;
	float m_NearPlane;

	// viewDistanceX = (vpData.xdim * 0.5f) / tan(fovH/2));
	// viewDistanceY = (vpData.ydim * 0.5f) / tan(fovV/2));
	float m_ViewDistanceX;
	float m_ViewDistanceY;


	float m_ViewDistanceXInv;
	float m_ViewDistanceYInv;
	zBOOL m_VobFarClipZ;
	float m_FovH;
	float m_FovV;
	zCVob* m_ConnectedVob;

};