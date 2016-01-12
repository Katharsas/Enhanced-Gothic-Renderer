#pragma once
#include "RD3D11Shader.h"
class RD3D11PixelShader :
	public RD3D11Shader
{
public:
	RD3D11PixelShader();
	~RD3D11PixelShader();

	/**
	* Loads the given shader
	*/
	bool LoadShaderAPI();

	/**
	* Getters
	*/
	ID3D11PixelShader* GetShader(){return Shader;}

protected:
	// Shader
	ID3D11PixelShader* Shader;
};

