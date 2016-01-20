#pragma once
#include "zDefinitions.h"
#include "zCArray.h"
#include <vector>
#include "zCPolygon.h"
#include "zSTRING.h"
#include "GVobObject.h"
#include "zCVob.h"
#include "zCMaterial.h"
#include "zCCamera.h"

#define zSECTOR_INDEX_UNDEF	(-1)
#define zSECTOR_INDEX_PORTAL (1<<15)

enum zTTraceRayFlags 
{
	zTRACERAY_VOB_IGNORE_NO_CD_DYN		= 1<<0,		
	zTRACERAY_VOB_IGNORE				= 1<<1,		
	zTRACERAY_VOB_BBOX					= 1<<2,		
	zTRACERAY_VOB_OBB					= 1<<3,		
													
													
	zTRACERAY_STAT_IGNORE				= 1<<4,		
													
	zTRACERAY_STAT_POLY					= 1<<5,		
	zTRACERAY_STAT_PORTALS				= 1<<6,		

													
	zTRACERAY_POLY_NORMAL				= 1<<7,		
	zTRACERAY_POLY_IGNORE_TRANSP		= 1<<8,		
	zTRACERAY_POLY_TEST_WATER			= 1<<9,		
	zTRACERAY_POLY_2SIDED				= 1<<10,	
	zTRACERAY_VOB_IGNORE_CHARACTER		= 1<<11,	

													
	zTRACERAY_FIRSTHIT					= 1<<12,	
	zTRACERAY_VOB_TEST_HELPER_VISUALS	= 1<<13,	
	zTRACERAY_VOB_IGNORE_PROJECTILES	= 1<<14,
};

struct zCBspNode;
struct zCBspLeaf;
struct zCBspBase;
class zCMesh;
class zCPolygon;
class zCVob;
class zCVobLight;
class GVobObject;

struct zCBspBase
{
	zCBspNode* m_Parent;
	zTBBox3D m_BBox3D;
	zCPolygon** m_PolyList;
	unsigned int m_NumPolys;
	zTBspNodeType m_NodeType;
};

struct zCBspLeaf : public zCBspBase
{
	int LastTimeLighted;
	zCArray<zCVob*>	LeafVobList;
	zCArray<zCVobLight*> LightVobList;
};

struct zCBspNode : public zCBspBase
{
	/** Returns only the polygons used in LOD0 of the world */
	void GetLOD0Polygons(std::vector<zCPolygon *>& target)
	{
		for (int i = 0; i < m_NumLeafs; i++)
		{
			zCBspLeaf* leaf = &m_LeafList[i];

			for (unsigned int j = 0; j < leaf->m_NumPolys; j++)
			{
				target.push_back(leaf->m_PolyList[j]);
			}
		}
	}

	Plane	m_Plane;
	zCBspBase* m_Front;
	zCBspBase* m_Back;
	zCBspLeaf* m_LeafList;
	int m_NumLeafs;
	char m_PlaneSignbits;
};

struct zCBspSector
{
	void AddSectorVobsRec(const float3& cameraPosition, std::vector<GVobObject*>& visibleVobs, unsigned int frame, zCBspSector* lastSector)
	{
		if (m_Rendered == frame)
			return;
		
		m_Rendered = frame;
		for (unsigned int i = 0; i < m_SectorNodes.GetSize(); i++)
		{
			zCBspBase* base = m_SectorNodes.Array[i];

			zCBspLeaf* leaf = (zCBspLeaf*)base;
			for (unsigned int j = 0; j < leaf->LeafVobList.NumInArray; j++)
			{
				GVobObject* vob = leaf->LeafVobList.Array[j]->GetVobObject();
				if (vob && vob->UpdateObjectCollectionState(frame))
				{
					visibleVobs.push_back(vob);
				}
			}
		}

		for (unsigned int i = 0; i < m_SectorPortals.GetSize(); i++)
		{
			zCPolygon* p = m_SectorPortals.Array[i];

			// no outdoor -> indoor portals
			if (p->GetMaterial()->GetSectorFront())
			{
				zCBspSector* back = p->GetMaterial()->GetSectorBack();
				if (back && lastSector != back)
				{
					back->AddSectorVobsRec(cameraPosition, visibleVobs, frame, this);
				}
			}
		}
	}

	struct zTPortalInfo {
		byte visible;
		byte alpha;
	};

	zSTRING m_SectorName;
	zCArray<zCBspBase*> m_SectorNodes;
	DWORD m_SectorIndex;
	zCArray<zCPolygon*> m_SectorPortals;
	zCArray<zTPortalInfo> m_SectorPortalInfo;
	UINT m_Activated;
	UINT m_Rendered;
	zTBBox2D m_ActivePortal;
	float3 m_SectorCenter;
	byte m_HasBigNoFade;
};


class zCBspTree
{
public:
	/** Returns only the polygons used in LOD0 of the world */
	void GetLOD0Polygons(std::vector<zCPolygon *>& target)
	{
		for (int i = 0; i < m_NumLeafs; i++)
		{
			zCBspLeaf* leaf = &m_LeafList[i];

			for (unsigned int j = 0; j < leaf->m_NumPolys; j++)
			{
				target.push_back(leaf->m_PolyList[j]);
			}
		}
	}

	/** Returns the root-node of this tree */
	zCBspBase* GetRootNode()
	{
		return m_BSPRoot;
	}

	/** Returns how many nodes this tree contains. This also contains leafs. */
	unsigned int GetNumNodes()
	{
		return m_NumNodes;
	}

	/** Returns the sector the camera is currently in.
		Does a raycast, so don't call this too often. */
	zCBspSector* GetCurrentCameraSector()
	{
		// Start at the current cameraposition and trace down to check the poly we are on, including portals
		float3 start = zCCamera::GetActiveCameraPosition();
		float3 hitPoint;
		zCPolygon* hitPolygon = nullptr;

		TraceRay(start, start + float3(0, -500000, 0), zTRACERAY_STAT_POLY | zTRACERAY_STAT_PORTALS, hitPoint, hitPolygon, nullptr);


		// Check:
		if(	!hitPolygon
			|| !hitPolygon->GetPolyFlags().SectorPoly 
			|| (hitPolygon->GetPolyFlags().PortalPoly && !hitPolygon->GetMaterial()->GetSectorFront())) // Outdoor-facing portal?
			return nullptr;

		// Get the sector of the direction the poly is facing
		return hitPolygon->GetMaterial()->GetSectorFront();
	}

	/** Does a raytrace using the games original code. This is rather slow
		start: Startposition of the ray
		end: Endposition of the ray
		traceFlags: What to trace (zTTraceRayFlags)
		intersectionPoint: Point where the ray hit 
		vobList: List of vobs we have hit on the way */
	zBOOL TraceRay(const float3& start, const float3& end, int traceFlags, float3& intersectionPoint, zCPolygon* &hitPoly, zCArray<zCVob*> *vobList)
	{
		XCALL(MemoryLocations::Gothic::zCBspTree__TraceRay_zVEC3_const_r_zVEC3_const_r_int_zVEC3_r_zCPolygon_p_r_zCArrayzCVob_p_p);
	}

	/** Returns the number of polygons in the tree */
	size_t GetNumPolygons(){return m_NumPolys;}

	/** Returns the polygon-array of the tree */
	zCPolygon** GetPolygons(){return m_TreePolyList;}

private:
	zCBspNode* m_ActiveNodePtr;
	zCBspLeaf* m_ActiveLeafPtr;
	zCBspBase* m_BSPRoot;
	zCMesh* m_Mesh;
	zCPolygon** m_TreePolyList;
	zCBspNode* m_NodeList;
	zCBspLeaf* m_LeafList;
	int	m_NumNodes;
	int	m_NumLeafs;
	int	m_NumPolys;
	zCArray<zCVob*>	m_RenderVobList;
	zCArray<zCVobLight*> m_RenderLightList;
	zCArray<zCBspSector*> m_SectorList;
	zCArray<zCPolygon*>	m_PortalList;
	zTBspTreeMode m_BspTreeMode;
	zTWld_RenderMode m_WorldRenderMode;
	float m_VobFarClipZ;
	zTPlane	m_VobFarPlane;
	int	m_VobFarPlaneSignbits;
	zBOOL m_DrawVobBBox3D;
	int	m_LeafsRendered;
	int	m_VobsRendered;
};

