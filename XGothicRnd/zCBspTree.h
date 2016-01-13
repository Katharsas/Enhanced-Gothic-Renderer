#pragma once
#include "zDefinitions.h"
#include "zCArray.h"
#include <vector>
#include "zCPolygon.h"
#include "zSTRING.h"
#include "GVobObject.h"
#include "zCVob.h"
#include "zCMaterial.h"

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
	/** Recursively walks through connected sectors and adds them to visibleVobs */
	void AddSectorVobsRec(const float3& cameraPosition, std::vector<GVobObject*>& visibleVobs, unsigned int frame, zCBspSector* lastSector)
	{
		//return if this sector has already been processed to avoid running in circles
		if (m_Rendered == frame)
			return;
		
		m_Rendered = frame;

		//add all vobs from the current sector to visibleVobs
		for (int i = 0; i < m_SectorNodes.GetSize(); i++)
		{
			zCBspLeaf* leaf = (zCBspLeaf*)m_SectorNodes.Array[i];
			for (unsigned int j = 0; j < leaf->LeafVobList.NumInArray; j++)
			{
				GVobObject* vob = leaf->LeafVobList.Array[j]->GetVobObject();
				if (vob && vob->UpdateObjectCollectionState(frame))
				{
					visibleVobs.push_back(vob);
				}
			}
		}

		//recursively call  the function for all sectors connected to this sector via a sectorportal (except for outdoor -> indoor portals)
		for (int i = 0; i < m_SectorPortals.GetSize(); i++)
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

