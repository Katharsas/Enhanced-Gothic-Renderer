#pragma once
#include "pch.h"

#ifdef RND_D3D11
#include "RD3D11TweakBar.h"
#define RTWEAKBARBASE_API RD3D11TweakBar
#endif

enum ETValMode
{
	TVM_ReadWrite,
	TVM_Read
};


class RTweakBar : public RTWEAKBARBASE_API
{
public:
	RTweakBar();
	~RTweakBar();

	/** Initializes the tweakbar */
	bool Init();

	/** On window message */
	LRESULT OnWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/** Draws the bars */
	void Draw();

	/** Adds values to the bar */
	void AddFloatRW(const char* table, float* value, const char* name, float min = FLT_MIN, float max = FLT_MAX, float step = 0.1f);
	void AddFloatR(const char* table, float* value, const char* name, float min = FLT_MIN, float max = FLT_MAX, float step = 0.1f);
	void AddIntRW(const char* table, int* value, const char* name, float min = INT_MIN, float max = INT_MAX, float step = 1);
	void AddIntR(const char* table, int* value, const char* name, float min = INT_MIN, float max = INT_MAX, float step = 1);
	void AddBoolRW(const char* table, bool* value, const char* name);
	void AddBoolR(const char* table, bool* value, const char* name);

protected:
	/** Returns the bar of the given name. Creates one if not present */
	TwBar* GetBarByName(const char* name);
};

