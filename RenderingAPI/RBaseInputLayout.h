#pragma once
#include "pch.h"
#include "RResource.h"

// Layed out for D3D11
struct INPUT_ELEMENT_DESC
{
	const char* SemanticName;
	UINT SemanticIndex;
	EFormat Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	EInputClassification InputSlotClass;
	UINT InstanceDataStepRate;
};

class RBaseInputLayout : public RResource
{
public:
	RBaseInputLayout();
	virtual ~RBaseInputLayout();

protected:
	// Descriptor for the input layout
	INPUT_ELEMENT_DESC* InputElementDesc;
	unsigned int NumInputDescElements;
};

