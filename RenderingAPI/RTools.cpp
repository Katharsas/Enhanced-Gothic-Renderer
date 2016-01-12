#include "pch.h"
#include "RTools.h"
#include "RVertexShader.h"
#include "REngine.h"
#include "RTools.h"
#include <functional>
#include "RDepthStencilState.h"
#include "RBlendState.h"
#include "RSamplerState.h"
#include "RRasterizerState.h"

namespace RTools
{
	/**
	 * Creates default instances of the state-objects and caches them using the 'default'-alias
	 */
	void MakeDefaultStates(RDepthStencilState** defDSS, RSamplerState** defSS, RBlendState** defBS, RRasterizerState** defRS)
	{
		// Create the default versions of the states
		RDepthStencilState* dss = REngine::ResourceCache->CreateResource<RDepthStencilState>();
		RDepthStencilStateInfo dssinfo = RDepthStencilStateInfo().SetDefault();
		dss->CreateState(dssinfo);

		RSamplerState* ss = REngine::ResourceCache->CreateResource<RSamplerState>();
		ss->CreateState(RSamplerStateInfo().SetDefault());

		RBlendState* bs = REngine::ResourceCache->CreateResource<RBlendState>();
		bs->CreateState(RBlendStateInfo().SetDefault());

		RRasterizerState* rs = REngine::ResourceCache->CreateResource<RRasterizerState>();
		RRasterizerStateInfo rsinfo = RRasterizerStateInfo().SetDefault();
		rs->CreateState(rsinfo);

		// Now cache them all
		REngine::ResourceCache->AddToCache("default", dss);
		REngine::ResourceCache->AddToCache("default", ss);
		REngine::ResourceCache->AddToCache("default", bs);
		REngine::ResourceCache->AddToCache("default", rs);

		// And output
		*defDSS = dss;
		*defSS = ss;
		*defRS = rs;
		*defBS = bs;
	}
};