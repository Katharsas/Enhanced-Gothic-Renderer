#include "pch.h"
#include "RPixelShader.h"
#include "../Shared/Logger.h"

RPixelShader::RPixelShader()
{
}


RPixelShader::~RPixelShader()
{
}

/**
* Loads the given shader
*/
bool RPixelShader::LoadShader(const std::string & file, const std::vector<std::vector<std::string>>& definitions)
{
	LogInfo() << "Compilling shader: " << file;

	// Make sure this is only used once
	if(!ShaderFile.empty())
	{
		LogWarn() << "Shader '" << ShaderFile << "' already loaded. Can't overwrite with: " << file << ". Create a new object!";
		return false;
	}

	ShaderFile = file;
	Definitions = definitions;

	return LoadShaderAPI();
}
