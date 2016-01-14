#pragma once
#include "GzObjectExtension.h"
#include "GConstants.h"

class zCMaterial;
class RTexture;
class GTexture;

enum EMaterialPSFlags
{
	MPS_NONE = 0,
	MPS_LIGHTMAPPED = 1,
	MPS_FORCE_ALPHA_TEST = 2
};

/**
 * Material wrapper 
 */
class GMaterial : public GzObjectExtension<zCMaterial, GMaterial>
{
public:
	GMaterial(zCMaterial* sourceMaterial);
	~GMaterial(void);

	/**
	* Returns the diffuse-texture
	*/
	GTexture* GetDiffuse(){return m_Diffuse;}

	/**
	 * Checks if all textures are loaded. If not, will place a cache-in-request
	 */
	bool CacheTextures(bool checkOnly = false);

	/**
	 * Applies this material to the state-machine
	 */
	void ApplyStates();

	/**
	 * Returns whether this materials uses alphatesting or not
	 * Be aware that the texture must be cached in for this to give accurate results!
	 */
	bool IsMaterialUsingAlphaTest();


	/**
	 * Returns the right pixelshader for this material and the given rendering stage
	 * Flags of type EMaterialPSFlags
	 */
	RPixelShader* GetMaterialPixelShader(GConstants::ERenderStage stage, UINT flags = MPS_NONE);
protected:
	// Pointer to diffuse-texture
	GTexture* m_Diffuse;

	// Name of the material
	std::string m_Name;
};

