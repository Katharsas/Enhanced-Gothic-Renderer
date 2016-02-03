#pragma once

#include <string>
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

/** Defines types used for the project */

/** Errorcodes */
enum XRESULT
{
	XR_SUCCESS,
	XR_FAILED,
	XR_INVALID_ARG,
};

struct INT2
{
	INT2(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	INT2(const Vector2& v)
	{
		this->x = (int)(v.x + 0.5f);
		this->y = (int)(v.y + 0.5f);
	}

	INT2(){}

	std::string toString()
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}

	bool operator== (const INT2& v)
	{
		return x == v.x && y == v.y;
	}

	bool operator!= (const INT2& v)
	{
		return x != v.x || y != v.y;
	}

	int x;
	int y;
};


struct INT4
{
	INT4(int x, int y, int z, int w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	INT4(){}

	int x;
	int y;
	int z;
	int w;
};

