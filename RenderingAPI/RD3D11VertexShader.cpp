#include "pch.h"
#include "RD3D11VertexShader.h"
#include "../Shared/Logger.h"
#include "REngine.h"

RD3D11VertexShader::RD3D11VertexShader()
{
	Shader = nullptr;
}


RD3D11VertexShader::~RD3D11VertexShader()
{
	SafeRelease(Shader);
}

/**
* Loads the given shader
*/
bool RD3D11VertexShader::LoadShaderAPI()
{
	ID3D11VertexShader* old = Shader;

	// Compile and load
	Shader = (ID3D11VertexShader*)CompileShaderAPI("VSMain", "vs_4_0", ST_VERTEX);

	// Keep the old shader, in case this one failed
	if(Shader)
		SafeRelease(old);
	else
		Shader = old;

	return Shader != NULL;
}
