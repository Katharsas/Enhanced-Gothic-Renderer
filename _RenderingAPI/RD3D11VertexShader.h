#pragma once
#include "RD3D11Shader.h"

class RD3D11VertexShader :
	public RD3D11Shader
{
public:
	RD3D11VertexShader();
	~RD3D11VertexShader();

	/**
	* Loads the given shader
	*/
	bool LoadShaderAPI();

	/**
	* Getters
	*/
	ID3D11VertexShader* GetShader(){return Shader;}

protected:
	// Shader
	ID3D11VertexShader* Shader;
};

