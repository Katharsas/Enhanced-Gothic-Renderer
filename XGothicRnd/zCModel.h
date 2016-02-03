#pragma once
#include "pch.h"
#include "zCVisual.h"
#include "zCModelMeshLib.h"



class zCModelAni : public zCObject
{
public:

	bool IsIdle()
	{
		return false;//m_FlagIdle && m_NumFrames <= 1;
	}

private:
	zSTRING	m_AniName;		
	zSTRING	m_AscName;
	int	m_AniID;

	zSTRING	m_AliasName;
	zCList<zCModelAni> m_CombAniList;

	int	m_Layer;
	float m_BlendInSpeed;			
	float m_BlendOutSpeed;
	zTBBox3D m_AniBBox3DObjSpace;
	float m_CollisionVolumeScale;

	zCModelAni* m_NextAni;
	zSTRING	m_NextAniName;
	class zCModelAniEvent* m_AniEvents;

	float m_FpsRate;
	float m_FpsRateSource;
	int m_RootNodeIndex;

	zCArray<int> m_NodeIndexList;
	zCModelNode** m_NodeList;
	struct zTMdl_AniSample* m_AniSampleMatrix;
	float m_SamplePosRangeMin;
	float m_SamplePosScaler;

	struct {
		int m_NumFrames		: 16;		// table-y
		int m_NumNodes		: 16;		// table-x
		int m_AniType		: 6;
		int m_AniDir		: 2;
		int m_NumAniEvents	: 6;		// maximum of 64 aniEvents
	};
	struct {
		int m_FlagVobRot		: 1;
		int m_FlagVobPos		: 1;
		int m_FlagEndSync		: 1;
		int m_FlagFly			: 1;
		int m_FlagIdle			: 1;			
		int m_FlagInPlace		: 1;			
		int m_FlagStaticCycle	: 1;			
	};
};

struct zCModelAniActive
{
	zCModelAni* m_Animation;

	// .. more here
};

class zCModel : public zCVisualAnimate
{
public:
	struct zTMdl_NodeVobAttachment 
	{
		zCVob* m_Vob;
		zCModelNodeInst* m_Mnode;
	};

	struct zTMdl_StartedVobFX 
	{
		zCVob* m_Vob;
		float m_VobFXHandle;
	};

	struct zTAniMeshLibEntry 
	{
		struct zCModelAniActive* m_Ani;
		zCModelMeshLib * m_MeshLib;
	};

	struct zTMeshLibEntry 
	{
		zCModelTexAniState m_TexAniState;
		zCModelMeshLib * m_MeshLib;
	};


	struct ModelFlags {
		byte IsVisible : 1;
		byte IsFlying : 1;
		byte RandAnisEnabled : 1;
		byte LerpSamples : 1;
		byte ModelScaleOn : 1;
		byte DoVobRot : 1;
		byte NodeShadowEnabled : 1;
		byte DynLightMode : 1;
	};

	/** Returns the fatness of this model */
	float GetModelFatness()
	{
		return m_Fatness;
	}

	/** Returns the scale of this model */
	float GetModelScale()
	{
		return m_ModelScale;
	}

	/** Returns the number of nodes we have in the skeleton */
	unsigned int GetNumNodes()
	{
		return m_NodeList.NumInArray;
	}

	/** Returns the list of nodes of this model */
	zCArray<zCModelNodeInst*>& GetNodeList()
	{
		return m_NodeList;
	}

	/** Updates stuff like blinking eyes, etc */
	void zCModel::UpdateMeshLibTexAniState() 
	{
		for (unsigned int i=0; i<m_MeshLibList.NumInArray; i++) 
			m_MeshLibList.Array[i]->m_TexAniState.UpdateTexList();
	}

	/* Updates the world matrices of the attached VOBs */
	void UpdateAttachedVobs()
	{
		XCALL(MemoryLocations::Gothic::zCModel__UpdateAttachedVobs_void); 
	}

	/** Fills a vector of (viewspace) node-transformation matrices for this frame.
		'transformsArray' must have at least the same number of places as m_NodeList.NumInArray. */
	void GetNodeTransforms(Matrix* transformsArray)
	{
		static std::vector<Matrix*> tptr; // Static to keep memory around and avoid reallocation
		tptr.resize(m_NodeList.NumInArray, NULL);

		for (unsigned int i = 0; i<m_NodeList.NumInArray; i++)
		{
			zCModelNodeInst* node = m_NodeList.Array[i];
			zCModelNodeInst* parent = node->m_ParentNode;
			tptr[i] = &node->m_TrafoObjToCam;

			// Calculate transform for this node
			if (parent)
				node->m_TrafoObjToCam = parent->m_TrafoObjToCam * node->m_Trafo;
			else
				node->m_TrafoObjToCam = node->m_Trafo;
		}
		// Put them into our array
		for (unsigned int i = 0; i<tptr.size(); i++)
		{
			transformsArray[i] = *tptr[i];
		}
	}

	/** Returns true if this model is currently in the idle state and does not need to be updated */
	bool IsInIdle()
	{
		return m_ActiveAniList == nullptr || !m_ActiveAniList->m_Animation || m_ActiveAniList->m_Animation->IsIdle();
	}

	/** Returns the amount if mesh librarys we have */
	unsigned int GetNumModelMeshLibs()
	{
		return m_MeshLibList.NumInArray;
	}

	/** Returns the array of mesh-libs this model is using */
	zTMeshLibEntry** GetMeshLibArray()
	{
		return m_MeshLibList.Array;
	}

	/** Returns the single vob using this visual */
	zCVob* GetHomeVob()
	{
		return m_HomeVob;
	}

	/** This value MUST be set some time in the frame. Otherwise the
		animations will look jerky. You can use the actual distance
		or simply 0 to make all animations smooth */
	void SetModelDistanceToCam(float dist)
	{
		m_ModelDistanceToCam = dist;
	}
private:

	int	m_NumActiveAnis;
	zCModelAniActive* m_AniChannels[6];
	zCModelAniActive* m_ActiveAniList;

#if DATASET_VERSION == VERSION_2_6_FIX
	zCArray<int> m_ListOfVoiceHandles;
#endif
							
	// Vob using this visual. zCModels are attached to exactly one vob.
	zCVob* m_HomeVob;


	zCArray<class zCModelPrototype*> m_ModelProtoList;
	zCArray<zCModelNodeInst*> m_NodeList;
	zCArray<zCMeshSoftSkin*> m_MeshSoftSkinList;
	zCArray<class zTAniAttachment*>	m_AniAttachList;
	byte aniAttachData[4];
		
	zCArray<zTMdl_NodeVobAttachment> m_AttachedVobList;
	zCArray<zTMdl_StartedVobFX> m_StartedVobFX;
	zCArray<zTAniMeshLibEntry> m_AniMeshLibList;

	// This is where we get our data from
	zCArray<zTMeshLibEntry*> m_MeshLibList;

	unsigned int m_LastTimeBBox3DTreeUpdate;

	zCArray<class zCModelAniEvent*> m_OccuredAniEvents;

	// Boundingboxes
	zTBBox3D m_BBox3D; // World
	zTBBox3D m_BBox3DLocalFixed; // Local
	zTBBox3D m_BBox3DCollDet; // Local

	// This value MUST be set some time in the frame. Otherwise the
	// animations will look jerky. You can use the actual distance
	// or simply 0 to make all animations smooth
	float m_ModelDistanceToCam;

#if DATASET_VERSION == VERSION_2_6_FIX
	zBOOL m_IsInMobInteraction;
#endif

	// Fatness of the model					
	float m_Fatness;

	// Scale of the model. Actually not in the world-Matrix for this...
	float m_ModelScale;

	// General stuff
	float3 m_AniTransScale;
	float3 m_RootPosLocal;
	float3 m_VobTrans;

#if DATASET_VERSION == VERSION_2_6_FIX
	float3 m_VobTransRing;
	zBOOL m_NewAniStarted;
	zBOOL m_SmoothRootNode;
	float m_RelaxWeight;

	// True when in firstperson-mode
	zBOOL m_DrawHandVisualsOnly;
#endif

	// Used internally 
	Quaternion m_VobRotation;
	float3 m_ModelVelocity;

	// Values for the water-splash-rings
	int m_ActVelRingPos;
	float3 m_ModelVelRing[8];

	// Flags-struct
	ModelFlags m_Flags;

	// Animation timescale
	float m_TimeScale;

	byte data2[4];
};