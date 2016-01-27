#pragma once
#include "../Shared/SimpleMath.h"
#include <set>
#include <vector>
#include "REngine.h"
#include "RResourceCache.h"
#include "RInputLayout.h"
#include "../Shared/Toolbox.h"
#include "RRasterizerState.h"
#include "RBlendState.h"
#include "RDepthStencilState.h"
#include "RSamplerState.h"
#include "RLineRenderer.h"
#include "RTweakBar.h"
#include "RPixelShader.h"
#include "RVertexShader.h"

/**
 * Useful function for handling rendering specific things
 */
class RVertexShader;
class RDepthStencilState;
class RSamplerState;
class RBlendState;
class RRasterizerState;
struct RRasterizerStateInfo;
struct RDepthStencilStateInfo;
struct RBlendStateInfo;
struct RSamplerStateInfo;
namespace RTools
{
	/** Line renderer */
	__declspec(selectany) RLineRenderer LineRenderer;
	__declspec(selectany) RTweakBar TweakBar;

	/** Shader loading functions, which also cache the objects by using the alias as hash */
	//static RVertexShader* LoadVertexShader(const std::string& file, 
	//	const std::string& alias, 
	//	const std::vector<std::vector<std::string>>& definitions);

	/** Reloads all shaders */
	static void ReloadShaders()
	{
		auto& psmap = REngine::ResourceCache->GetCacheMap<RPixelShader>();
		auto& vsmap = REngine::ResourceCache->GetCacheMap<RVertexShader>();

		for(auto s : psmap)
			((RPixelShader*)s.second)->ReloadShader();

		for(auto s : vsmap)
			((RVertexShader*)s.second)->ReloadShader();
	}

	/** Generic state creation function */
	template<typename T, typename S>
	static T* GetState(const S& info)
	{
		size_t hash = Toolbox::HashObject(info);
		T* s = REngine::ResourceCache->GetCachedObject<T>(hash);

		// Create states if missing
		if (!s)
		{
			s = REngine::ResourceCache->CreateResource<T>();
			s->CreateState(info);
			REngine::ResourceCache->AddToCache(hash, s);
		}

		return s;
	}

	/** State-creation wrappers */
	static RRasterizerState* GetState(const RRasterizerStateInfo& info)
	{
		return GetState<RRasterizerState, RRasterizerStateInfo>(info);
	}

	static RDepthStencilState* GetState(const RDepthStencilStateInfo& info)
	{
		return GetState<RDepthStencilState, RDepthStencilStateInfo>(info);
	}

	static RBlendState* GetState(const RBlendStateInfo& info)
	{
		return GetState<RBlendState, RBlendStateInfo>(info);
	}

	static RSamplerState* GetState(const RSamplerStateInfo& info)
	{
		return GetState<RSamplerState, RSamplerStateInfo>(info);
	}

	

	/** Shader loading functions, which also cache the objects by using the alias as hash */
	template<typename T>
	static T* LoadShader(const std::string& file, 
		const std::string& alias, 
		const std::vector<std::vector<std::string>>& definitions = std::vector<std::vector<std::string>>())
	{
		// Check if this was already loaded
		RResourceCache& cache = *REngine::ResourceCache;
		T* shader = cache.GetCachedObject<T>(alias);

		if(shader)
			return shader;

		// Not in cache, load it
		shader = cache.CreateResource<T>();
		if(!shader->LoadShader(file, definitions))
			return false;

		// Add it to cache
		cache.AddToCache(alias, shader);

		return shader;
	}

	/** Shader loading functions, which also cache the objects by using the alias as hash */
	template<typename T>
	static T* LoadShaderFromString(const std::string& shadercode, 
		const std::string& alias, 
		const std::vector<std::vector<std::string>>& definitions = std::vector<std::vector<std::string>>())
	{
		// Check if this was already loaded
		RResourceCache& cache = *REngine::ResourceCache;
		T* shader = cache.GetCachedObject<T>(alias);

		if(shader)
			return shader;

		// Not in cache, load it
		shader = cache.CreateResource<T>();
		if(!shader->LoadShaderFromString(shadercode, definitions))
			return false;

		// Add it to cache
		cache.AddToCache(alias, shader);

		return shader;
	}

	/** Creates an InputLayout for the given vertextype an decleration. It will be cached as well, so this
		is save to be called multiple times for the same layout.
		The Vertexdesc has to have an INPUT_LAYOUT_DESC static member. */
	template<typename T>
	static RInputLayout* CreateInputLayoutFor(RVertexShader* vsTemplate)
	{
		// Try to fetch one from cache
		RResourceCache& cache = *REngine::ResourceCache;
		RInputLayout* layout = cache.GetCachedObject<RInputLayout>(std::hash<void*>()((void*)T::INPUT_LAYOUT_DESC));

		if(layout)
			return layout;

		// Create a new one
		layout = cache.CreateResource<RInputLayout>();
		layout->CreateInputLayout(vsTemplate, T::INPUT_LAYOUT_DESC, ARRAYSIZE(T::INPUT_LAYOUT_DESC));

		cache.AddToCache(std::hash<void*>()((void*)T::INPUT_LAYOUT_DESC), layout);

		return layout;
	}

	/**
	 * Creates default instances of the state-objects and caches them using the 'default'-alias
	 */
	void MakeDefaultStates(RDepthStencilState** defDSS, RSamplerState** defSS, RBlendState** defBS, RRasterizerState** defRS);

	/** Indexes the given vertex array 
	 *	T: Vertex-Type
	 *	I: Index-Type
	 *	C: Comparison-class, defining the ()-operator and
	 *			static void FoundDuplicate(T& toStay, const T& toRemove)
	 */
	template<typename C, typename T, typename I>
	static void IndexVertices(T* input, unsigned int numInputVertices, std::vector<T>& outVertices, std::vector<I>& outIndices)
	{
		std::set<std::pair<T, int>, C> vertices;
		int index = 0;

		for (unsigned int i=0; i<numInputVertices; i++)
		{
			std::set<std::pair<T, int>, C>::iterator it = vertices.find(std::make_pair(input[i], 0/*this value doesn't matter*/));
			if (it!=vertices.end())
			{
				std::pair<T, int> vxNew = (*it);
				vertices.erase(it);
				C::FoundDuplicate(vxNew.first, input[i]);
				vertices.insert(vxNew);
				outIndices.push_back(vxNew.second);
			}
			else
			{
				/*char* c = (char *)&input[i].color;
				c[0] = rand() % 255;
				c[1] = rand() % 255;
				c[2] = rand() % 255;*/

				vertices.insert(std::make_pair(input[i], index));
				outIndices.push_back(index++);
			}
		}
		/*
		// Check for overlaying triangles and throw them out
		// Some mods do that for the worldmesh for example
		std::set<std::tuple<I,I,I>> triangles;
		for(unsigned int i=0;i<outIndices.size();i+=3)
		{
			// Insert every combination of indices here. Duplicates will be ignored
			triangles.insert(std::make_tuple(outIndices[i+0], outIndices[i+1], outIndices[i+2]));
		}

		// Extract the cleaned triangles to the indices vector
		outIndices.clear();
		for(auto it = triangles.begin(); it != triangles.end(); it++)
		{
			outIndices.push_back(std::get<0>((*it)));
			outIndices.push_back(std::get<1>((*it)));
			outIndices.push_back(std::get<2>((*it)));
		}
		*/

		// Notice that the vertices in the set are not sorted by the index
		// so you'll have to rearrange them like this:
		outVertices.clear();
		outVertices.resize(vertices.size());
		for (auto it=vertices.begin(); it!=vertices.end(); it++)
		{
			if((unsigned int)it->second >= vertices.size())
			{
				continue;
			}

			outVertices[it->second] = it->first;
		}
	}

	/** Converts a triangle fan to a list */
	template<typename T>
	static void TriangleFanToList(T* input, unsigned int numInputVertices, std::vector<T>& outVertices)
	{
		for(unsigned int i=1;i<numInputVertices-1;i++)
		{
			outVertices.push_back(input[0]);
			outVertices.push_back(input[i+1]);
			outVertices.push_back(input[i]);
		}
	}

	/**
	 * Calculates the tangent-vector to the given triangle */
	static float3 CalculateTangent(const float3& position1, const float2& vuv1, 
		const float3& position2, const float2& vuv2,
		const float3& position3, const float2& vuv3)
	{
		float3 v1 = position2 - position1;
		float3 v2 = position3 - position1;

		float2 uv1 = float2(vuv2.x - vuv1.x, vuv2.y - vuv1.y);
		float2 uv2 = float2(vuv3.x - vuv1.x, vuv3.y - vuv1.y);

		// Calculate the denominator of the tangent/binormal equation.
		float den = 1.0f / (uv1.x * uv2.y - uv2.x * uv1.y);

		// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
		float3 tangent;
		tangent.x = (uv2.y * v1.x - uv1.y * v2.x) * den;
		tangent.y = (uv2.y * v1.y - uv1.y * v2.y) * den;
		tangent.z = (uv2.y * v1.z - uv1.y * v2.z) * den;

		tangent.Normalize();

		//bitangent.x = (uv1.x * v2.x - uv2.x * v1.x) * den;
		//bitangent.y = (uv1.x * v2.y - uv2.x * v1.y) * den;
		//bitangent.z = (uv1.x * v2.z -uv2.x * v1.z) * den;
		//bitangent.Normalize();

		return tangent;
	}

	/**
	 * Converts a DWORD-Color to float4 
	 */
	static float4 DWORDToFloat4(DWORD color)
	{
		BYTE a = color >> 24;
		BYTE r = (color >> 16) & 0xFF;
		BYTE g = (color >> 8) & 0xFF;
		BYTE b = color & 0xFF;

		return float4(r / 255.0f,
			g / 255.0f,
			b / 255.0f,
			a / 255.0f);
	}

	/**
	 * Converts a float4-Color to DWORD. Values are clamped to 1. Stored as argb, where a is the 4th byte.
	 */
	static DWORD float4ToDWORD(const float4& color)
	{
		DWORD dw;
		byte* v = (byte*)&dw;
		v[0] = std::min((byte)255, (byte)(color.z * 255.0f));
		v[1] = std::min((byte)255, (byte)(color.y * 255.0f));
		v[2] = std::min((byte)255, (byte)(color.x * 255.0f));
		v[3] = std::min((byte)255, (byte)(color.w * 255.0f));

		return dw;
	}
};