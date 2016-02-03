#include "pch.h"
#include "RD3D11PixelShader.h"


RD3D11PixelShader::RD3D11PixelShader()
{
	Shader = nullptr;
}


RD3D11PixelShader::~RD3D11PixelShader()
{
	SafeRelease(Shader);
}

/**
* Loads the given shader
*/
bool RD3D11PixelShader::LoadShaderAPI()
{
	ID3D11PixelShader* old = Shader;

	// Compile and load
	Shader = (ID3D11PixelShader*)CompileShaderAPI("PSMain", "ps_4_0", ST_PIXEL);

	// Keep the old shader, in case this one failed
	if(Shader)
		SafeRelease(old);
	else
		Shader = old;

	return Shader != NULL;
}