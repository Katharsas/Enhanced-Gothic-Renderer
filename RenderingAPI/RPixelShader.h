#pragma once

#ifdef RND_D3D11
#include "RD3D11PixelShader.h"
#define RPSBASE_API RD3D11PixelShader
#endif

class RPixelShader :
	public RPSBASE_API
{
public:
	RPixelShader();
	~RPixelShader();

	/**
	* Loads the given shader
	*/
	bool LoadShader(const std::string& file, const std::vector<std::vector<std::string>>& definitions = std::vector<std::vector<std::string>>());
};

