#include "pch.h"
#include "RD3D11InputLayout.h"
#include "REngine.h"
#include "../Shared/Logger.h"
#include "RVertexShader.h"

RD3D11InputLayout::RD3D11InputLayout()
{
}


RD3D11InputLayout::~RD3D11InputLayout()
{
}

/**
* Creates the inputlayout using the given input decleration
*/
bool RD3D11InputLayout::CreateInputLayoutAPI(RVertexShader* vertexshadertemplate)
{
	HRESULT hr;

	// Create inputlayout
	LE(REngine::RenderingDevice->GetDevice()->CreateInputLayout((D3D11_INPUT_ELEMENT_DESC*)InputElementDesc, NumInputDescElements, vertexshadertemplate->GetShaderData()->GetBufferPointer(),
		vertexshadertemplate->GetShaderData()->GetBufferSize(), &InputLayout));

	return hr == S_OK;
}
