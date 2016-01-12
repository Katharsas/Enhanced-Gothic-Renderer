#include "pch.h"
#include "GCamera.h"
#include "zCCamera.h"

GCamera::GCamera(zCCamera* sourceObject) : GzObjectExtension<zCCamera, GCamera>(sourceObject)
{
}


GCamera::~GCamera(void)
{
}
