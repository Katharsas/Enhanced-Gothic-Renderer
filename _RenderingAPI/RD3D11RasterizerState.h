#pragma once
#include "rbaserasterizerstate.h"
class RD3D11RasterizerState :
	public RBaseRasterizerState
{
public:
	RD3D11RasterizerState(void);
	~RD3D11RasterizerState(void);

	/**
	 * API-Version of CreateState
	 */
	bool CreateStateAPI();

	/** Getters */
	ID3D11RasterizerState* GetState(){return State;}
private:
	// State object
	ID3D11RasterizerState* State;
};

