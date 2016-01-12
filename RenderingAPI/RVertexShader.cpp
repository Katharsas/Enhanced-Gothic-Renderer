#include "pch.h"
#include "RVertexShader.h"
#include "../Shared/Logger.h"

RVertexShader::RVertexShader()
{
}


RVertexShader::~RVertexShader()
{
}

/**
* Loads the given shader
*/
bool RVertexShader::LoadShader(const std::string & file, const std::vector<std::vector<std::string>>& definitions)
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
