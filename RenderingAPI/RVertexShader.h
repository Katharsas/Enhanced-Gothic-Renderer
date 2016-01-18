#pragma once

#ifdef RND_D3D11
#include "RD3D11VertexShader.h"
#define RVSBASE_API RD3D11VertexShader
#endif


class RVertexShader :
	public RVSBASE_API
{
public:
	RVertexShader();
	~RVertexShader();

	/**
	 * Loads the given shader
	 */
	bool LoadShader(const std::string& file, const std::vector<std::vector<std::string>>& definitions = std::vector<std::vector<std::string>>());
	bool LoadShaderFromString(const std::string& shadercode, const std::vector<std::vector<std::string>>& definitions = std::vector<std::vector<std::string>>());
};

