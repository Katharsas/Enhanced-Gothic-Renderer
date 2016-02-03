#include "pch.h"
#include "GMaterial.h"
#include <RResourceCache.h>
#include <REngine.h>
#include "zCMaterial.h"
#include "GTexture.h"
#include "zCTexture.h"
#include <RRasterizerState.h>
#include <RTools.h>

using namespace GConstants;

GMaterial::GMaterial(zCMaterial* sourceMaterial) : GzObjectExtension<zCMaterial, GMaterial>(sourceMaterial)
{
	m_Diffuse = nullptr;
	m_Name = sourceMaterial->GetObjectName();
}


GMaterial::~GMaterial(void)
{

}

/**
 * Checks if all textures are loaded. If not, will place a cache-in-request
 */
bool GMaterial::CacheTextures(bool checkOnly)
{
	// TODO: Maybe only check for in debug builds
	if(m_Invalid)
	{
		LogWarn() << "Operating on invalid material: " << m_Name;
		return false;
	}

	// Do we even have a texture?
	if(!m_SourceObject->GetTexture())
		return false; // TODO: No texture means black in gothic. Gregs hat, for example!

	if(checkOnly)
		return m_Diffuse->GetSourceObject()->GetCacheState() == zRES_CACHED_IN;

	// Check if the texture is still right
	if(!m_Diffuse || m_Diffuse->GetSourceObject() != m_SourceObject->GetTexture())
	{
		m_Diffuse = GTexture::GetFromSource(m_SourceObject->GetTexture());

		if(!m_Diffuse)
		{
			LogWarn() << "Couldn't find diffuse for material " << m_SourceObject;
			return false; 
		}
	}
	
	// Ask for cache in
	return m_Diffuse->CacheIn() == zRES_CACHED_IN;
}

/**
 * Applies this material to the state-machine
 */
void GMaterial::ApplyStates()
{
	RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();

	CacheTextures();

	if(!m_Diffuse)
		return; // TODO: Black texture or material-color instead!

	sm.SetTexture(0, m_Diffuse->GetTexture(), RAPI::EShaderType::ST_PIXEL);

	// Turn on two-sided rendering if this is masked or animated
	if (IsMaterialUsingAlphaTest() || m_SourceObject->GetRootAniTexture()->GetTextureFlags().IsAnimated)
	{
		RAPI::RRasterizerStateInfo info = sm.GetCurrentState().RasterizerState->GetStateInfo();
		info.CullMode = RAPI::RRasterizerStateInfo::CM_CULL_NONE;
		sm.SetRasterizerState(RAPI::RTools::GetState(info));
	}

	// Alphablending
	if(m_SourceObject->GetBlendFunc() == zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_BLEND
		|| m_SourceObject->GetBlendFunc() == zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_BLEND_TEST)
	{
		RAPI::RBlendStateInfo info = sm.GetCurrentState().BlendState->GetStateInfo();
		info.SetAlphaBlending();
		sm.SetBlendState(RAPI::RTools::GetState(info));

		// Disable depth-writes
		RAPI::RDepthStencilStateInfo dsInfo = sm.GetCurrentState().DepthStencilState->GetStateInfo();
		dsInfo.DepthWriteEnabled = false;
		sm.SetDepthStencilState(RAPI::RTools::GetState(dsInfo));

	}else if(m_SourceObject->GetBlendFunc() == zTRnd_AlphaBlendFunc::zRND_ALPHA_FUNC_ADD)
	{
		RAPI::RBlendStateInfo info = sm.GetCurrentState().BlendState->GetStateInfo();
		info.SetAdditiveBlending();
		sm.SetBlendState(RAPI::RTools::GetState(info));

		// Disable depth-writes
		RAPI::RDepthStencilStateInfo dsInfo = sm.GetCurrentState().DepthStencilState->GetStateInfo();
		dsInfo.DepthWriteEnabled = false;
		sm.SetDepthStencilState(RAPI::RTools::GetState(dsInfo));
	}

	// TODO: Other blendstates
}

/**
 * Returns whether this materials uses alphatesting or not
 * Be aware that the texture must be cached in for this to give accurate results!
 */
bool GMaterial::IsMaterialUsingAlphaTest()
{
	if (!m_Diffuse)
		return false;

	if (m_SourceObject->GetBlendFunc() > zRND_ALPHA_FUNC_BLEND
		&& m_SourceObject->GetBlendFunc() < zRND_ALPHA_FUNC_TEST)
		return false;

	if (m_SourceObject->GetBlendFunc() == zRND_ALPHA_FUNC_TEST)
		return true;

	// This is only valid if the texture is cached in!
	if (m_SourceObject->GetTexture() && m_SourceObject->GetTexture()->GetTextureFlags().HasAlpha)
		return true;

	return false;
}

/**
* Returns the right pixelshader for this material and the given rendering stage
*/
RAPI::RPixelShader* GMaterial::GetMaterialPixelShader(GConstants::ERenderStage stage, UINT flags)
{
	switch (stage)
	{
	case RS_WORLD:
	case RS_SHADOW_SUN:
	case RS_INVENTORY:
		if((flags & MPS_LIGHTMAPPED) != 0)
			return RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>(ShaderAliases::PS_DEFAULT_WORLD_LIGHTMAPPED);

		if(!IsMaterialUsingAlphaTest() && (flags & MPS_FORCE_ALPHA_TEST) == 0)
			return RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>(ShaderAliases::PS_DEFAULT_WORLD);
		else
			return RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>(ShaderAliases::PS_MASKED_WORLD);

	break;
	}

	LogError() << "Invalid rendering stage!";

	return nullptr;
}