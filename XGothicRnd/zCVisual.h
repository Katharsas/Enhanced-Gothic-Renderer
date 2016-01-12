#pragma once
#include "zDefinitions.h"
#include "zCObject.h"
#include "zEngineFunctions.h"
#include "zClassDef.h"

class zCVobLight;
struct zTRenderContext;
struct zTTraceRayReport;
class zCVisual : public zCObject
{
public:


	enum EVisualType 
	{
		VT_OTHER,
		VT_PROGMESHPROTO,
		VT_MODEL,
		VT_PARTICLE_FX,
		VT_MORPHMESH,
		VT_DECAL,
		VT_MESHSOFTSKIN
	};

	// Rebuild VTable
	virtual zBOOL Render(zTRenderContext& renderContext) = 0;
	virtual zBOOL IsBBox3DLocal() = 0;
	virtual zTBBox3D GetBBox3D() = 0;
	virtual zCOBBox3D* GetOBBox3D() = 0;
	virtual zSTRING GetVisualName() = 0;
	virtual zBOOL GetVisualDied() = 0;
	virtual void SetVisualUsedBy(zCVob* vob) = 0;
	virtual void DynLightVisual(const zCArray<zCVobLight*>& vobLightList, Matrix*trafoObjToWorld = 0) = 0;
	virtual DWORD GetRenderSortKey() const = 0;
	virtual zBOOL CanTraceRay() const = 0;
	virtual zBOOL TraceRay(const float3& rayOrigin, const float3& ray, const int traceFlags, zTTraceRayReport &report);
	virtual void HostVobRemovedFromWorld(zCVob* hostVob, zCWorld* hostWorld) = 0;
	virtual void HostVobAddedToWorld(zCVob* hostVob, zCWorld* hostWorld) = 0;
	virtual const zSTRING* __GetFileExtension(int i) = 0;
	virtual void GetLODVisualAndAlpha(const float distToCam, zCVisual* &vis, float& alpha) = 0;
	virtual zBOOL GetAlphaTestingEnabled() const = 0;
	virtual void SetAlphaTestingEnabled(const zBOOL a_btest) = 0;


	/** File extension this visual uses. Handy for finding out what class this is */
	const char* GetFileExtension(int i)
	{
		if(__GetFileExtension(i))
			return __GetFileExtension(i)->ToChar();

		return "";
	}

	/** Returns the class-type of this visual */
	EVisualType GetVisualType()
	{
		std::string c = GetClassDef()->GetClassName();

		//LogInfo() << "GVT: " << c;

		if (c == "zCProgMeshProto")
			return VT_PROGMESHPROTO;
		else if (c == "zCMeshSoftSkin")
			return VT_MESHSOFTSKIN;
		else if (c == "zCMorphMesh")
			return VT_MORPHMESH;
		else if (c == "zCModel")
			return VT_MODEL;
		else if (c == "zCParticleFX")
			return VT_PARTICLE_FX;
		else if (c == "zCDecal")
			return VT_DECAL;

		return VT_OTHER;
		
	}

protected:

	zCVisual* m_NextLODVisual;
	zCVisual* m_PrevLODVisual;
	float m_LodFarDistance;
	float m_LodNearFadeOutDistance;
};

class zCVisualAnimate : public zCVisual
{
	// Only some virtual functions we don't need
};