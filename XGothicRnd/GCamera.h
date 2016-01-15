#pragma once
#include "GzObjectExtension.h"

class zCCamera;

class GCamera : public GzObjectExtension<zCCamera, GCamera>
{
public:
	GCamera(zCCamera* sourceObject);
	~GCamera(void);

	/** Returns the position of this camera */
	const float3& GetCameraPosition();

	/** Returns the direction of this camera */
	const float3& GetDirection();

	/** Returns the near and farplanes */
	float GetFarZ();
	float GetNearZ();

	/** Returns the currently active camera */
	static GCamera* GetActiveCamera();

private:
};

