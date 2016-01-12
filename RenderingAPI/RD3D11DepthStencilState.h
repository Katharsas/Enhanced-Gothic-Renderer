#pragma once
#include "rbasedepthstencilstate.h"
class RD3D11DepthStencilState :
	public RBaseDepthStencilState
{
public:
	RD3D11DepthStencilState(void);
	~RD3D11DepthStencilState(void);


	/**
	 * API-Version of CreateState
	 */
	bool CreateStateAPI();

	/** Getters */
	ID3D11DepthStencilState* GetState(){return State;}
private:
	// State object
	ID3D11DepthStencilState* State;
};

