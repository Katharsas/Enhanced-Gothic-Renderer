#pragma once
#include "GzObjectExtension.h"

class zCCamera;

class GCamera : public GzObjectExtension<zCCamera, GCamera>
{
public:
	GCamera(zCCamera* sourceObject);
	~GCamera(void);

private:
};

