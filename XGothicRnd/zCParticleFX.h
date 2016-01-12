#pragma once
#include "zCVisual.h"
#include "zDefinitions.h"
#include "zCTimer.h"
#include "zCTexture.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

class zCTexture;
class zCParticleFX;

enum zTPFX_EmitterShape {
	zPFX_EMITTER_SHAPE_POINT,
	zPFX_EMITTER_SHAPE_LINE,
	zPFX_EMITTER_SHAPE_BOX,	
	zPFX_EMITTER_SHAPE_CIRCLE,
	zPFX_EMITTER_SHAPE_SPHERE,
	zPFX_EMITTER_SHAPE_MESH
};

enum zTPFX_EmitterFOR {
	zPFX_FOR_WORLD,
	zPFX_FOR_OBJECT,
	zPFX_FOR_OBJECT_EACH_FRAME
};

enum zTPFX_EmitterDirMode {
	zPFX_EMITTER_DIRMODE_NONE,
	zPFX_EMITTER_DIRMODE_DIR,
	zPFX_EMITTER_DIRMODE_TARGET,
	zPFX_EMITTER_DIRMODE_MESH
};

enum zTPFX_EmitterVisOrient {
	zPFX_EMITTER_VISORIENT_NONE,
	zPFX_EMITTER_VISORIENT_VELO_ALIGNED,
	zPFX_EMITTER_VISORIENT_VOB_XZPLANE,
	zPFX_EMITTER_VISORIENT_VELO_ALIGNED3D,
};

enum zTPFX_DistribType {
	zPFX_EMITTER_DISTRIBTYPE_RAND,
	zPFX_EMITTER_DISTRIBTYPE_UNIFORM,
	zPFX_EMITTER_DISTRIBTYPE_WALK,
};

enum zTPFX_FlockMode
{
	zPFX_FLOCK_NONE,
	zPFX_FLOCK_WIND,
	zPFX_FLOCK_WIND_PLANTS,
};


struct zTParticle {
	zTParticle* m_Next;
	float3 m_Position;
	float3 m_PositionWS;
	float3 m_Vel;
	float m_LifeSpan;
	float m_Alpha;
	float m_AlphaVel;
	float2 m_Size;
	float2 m_SizeVel;
	float3 m_Color;
	float3 m_ColorVel;
	class zCPolyStrip* m_PolyStrip;
};

// =======================================================================================================================

class zCParticleEmitter {
public:
	float					ppsValue;
	zSTRING					ppsScaleKeys_S;
	int						ppsIsLooping;
	int						ppsIsSmooth;
	float					ppsFPS;
	zSTRING					ppsCreateEm_S;
	float					ppsCreateEmDelay;
	zSTRING					shpType_S;					
	zSTRING					shpFOR_S;					
	zSTRING					shpOffsetVec_S;				
	zSTRING					shpDistribType_S;			
	float					shpDistribWalkSpeed;
	int						shpIsVolume;
	zSTRING					shpDim_S;					
	zSTRING					shpMesh_S;					
	int						shpMeshRender_B;
	zSTRING					shpScaleKeys_S;				
	int						shpScaleIsLooping;
	int						shpScaleIsSmooth;
	float					shpScaleFPS;
	zSTRING					dirMode_S;					
	zSTRING					dirFOR_S;					
	zSTRING					dirModeTargetFOR_S;
	zSTRING					dirModeTargetPos_S;			
	float					dirAngleHead;
	float					dirAngleHeadVar;
	float					dirAngleElev;
	float					dirAngleElevVar;
	float					velAvg;
	float					velVar;
	float					lspPartAvg;
	float					lspPartVar;
	zSTRING					flyGravity_S;
	int						flyCollDet_B;
	zSTRING					visName_S;					
	zSTRING					visOrientation_S;
	int						visTexIsQuadPoly;			
	float					visTexAniFPS;
	int						visTexAniIsLooping;																
	zSTRING					visTexColorStart_S;
	zSTRING					visTexColorEnd_S;
	zSTRING					visSizeStart_S;
	float					visSizeEndScale;
	zSTRING					visAlphaFunc_S;
	float					visAlphaStart;
	float					visAlphaEnd;
	float					trlFadeSpeed;
	zSTRING					trlTexture_S;
	float					trlWidth;
	float					mrkFadeSpeed;
	zSTRING					mrkTexture_S;
	float					mrkSize;
	zSTRING					m_flockMode_S;
	float					m_fFlockWeight;
	zBOOL					m_bSlowLocalFOR;
	zSTRING					m_timeStartEnd_S;
	zBOOL					m_bIsAmbientPFX;
	int						endOfDScriptPart;
	zSTRING					particleFXName;			
	zCArray<float>			ppsScaleKeys;
	zCParticleEmitter		*ppsCreateEmitter;
	zTPFX_EmitterShape		shpType;					
	float					shpCircleSphereRadius;
	float3					shpLineBoxDim;
	float3*					shpMeshLastPolyNormal;
	class zCMesh			*shpMesh;
	class zCProgMeshProto	*shpProgMesh;
	zCModel					*shpModel;
	zTPFX_EmitterFOR		shpFOR;
	zTPFX_DistribType		shpDistribType;
	float3					shpOffsetVec;
	zCArray<float>			shpScaleKeys;																	
	zTPFX_EmitterDirMode	dirMode;					
	zTPFX_EmitterFOR		dirFOR;						
	zTPFX_EmitterFOR		dirModeTargetFOR;
	float3					dirModeTargetPos;			
	zTBBox3D				dirAngleBox;
	float3					dirAngleBoxDim;
	float3					flyGravity;
	zCTexture				*visTexture;
	zTPFX_EmitterVisOrient	visOrientation;
	float2					visSizeStart;
	float3					visTexColorRGBAStart;
	float3					visTexColorRGBAEnd;
	zTRnd_AlphaBlendFunc	visAlphaFunc;
	zCTexture				*trlTexture;
	zCTexture				*mrkTexture;
	zBOOL					 isOneShotFX;
	float					dirAngleHeadVarRad;
	float					dirAngleElevVarRad;
	zTPFX_FlockMode			m_flockMode;
	float					m_ooAlphaDist;
	float					m_startTime;
	float					m_endTime;
};

class zCParticleEmitterVars {
public:
	float					ppsScaleKeysActFrame;
	float					ppsNumParticlesFraction;
	float					ppsTotalLifeTime;
	zBOOL					ppsDependentEmitterCreated;
	float					shpScaleKeysActFrame;
	float					uniformValue;
	float					uniformDelta;
};

class zCStaticPfxList 
{
public:
	void TouchPfx(zCParticleFX *pfx)
	{
		XCALL(MemoryLocations::Gothic::zCParticleFX__zCStaticPfxList__TouchPfx_zCParticleFX_p);
	}

	static zCStaticPfxList* GetStaticPfxList()
	{
		return (zCStaticPfxList*)MemoryLocations::Gothic::zCParticleFX__zCStaticPfxList_zCParticleFX__s_pfxList;
	}

	zCParticleFX* m_PfxListHead;
	zCParticleFX* m_PfxListTail;
	int	m_NumInPfxList;
};

class zCParticleFX : public zCVisual
{
public:

	/** Prepares this pfx for rendering */
	bool PrepareForRendering()
	{
		// Advance simulation
		UpdateTime();
		CheckDependentEmitter();

		// Don't do anything more if empty
		if(!m_FirstPart)
		{
			FinalizeUpdate();
			return false;
		}

		// Check if the texture is loaded
		if(!m_Emitter->visTexture || m_Emitter->visTexture->CacheIn(0.6f) != zRES_CACHED_IN)
		{
			FinalizeUpdate();
			return false;
		}

		return true;
	}

	/** Closes the rendering-process on this particle-fx */
	void FinalizeUpdate()
	{
		CreateParticlesUpdateDependencies();
		zCStaticPfxList::GetStaticPfxList()->TouchPfx(this);
	}

	/** Checks if this/the next one is a dead particle and cleans it from the list if so. */
	void ProcessParticle(zTParticle* p)
	{
		zTParticle* kill = p;
	
		// First particle
		if(!p)
		{
			for(;;)
			{
				kill = m_FirstPart;
				if(kill && (kill->m_LifeSpan < m_PrivateTotalTime))
				{
					if(kill->m_PolyStrip)
						((zCObject *)kill->m_PolyStrip)->Release();

					m_FirstPart = kill->m_Next;
		
					kill->m_Next = *(zTParticle **)MemoryLocations::Gothic::zSParticle_p_zCParticleFX__s_globFreePart;
					*(zTParticle **)MemoryLocations::Gothic::zSParticle_p_zCParticleFX__s_globFreePart = kill;
					continue;
				}
				break;
			}
		}
		else
		{
			// Any other particle
			while(true)
			{
				kill = p->m_Next;
				if(kill && (kill->m_LifeSpan < m_PrivateTotalTime))
				{
					if(kill->m_PolyStrip)
						((zCObject *)kill->m_PolyStrip)->Release();

					p->m_Next = kill->m_Next;

					kill->m_Next = *(zTParticle **)MemoryLocations::Gothic::zSParticle_p_zCParticleFX__s_globFreePart;
					*(zTParticle **)MemoryLocations::Gothic::zSParticle_p_zCParticleFX__s_globFreePart = kill;
					continue;
				}
				break;
			}
		}
	}

	static float SinEase(float value)
	{
		return (float)((sin(value * M_PI - M_PI / 2.0) + 1.0) / 2.0);
	}

	static float SinSmooth(float value)
	{
		if(value < 0.5f) 
			return SinEase(value * 2);
		else		  
			return 1.0f - SinEase((value - 0.5f) * 2);
	}

	void CheckDependentEmitter()
	{
		XCALL(MemoryLocations::Gothic::zCParticleFX__CheckDependentEmitter_void);
	}

	void CreateParticlesUpdateDependencies()
	{
		XCALL(MemoryLocations::Gothic::zCParticleFX__CreateParticlesUpdateDependencies_void);
	}

	void UpdateParticle(zTParticle* p)
	{
		XCALL(MemoryLocations::Gothic::zCParticleFX__UpdateParticle_zSParticle_p);
	}

	void UpdateTime()
	{
		m_LocalFrameTimeF = (m_TimeScale * zCTimer::GetTimer()->frameTimeFloat);
	}

	zCParticleEmitter* GetEmitter()
	{
		return m_Emitter;
	}

	zTParticle* GetFirstParticle()
	{
		return m_FirstPart;
	}
private:

	

	// First particle in active list
	zTParticle* m_FirstPart;
	zCParticleEmitterVars m_EmitterVars;

	// Emitter this was made from
	zCParticleEmitter* m_Emitter;

	// World-Space AABB of this PFX
	zTBBox3D			m_BBox3DWorld;

	// This is one of the visuals which are bound to only a single vob.
	zCVob* m_ConnectedVob;

	// Last time we updated the boundingbox
	int	m_BBoxUpdateCtr;

	// Flags
	struct {
		byte m_EmitterIsOwned			: 1;
		byte m_DontKillPFXWhenDone		: 1;
		byte m_Dead						: 1;
		byte m_IsOneShotFX				: 1;	
		byte m_ForceEveryFrameUpdate	: 1;	
		byte m_RenderUnderWaterOnly		: 1;
	};

	zCParticleFX* m_NextPfx;
	zCParticleFX* m_PrevPfx;
	float m_PrivateTotalTime;
	float m_LastTimeRendered;
	float m_TimeScale;
	float m_LocalFrameTimeF;
	class zCQuadMark* m_QuadMark;
	zTBBox3D m_QuadMarkBBox3DWorld;
	float m_BboxYRangeInv;
	zBOOL m_bVisualNeverDies;
};