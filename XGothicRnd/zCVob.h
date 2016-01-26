#pragma once
#include "zCObject.h"
#include "zDefinitions.h"
#include "zCArray.h"
#include <RTools.h>
#include "zCPolygon.h"
#include "zEngineHooks.h"
#include "zClassDef.h"

#ifndef HOOK_RENDERING
#include "GVobObject.h"
#endif

class zCPolygon;
class zCVisual;
class GVobObject;
class zCCollisionObjectDef;
class zCCollisionObject;
class zCVob : public zCObject
{
public:

	// Thanks, Community!
	struct VobFlags 
	{
		byte ShowVisual					: 1;	
		byte DrawBBox3D					: 1;	
		byte VisualAlphaEnabled			: 1;	
		byte PhysicsEnabled				: 1;	
		byte StaticVob					: 1;	
		byte IgnoredByTraceRay			: 1;	
		byte CollDetectionStatic		: 1;	
		byte CollDetectionDynamic		: 1;	
		byte CastDynShadow				: 2;	
		byte LightColorStatDirty		: 1;	
		byte LightColorDynDirty			: 1;	
		byte IsInMovementMode			: 2;	
		byte SleepingMode				: 2;	
		byte HintTrafoLocalConst		: 1;	
		byte InsideEndMovementMethod	: 1;
		byte VisualCamAlign				: 2;
#if DATASET_VERSION == VERSION_2_6_FIX
		byte CollisionButNoMove			: 4;
		byte DontWriteIntoArchive		: 1;
		byte IsInWater					: 1;
		byte IsAmbientVob				: 1;
#elif DATASET_VERSION == VERSION_1_8K_MOD
		byte CollisionButNoMove			: 8;
		byte DontWriteIntoArchive		: 1;
#endif
	};	

	/** Returns the world-matrix of this vob */
	const Matrix& GetWorldMatrix()
	{
		return m_WorldMatrix;
	}

	/** Stores the vob-object associated with this in the vob */
	void SetVobObject(class GVobObject* obj)
	{
#ifdef HOOK_RENDERING
		m_VobObject = obj == nullptr ? (GVobObject*)-1 : obj;
#endif
	}

	/** Returns the stored vob object */
	GVobObject* GetVobObject()
	{
#ifdef HOOK_RENDERING
		// This value is initialized to -1 at creationtime
		// TODO: Modify the code to set this to 0 to remove the check!
		return m_VobObject != (void*)-1 ? m_VobObject : nullptr;
#else
		return GVobObject::QueryFromSource(this);
#endif
	}

	/** Returns the vobflags bitfield, containing useful information about this vob */
	VobFlags& GetVobFlags()
	{
		return m_VobFlags;
	}

	/** Returns the visual used by this vob */
	zCVisual* GetVisual() 
	{
		return m_Visual;
	}

	/** Returns the static lighting on this vob 
		Recalculates it, if dirty */
	DWORD GetStaticLighting()
	{
		CalcStaticLighting();
		return m_StaticLightColor;
	}

	/** Returns the world position of this vob */
	float3 GetWorldPosition()
	{
		return m_WorldMatrix.TranslationT();
	}

	/** Returns the world this vob resists in */
	zCWorld* GetHomeWorld()
	{
		return m_HomeWorld;
	}

	/** Returns the polygon this vob is standing on */
	zCPolygon* GetGroundPolygon()
	{
		return m_GroundPolygon;
	}

	/** Calculates the static lighting of this vob */
	void CalcStaticLighting()
	{
		if (!m_VobFlags.LightColorStatDirty)
			return;

		// Set to something general if no groundpoly was found
		if (!m_GroundPolygon)
		{
			m_StaticLightColor = RTools::float4ToDWORD(float4(160 / 255.0f, 160 / 255.0f, 160 / 255.0f, 1.0f));
			m_VobFlags.LightColorStatDirty = 0;
			return;
		}

		// Cast a ray downwards to find the static lighting under the vob
		float3 rayStart = 0.5f * (m_BBox3D.m_Min + m_BBox3D.m_Max);
		float3 hitPoint;
		float alpha;

		zBOOL hit = m_GroundPolygon->CheckRayPolyIntersection(rayStart, float3(0, -3000, 0), hitPoint, alpha);

		// Try again, from top of the BBox's y-coord
		if (!hit)
			m_GroundPolygon->CheckRayPolyIntersection(float3(rayStart.x, m_BBox3D.m_Max.y, rayStart.z), float3(0, -3000, 0), hitPoint, alpha);

		// Now extract the color of this position
		float3 c = m_GroundPolygon->GetLightStatAtPos(hitPoint) / 255.0f;
		
		// Modify brightness
		if (!m_GroundPolygon->GetPolyFlags().SectorPoly)
		{
			// Make everything 50% brighter a bit if under the sky
			c = 0.5f * c + float3(0.5f);
		}
		else
		{
			// And a 20%, if not
			c = 0.8f * c + float3(0.2f);
		}

		m_StaticLightColor = RTools::float4ToDWORD(float4(c.x, c.y, c.z, 1.0f));
		m_VobFlags.LightColorStatDirty = 0;

	}

protected:
	zCTree<zCVob>* m_GlobalVobTreeNode;	

	//unsigned int m_LastTimeDrawn;
	/** Gothic uses a 32-bit integer for m_LastTimeDrawn. However, this value is
		never set anymore, since we completely removed the original
		rendering-function from the game. That means, this value is only set to -1
		at creation of the vob and then not touched at all, making it ideal for storing
		our own data in here */
	GVobObject* m_VobObject;

	unsigned int m_LastTimeCollected;
	zCArray<struct zCBspLeaf*> m_VobLeafList;

	// The worldmatrix for this vob. Can directly be used for rendering!
	Matrix m_WorldMatrix;

	// Bounding primitives
	zTBBox3D m_BBox3D;

#if DATASET_VERSION == VERSION_2_6_FIX
	zTBSphere3D m_BSphere3D;
#endif

	byte N016DC337[12];

	// Sound, Light, etc
	enum zTVobType m_VobType;

	// Not sure what this does
	byte data1[4];

	zCWorld* m_HomeWorld;
	zCPolygon* m_GroundPolygon;

	byte data2[8];

	zCVisual* m_Visual;
	float m_VisualAlpha;

#if DATASET_VERSION == VERSION_2_6_FIX
	float m_VobFarClipZScale;
	unsigned int m_WindMode; // 1 and 2 define this as wind affected
	float m_WindStrength;
	int	m_zBias;
#endif

	byte data3[4];

	DWORD m_StaticLightColor;
	DWORD m_DynamicLightColor;
	float3 m_StaticLightDirection; // Never changes, though, the sun is moving
	zSTRING* m_VobPresetName;

	byte data4[8];

	VobFlags m_VobFlags;

	zCCollisionObjectDef* m_CollisionObjectClass;
	zCCollisionObject* m_CollisionObject;
};