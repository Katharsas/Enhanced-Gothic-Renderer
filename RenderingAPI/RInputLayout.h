#pragma once
#include "pch.h"

#ifdef RND_D3D11
#include "RD3D11InputLayout.h"
#define RINPUTLAYOUTBASE_API RD3D11InputLayout
#endif

class RInputLayout :
	public RINPUTLAYOUTBASE_API
{
public:
	RInputLayout();
	~RInputLayout();

	/**
	* Creates the inputlayout using the given input decleration
	*/
	bool CreateInputLayout(class RVertexShader* vertexshadertemplate, const INPUT_ELEMENT_DESC* inputElementDesc, unsigned int numInputDescElements);
};

