#include "pch.h"
#include "RD3D11VertexShader.h"
#include "../Shared/Logger.h"
#include "REngine.h"

RD3D11VertexShader::RD3D11VertexShader()
{
}


RD3D11VertexShader::~RD3D11VertexShader()
{
}

/**
* Loads the given shader
*/
bool RD3D11VertexShader::LoadShaderAPI()
{
	// Compile and load
	Shader = (ID3D11VertexShader*)CompileShaderAPI("VSMain", "vs_4_0", ST_VERTEX);

	return Shader != NULL;
}
