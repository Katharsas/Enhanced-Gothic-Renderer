#pragma once
#include "zEngineFunctions.h"
#include "MemoryLocations.h"
#include "zCObject.h"
#include "zClassDef.h"

enum zTWeather
{
	zTWEATHER_SNOW,
	zTWEATHER_RAIN
};

enum zTAnimationMode {
	zVISUAL_ANIMODE_NONE = 0,
	zVISUAL_ANIMODE_WIND = 1,
	zVISUAL_ANIMODE_WIND2 = 2,
};

enum zTCamLocationHint
{
	zCAM_OUTSIDE_SECTOR,
	zCAM_INSIDE_SECTOR_CANT_SEE_OUTSIDE,
	zCAM_INSIDE_SECTOR_CAN_SEE_OUTSIDE,
};

class zCTexture;
class zCPolygon;
class zCMesh;

class zCSkyController : public zCObject
{
public:
	/** Static accessors */
	static void SetSkyEffectsEnabled(zBOOL b) { *(zBOOL *)MemoryLocations::Gothic::int_zCSkyControler__s_skyEffectsEnabled = b; };
	static zBOOL GetSkyEffectsEnabled() { return *(zBOOL *)MemoryLocations::Gothic::int_zCSkyControler__s_skyEffectsEnabled; };
	static zCSkyController* GetActiveSkyControler() { return *(zCSkyController**)MemoryLocations::Gothic::zCSkyControler_p_zCSkyControler__s_activeSkyControler; };

	virtual void SetTime(const float time) = 0; // 0..1 (0=12h, 0.5=24h)
	virtual float GetTime() const = 0;
	virtual void ResetTime() = 0;
	virtual void SetFarZ(const float z) = 0;
	virtual float GetFarZ() const = 0;
	virtual void SetFarZScalability(const float zscale) = 0;
	virtual float GetFarZScalability() const = 0;
	virtual void SetBackgroundColor(const DWORD col) = 0;

private:
	// This is compiled with one argument instead of none, which
	// seems to be a pointer to the location of where to put the data.
	// Returnvalue is the passed pointer again.
	// Use GetBackgroundColor() (no __) for a better interface.
	virtual DWORD* __GetBackgroundColor(DWORD* dw) const = 0;
public:

	DWORD GetBackgroundColor()
	{
		DWORD dw;
		__GetBackgroundColor(&dw);
		return dw;
	}

	virtual DWORD GetBackgroundColorDef() const = 0;
	virtual void SetOverrideColor(const float3 col) = 0;
	virtual void SetOverrideColorFlag(const zBOOL b) = 0;
	virtual float GetCloudShadowScale()const = 0;
	virtual void SetCloudShadowScale(const float f) = 0;
	virtual void SetFillBackground(const zBOOL b) = 0;
	virtual zBOOL GetFillBackground() const = 0;
	virtual void SetUnderwaterFX(const zBOOL b) = 0;
	virtual zBOOL GetUnderwaterFX() const = 0;
	virtual void UpdateWorldDependencies() = 0;
	virtual void SetLightDirty() = 0;
	virtual void SetRelightTime(const float a_fLightPerfCtr) = 0;

	virtual unsigned int GetRelightCtr() = 0;
	virtual DWORD GetDaylightColorFromIntensity(int intensity) = 0;
	virtual void RenderSkyPre() = 0;
	virtual void RenderSkyPost(const zBOOL renderSkyMesh) = 0;
	virtual zBOOL GetGlobalWindVec(float3 &resVec, const zTAnimationMode a_aniMode) = 0;
	virtual void SetGlobalSkyScale(const float skyScale) = 0;
	virtual float GetGlobalSkyScale() const = 0;
	virtual zBOOL GetGlobalSkyScaleChanged() const = 0;

	virtual void SetCameraLocationHint(const zTCamLocationHint camLocHint) = 0;
	virtual void SetWeatherType(const zTWeather a_weather) = 0;
	virtual zTWeather GetWeatherType() const = 0;

	/** Returns the daylight coloring-array */
	DWORD* GetPolyLightCLUT()
	{
		return m_PolyLightCLUTPtr;
	}

protected:

	// This is for coloring the world during different daytimes
	// Points to a 256-sized DWORD-array
	DWORD* m_PolyLightCLUTPtr;

	// Cloudshadows for when it's raining
	float m_CloudShadowScale;

	// Whether the game should update the worlds color
	zBOOL m_ColorChanged;

	// Current weather type
	zTWeather m_EnuWeather;

	// Same as Clear- and Fog-Color
	DWORD m_BackgroundColor;

	// If true, clear the color-buffer, if false, only clear zBuffer
	zBOOL m_FillBackground;

	// Background texture for everything. Not sure where this is used, probably menus.
	zCTexture* m_BackgroundTexture;

	// Last times this was relighted
	unsigned int m_RelightCtr;
	float m_LastRelightTime;
	float m_RelightTime;
};

class zCSkyController_Mid : public zCSkyController
{
public:
	// Whether to draw underwater-fx
	zBOOL m_UnderwaterFX;
	DWORD m_UnderwaterColor;
	float m_UnderwaterFarZ;
	float m_UnderwaterStartTime;

	// For camera effects
	float m_OldFovX;
	float m_OldFovY;

	// Particle-effect for being under water
	zCVob *m_VobUnderwaterPFX;

	// Screen-blend-effect when underwater?
	zCPolygon *m_ScrPoly;
	zCMesh *m_ScrPolyMesh;
	int m_ScrPolyAlpha;
	DWORD m_ScrPolyColor;
	zTRnd_AlphaBlendFunc m_ScrPolyAlphaFunc;
};


enum zESkyLayerMode 
{ 
	zSKY_MODE_POLY, 
	zSKY_MODE_BOX 
};

const int zSKY_NUM_LAYER = 2;
const int NUM_PLANETS = 2;

class zCSkyLayerData
{
public:
	zESkyLayerMode m_SkyMode;
	zCTexture* m_Tex;
	zSTRING m_TexName;
	float m_TexAlpha;
	float m_TexScale;
	float2 m_TexSpeed;
};

enum zTSkyStateEffect
{
	zSKY_STATE_EFFECT_SUN,
	zSKY_STATE_EFFECT_CLOUDSHADOW,
};

class zCSkyState
{
public:
	float m_Time;
	float3 m_PolyColor;
	float3 m_FogColor;
	float3 m_DomeColor1;
	float3 m_DomeColor0;
	float m_FogDist;
	zBOOL m_SunOn;
	int m_CloudShadowOn;
	zCSkyLayerData m_Layer[zSKY_NUM_LAYER];
};

class zCSkyLayer {
public:
	zCMesh* m_SkyPolyMesh;
	zCPolygon* m_SkyPoly;
	float2 m_SkyTexOffs;
	zCMesh* m_SkyDomeMesh;
	zESkyLayerMode m_SkyMode;
};

class zCSkyPlanet {
public:
	zCMesh *mesh;
	float4 color0;
	float4 color1;
	float size;
	float3 pos;
	float3 rotAxis;
};

class zCOutdoorRainFX;
class zCSkyController_Outdoor : public zCSkyController_Mid
{
public:
	/** Returns the current main fog-color */
	DWORD GetFogColor()
	{
		return m_ResultFogColor;
	}

	/** Returns near- and farplanes for distancefog */
	void GetFogPlanes(float& fogNear, float& fogFar)
	{
		fogNear = m_ResultFogNear;
		fogFar = m_ResultFogFar;
	}

	/** Returns the active sky controller, if it is an outdoor one. nullptr otherwiese. */
	static zCSkyController_Outdoor* GetActiveSkyControllerAsOutdoor()
	{
		zCSkyController* sky = zCSkyController::GetActiveSkyControler();

		if(sky && strcmp(sky->GetClassDef()->GetClassName(), "zCSkyControler_Outdoor") == 0)
		{
			return (zCSkyController_Outdoor*)sky;
		}

		return nullptr;
	}

private:
	zBOOL m_InitDone;
	float m_MasterTime;
	float m_MasterTimeLast;
	zCSkyState m_MasterState;
	zCSkyState* m_State0;
	zCSkyState* m_State1;
	zCArray<zCSkyState*> m_StateList;
	DWORD m_PolyLightCLUT[256];
	float m_DayCounter;
	zCArray<float3> m_FogColorDayVariations;
	zCArray<float3> m_FogColorDayVariations2;
	float m_SkyScale;
	zBOOL m_SkyScaleChanged;
	float3 m_OverrideColor;
	zBOOL m_OverrideColorFlag;
	zBOOL m_DontRain;
	zBOOL m_LevelChanged;
	zBOOL m_DarkSky;

	float m_ResultFogScale;
	float m_HeightFogMinY;
	float m_HeightFogMaxY;
	float m_UserFogFar;
	float m_ResultFogNear;
	float m_ResultFogFar;
	float m_ResultFogSkyNear;
	float m_ResultFogSkyFar;
	DWORD m_ResultFogColor;
	DWORD m_ResultFogColorOverride;
	float m_UserFarZScalability;

	zCSkyState* m_SkyLayerState[2];
	zCSkyLayer m_SkyLayer[2];
	zCSkyLayer m_SkyLayerRainClouds;
	zCTexture* m_SkyCloudLayerTex;
	zCSkyPlanet m_Planets[NUM_PLANETS];
	zBOOL m_SunVisible;
	float m_FadeScale;
	zCVob * m_VobSkyPFX;
	float m_SkyPFXTimer;
	zBOOL m_IsMainControler;
	float3 m_WindVec;

	struct zTRainFX {
		zCOutdoorRainFX * m_OutdoorRainFX;
		zTCamLocationHint m_CamLocationHint;
		float m_OutdoorRainFXWeight;
		float m_SoundVolume;
		float m_TimerInsideSectorCantSeeOutside; 
		float m_TimeStartRain;
		float m_TimeStopRain;
		zBOOL m_RenderLightning;
		zBOOL m_Raining; 
		int m_RainCtr;

	} rainFX;
};